// Black Ledger - AI combat driver

#include "BLAIController.h"
#include "Core/BLGameInstance.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "HAL/IConsoleManager.h"
#include "Vehicles/BLCombatVehicle.h"
#include "Vehicles/BLHealthComponent.h"
#include "Weapons/BLPickupActor.h"
#include "Weapons/BLWeaponComponent.h"

// live AI debug overlay: line to current objective + state label per vehicle.
//   bl.AIDebug 1
static TAutoConsoleVariable<int32> CVarBLAIDebug(
	TEXT("bl.AIDebug"), 0,
	TEXT("1 = draw AI state labels and target lines"));

namespace
{
	// signed yaw (deg) from the vehicle's forward to a world-space direction
	float SignedYawTo(const FVector& Forward, const FVector& To)
	{
		const FVector F = FVector(Forward.X, Forward.Y, 0.f).GetSafeNormal();
		const FVector T = FVector(To.X, To.Y, 0.f).GetSafeNormal();
		const float Dot = FMath::Clamp(FVector::DotProduct(F, T), -1.f, 1.f);
		const float Deg = FMath::RadiansToDegrees(FMath::Acos(Dot));
		return (FVector::CrossProduct(F, T).Z >= 0.f) ? Deg : -Deg;
	}
}

ABLAIController::ABLAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	// Easy: drives softer, sprays early (wide cone), long rests, rare missiles, less HP
	EasyParams.ThrottleScale = 0.8f;
	EasyParams.FireBurstSeconds = 0.7f;
	EasyParams.FireRestSeconds = 1.2f;
	EasyParams.FireConeDeg = 14.f;
	EasyParams.MissileIntervalSeconds = 8.f;
	EasyParams.HealthScale = 0.8f;

	// Medium: the baseline the tuning sheet balances around
	MediumParams.ThrottleScale = 0.9f;
	MediumParams.FireBurstSeconds = 1.2f;
	MediumParams.FireRestSeconds = 0.6f;
	MediumParams.FireConeDeg = 9.f;
	MediumParams.MissileIntervalSeconds = 5.f;
	MediumParams.HealthScale = 1.f;

	// Hard: relentless - near-continuous fire, tight aim, frequent missiles, more HP
	HardParams.ThrottleScale = 1.f;
	HardParams.FireBurstSeconds = 2.5f;
	HardParams.FireRestSeconds = 0.15f;
	HardParams.FireConeDeg = 6.f;
	HardParams.MissileIntervalSeconds = 2.5f;
	HardParams.HealthScale = 1.2f;
}

void ABLAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (ABLCombatVehicle* V = Cast<ABLCombatVehicle>(InPawn))
	{
		if (V->Health)
		{
			V->Health->ScaleMaxHealth(GetParams().HealthScale);
		}
	}
}

const FBLAIDifficultyParams& ABLAIController::GetParams() const
{
	const UBLGameInstance* GI = Cast<UBLGameInstance>(GetGameInstance());
	switch (GI ? GI->GetDifficulty() : EBLDifficulty::Medium)
	{
	case EBLDifficulty::Easy: return EasyParams;
	case EBLDifficulty::Hard: return HardParams;
	default:                  return MediumParams;
	}
}

ABLCombatVehicle* ABLAIController::GetVehicle() const
{
	return Cast<ABLCombatVehicle>(GetPawn());
}

void ABLAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ABLCombatVehicle* V = GetVehicle();
	if (!V || !V->Health || V->Health->IsDead())
	{
		return; // the pawn zeroes its own inputs when dead
	}

	APawn* Target = FindTarget();
	if (!Target)
	{
		V->SetDriveInput(0.f, 0.f);
		V->Weapon->StopFirePrimary();
		return;
	}

	const FVector MyLoc = V->GetActorLocation();
	const FVector Fwd = V->GetActorForwardVector();
	const FVector ToTarget = Target->GetActorLocation() - MyLoc;
	const float Dist = ToTarget.Size2D();
	const float AngleToTarget = SignedYawTo(Fwd, ToTarget);

	const FBLAIDifficultyParams& Params = GetParams();

	// ---- pick where to drive ----
	FVector DriveTo = ToTarget;
	float Throttle = 1.f;

	ABLPickupActor* Pickup = (V->Weapon->GetPickupAmmo() == 0) ? FindPickup() : nullptr;
	if (Pickup)
	{
		DriveTo = Pickup->GetActorLocation() - MyLoc; // GrabPickup
	}
	else if (Dist < MinFightRange)
	{
		// too close: peel away past the tangent to re-open distance - never park
		// nose-first grinding into the target
		DriveTo = ToTarget.RotateAngleAxis(OrbitSign * PeelAngleDeg, FVector::UpVector);
		Throttle = 0.9f;
	}
	else if (Dist < PursueRange)
	{
		// StrafeTarget: orbit by aiming off to one side of the target
		DriveTo = ToTarget.RotateAngleAxis(OrbitSign * OrbitAngleDeg, FVector::UpVector);
		Throttle = 0.75f;
	}

	float Steer = FMath::Clamp(SignedYawTo(Fwd, DriveTo) / SteerResponseDeg, -1.f, 1.f);

	// ---- debug overlay (bl.AIDebug 1) ----
	if (CVarBLAIDebug.GetValueOnGameThread() != 0)
	{
		const TCHAR* State = Pickup ? TEXT("PICKUP")
			: (ReverseTime > 0.f) ? TEXT("UNSTUCK")
			: (Dist < MinFightRange) ? TEXT("PEEL")
			: (Dist < PursueRange) ? TEXT("STRAFE") : TEXT("PURSUE");
		const FColor StateColor = Pickup ? FColor::Yellow
			: (ReverseTime > 0.f) ? FColor::Red
			: (Dist < MinFightRange) ? FColor::Magenta
			: (Dist < PursueRange) ? FColor::Orange : FColor::Cyan;
		DrawDebugLine(GetWorld(), MyLoc + FVector(0, 0, 120),
			Target->GetActorLocation() + FVector(0, 0, 120), StateColor, false, -1.f, 0, 4.f);
		DrawDebugString(GetWorld(), MyLoc + FVector(0, 0, 320),
			FString::Printf(TEXT("%s  hp %.0f  ammo %d"), State,
				V->Health->GetHealth(), V->Weapon->GetPickupAmmo()),
			nullptr, StateColor, 0.f, true);
	}

	// ---- unstuck: full throttle but not moving -> back out the other way ----
	const float Speed = V->GetVelocity().Size2D();
	if (ReverseTime > 0.f)
	{
		ReverseTime -= DeltaTime;
		V->SetDriveInput(-1.f, -LastSteer);
		V->Weapon->StopFirePrimary();
		return;
	}
	if (Speed < 60.f && Throttle > 0.3f)
	{
		StuckTime += DeltaTime;
		if (StuckTime > 1.5f)
		{
			StuckTime = 0.f;
			ReverseTime = 1.2f;
			OrbitSign = -OrbitSign;
		}
	}
	else
	{
		StuckTime = 0.f;
	}

	LastSteer = Steer;
	V->SetDriveInput(Throttle * Params.ThrottleScale, Steer);

	// ---- weapons: burst/rest duty cycle (difficulty = attack frequency, Bible 4.6) ----
	FireCycleTime += DeltaTime;
	const float CycleLen = Params.FireBurstSeconds + Params.FireRestSeconds;
	const bool bInBurstWindow = FMath::Fmod(FireCycleTime, CycleLen) < Params.FireBurstSeconds;

	const float AbsAngle = FMath::Abs(AngleToTarget);
	if (bInBurstWindow && AbsAngle < Params.FireConeDeg && Dist < FireRange)
	{
		V->Weapon->StartFirePrimary();
	}
	else
	{
		V->Weapon->StopFirePrimary();
	}

	const double Now = GetWorld()->GetTimeSeconds();
	if (V->Weapon->GetPickupAmmo() > 0 && AbsAngle < MissileConeDeg && Dist > MissileMinRange
		&& Now - LastMissileTime >= Params.MissileIntervalSeconds)
	{
		V->Weapon->FirePickup();
		LastMissileTime = Now;
	}
}

APawn* ABLAIController::FindTarget() const
{
	APawn* Self = GetPawn();
	APawn* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();
	for (TActorIterator<ABLCombatVehicle> It(GetWorld()); It; ++It)
	{
		ABLCombatVehicle* Other = *It;
		if (Other == Self || (Other->Health && Other->Health->IsDead()))
		{
			continue;
		}
		const float DistSq = FVector::DistSquared(Other->GetActorLocation(), Self->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = Other;
		}
	}
	return Best;
}

ABLPickupActor* ABLAIController::FindPickup() const
{
	APawn* Self = GetPawn();
	ABLPickupActor* Best = nullptr;
	float BestDistSq = FMath::Square(PickupSeekRange);
	for (TActorIterator<ABLPickupActor> It(GetWorld()); It; ++It)
	{
		ABLPickupActor* P = *It;
		if (P->IsHidden())
		{
			continue; // collected, waiting to respawn
		}
		const float DistSq = FVector::DistSquared(P->GetActorLocation(), Self->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = P;
		}
	}
	return Best;
}
