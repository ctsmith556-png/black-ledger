// Black Ledger - AI combat driver

#include "BLAIController.h"
#include "Core/BLGameInstance.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "HAL/IConsoleManager.h"
#include "Specials/BLSpecialComponent.h"
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

	// Easy: drives softer, sprays early (wide cone), long rests, rare missiles,
	// less HP, notices less of the map
	EasyParams.ThrottleScale = 0.8f;
	EasyParams.FireBurstSeconds = 0.7f;
	EasyParams.FireRestSeconds = 1.2f;
	EasyParams.FireConeDeg = 14.f;
	EasyParams.MissileIntervalSeconds = 8.f;
	EasyParams.HealthScale = 0.8f;
	EasyParams.EngagementScale = 0.75f;

	// Medium: the baseline the tuning sheet balances around
	MediumParams.ThrottleScale = 0.9f;
	MediumParams.FireBurstSeconds = 1.2f;
	MediumParams.FireRestSeconds = 0.6f;
	MediumParams.FireConeDeg = 9.f;
	MediumParams.MissileIntervalSeconds = 5.f;
	MediumParams.HealthScale = 1.f;
	MediumParams.EngagementScale = 1.f;

	// Hard: relentless - near-continuous fire, tight aim, frequent missiles,
	// more HP, wider awareness
	HardParams.ThrottleScale = 1.f;
	HardParams.FireBurstSeconds = 2.5f;
	HardParams.FireRestSeconds = 0.15f;
	HardParams.FireConeDeg = 6.f;
	HardParams.MissileIntervalSeconds = 2.5f;
	HardParams.HealthScale = 1.2f;
	HardParams.EngagementScale = 1.25f;
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
	// desync the wanderlust clocks so the field doesn't detour in unison
	WanderClock = FMath::FRandRange(0.f, DisengageCheckSeconds);
}

ABLCombatVehicle* ABLAIController::GetVehicle() const
{
	return Cast<ABLCombatVehicle>(GetPawn());
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

void ABLAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ABLCombatVehicle* V = GetVehicle();
	if (!V || !V->Health || V->Health->IsDead())
	{
		return; // the pawn zeroes its own inputs when dead
	}

	const FBLAIDifficultyParams& Params = GetParams();
	const FVector MyLoc = V->GetActorLocation();
	const FVector Fwd = V->GetActorForwardVector();

	// ---- wanderlust: sometimes break off and go live a little ----
	if (DisengageTime > 0.f)
	{
		DisengageTime -= DeltaTime;
	}
	else
	{
		WanderClock += DeltaTime;
		if (WanderClock >= DisengageCheckSeconds)
		{
			WanderClock = 0.f;
			if (FMath::FRand() < DisengageChance)
			{
				DisengageTime = FMath::FRandRange(8.f, 14.f);
				CurrentTarget.Reset();
				bHasRoamTarget = false; // pick a fresh detour destination
			}
		}
	}

	APawn* Target = AcquireTarget(Params.EngagementScale);

	// ---- decide where to drive ----
	FVector DriveTo;
	float Throttle = 1.f;
	float AbsAngleToTarget = 180.f;
	ABLPickupActor* Pickup = nullptr;

	if (Target)
	{
		const FVector ToTarget = Target->GetActorLocation() - MyLoc;
		const float Dist = ToTarget.Size2D();
		AbsAngleToTarget = FMath::Abs(SignedYawTo(Fwd, ToTarget));

		Pickup = (bSeekPickups && V->Weapon->GetPickupAmmo() == 0) ? FindPickup() : nullptr;
		if (Pickup)
		{
			DriveTo = Pickup->GetActorLocation() - MyLoc; // GrabPickup
		}
		else if (Dist < MinFightRange)
		{
			// too close: peel away past the tangent - never park nose-first
			DriveTo = ToTarget.RotateAngleAxis(OrbitSign * PeelAngleDeg, FVector::UpVector);
			Throttle = 0.9f;
		}
		else if (Dist < PursueRange)
		{
			// StrafeTarget: orbit by aiming off to one side of the target
			DriveTo = ToTarget.RotateAngleAxis(OrbitSign * OrbitAngleDeg, FVector::UpVector);
			Throttle = 0.75f;
		}
		else
		{
			DriveTo = ToTarget; // pursue
		}

		// ---- weapons: burst/rest duty cycle (difficulty = attack frequency) ----
		FireCycleTime += DeltaTime;
		const float CycleLen = Params.FireBurstSeconds + Params.FireRestSeconds;
		const bool bInBurstWindow = FMath::Fmod(FireCycleTime, CycleLen) < Params.FireBurstSeconds;
		if (bInBurstWindow && AbsAngleToTarget < Params.FireConeDeg && Dist < FireRange)
		{
			V->Weapon->StartFirePrimary();
		}
		else
		{
			V->Weapon->StopFirePrimary();
		}
		const double Now = GetWorld()->GetTimeSeconds();
		if (V->Weapon->GetPickupAmmo() > 0 && AbsAngleToTarget < MissileConeDeg
			&& Dist > MissileMinRange && Now - LastMissileTime >= Params.MissileIntervalSeconds)
		{
			V->Weapon->FirePickup();
			LastMissileTime = Now;
		}
		// UseSpecialWhenReady (TDD section 8): pop the field in a committed close fight
		if (V->Special && Dist < 1400.f)
		{
			V->Special->TryActivate();
		}
	}
	else
	{
		// ---- ROAM: nobody in range (or detouring) - wander the arena.
		// Destinations favor pickups, pulling traffic through conflict lanes. ----
		V->Weapon->StopFirePrimary();
		RoamTime += DeltaTime;
		if (!bHasRoamTarget || RoamTime > 12.f
			|| FVector::DistSquared2D(MyLoc, RoamTarget) < FMath::Square(1800.f))
		{
			RoamTarget = PickRoamPoint();
			bHasRoamTarget = true;
			RoamTime = 0.f;
		}
		DriveTo = RoamTarget - MyLoc;
		Throttle = RoamThrottle;
	}

	// ---- debug overlay (bl.AIDebug 1) ----
	if (CVarBLAIDebug.GetValueOnGameThread() != 0)
	{
		const TCHAR* State = !Target
			? ((DisengageTime > 0.f) ? TEXT("DETOUR") : TEXT("ROAM"))
			: Pickup ? TEXT("PICKUP")
			: (ReverseTime > 0.f) ? TEXT("UNSTUCK") : TEXT("ENGAGE");
		const FColor StateColor = !Target
			? ((DisengageTime > 0.f) ? FColor::White : FColor::Green)
			: Pickup ? FColor::Yellow
			: (ReverseTime > 0.f) ? FColor::Red : FColor::Orange;
		const FVector LineEnd = Target ? Target->GetActorLocation() : RoamTarget;
		DrawDebugLine(GetWorld(), MyLoc + FVector(0, 0, 120),
			LineEnd + FVector(0, 0, 120), StateColor, false, -1.f, 0, 4.f);
		DrawDebugString(GetWorld(), MyLoc + FVector(0, 0, 320),
			FString::Printf(TEXT("%s  hp %.0f  ammo %d"), State,
				V->Health->GetHealth(), V->Weapon->GetPickupAmmo()),
			nullptr, StateColor, 0.f, true);
	}

	// ---- shared: unstuck recovery, then drive ----
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
			bHasRoamTarget = false; // a stuck roamer needs a new destination too
		}
	}
	else
	{
		StuckTime = 0.f;
	}

	const float Steer = FMath::Clamp(SignedYawTo(Fwd, DriveTo) / SteerResponseDeg, -1.f, 1.f);
	LastSteer = Steer;
	V->SetDriveInput(Throttle * Params.ThrottleScale, Steer);
}

APawn* ABLAIController::AcquireTarget(float RangeScale)
{
	APawn* Self = GetPawn();
	// while detouring, only a point-blank brawler can drag us back in
	const float Range = (DisengageTime > 0.f) ? 2500.f : EngagementRange * RangeScale;

	// sticky current target (with leash hysteresis) - no per-tick flip-flopping
	if (ABLCombatVehicle* T = Cast<ABLCombatVehicle>(CurrentTarget.Get()))
	{
		if (T->Health && !T->Health->IsDead()
			&& FVector::DistSquared(T->GetActorLocation(), Self->GetActorLocation())
				< FMath::Square(Range * 1.3f))
		{
			return T;
		}
		CurrentTarget.Reset();
	}

	APawn* Best = nullptr;
	float BestDistSq = FMath::Square(Range);
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
	CurrentTarget = Best;
	return Best;
}

FVector ABLAIController::PickRoamPoint() const
{
	const FVector MyLoc = GetPawn()->GetActorLocation();

	// 70%: head for a random pickup spawn (conflict lanes); 30%: open wandering
	if (FMath::FRand() < 0.7f)
	{
		TArray<FVector> Spots;
		for (TActorIterator<ABLPickupActor> It(GetWorld()); It; ++It)
		{
			Spots.Add(It->GetActorLocation()); // hidden (collected) spots still pull traffic
		}
		if (Spots.Num() > 0)
		{
			return Spots[FMath::RandRange(0, Spots.Num() - 1)];
		}
	}
	const float Angle = FMath::FRandRange(0.f, 2.f * PI);
	const float Dist = FMath::FRandRange(5000.f, 12000.f);
	return MyLoc + FVector(FMath::Cos(Angle) * Dist, FMath::Sin(Angle) * Dist, 0.f);
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
