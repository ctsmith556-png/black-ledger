// Black Ledger - combat vehicle pawn, arcade movement (Phase 1)

#include "BLCombatVehicle.h"
#include "AI/BLAIController.h"
#include "BLHealthComponent.h"
#include "Audio/BLAudioSubsystem.h"
#include "Core/BLGameMode.h"
#include "FX/BLImpactFXSubsystem.h"
#include "Specials/BLSpecialComponent.h"
#include "UI/BLUISubsystem.h"
#include "Weapons/BLWeaponComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	constexpr float KphToCms = 100000.f / 3600.f; // 27.78
}

ABLCombatVehicle::ABLCombatVehicle()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetBoxExtent(FVector(330.f, 128.f, 92.f));
	CollisionBox->SetCollisionProfileName(TEXT("Pawn"));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetSimulatePhysics(true);
	CollisionBox->SetEnableGravity(true);
	CollisionBox->SetLinearDamping(0.05f);
	CollisionBox->SetAngularDamping(2.0f);
	CollisionBox->SetNotifyRigidBodyCollision(true); // ram-damage hit events
	CollisionBox->SetCanEverAffectNavigation(false); // vehicles must not churn navmesh tiles
	CollisionBox->BodyInstance.COMNudge = FVector(0.f, 0.f, -45.f); // keep it planted

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(CollisionBox);
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetCanEverAffectNavigation(false);
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, BodyMeshZOffset));

	auto MakeWheel = [this](const TCHAR* Name) -> UStaticMeshComponent*
	{
		UStaticMeshComponent* W = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		W->SetupAttachment(CollisionBox);
		W->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		W->SetCanEverAffectNavigation(false);
		return W;
	};
	WheelFL = MakeWheel(TEXT("WheelFL"));
	WheelFR = MakeWheel(TEXT("WheelFR"));
	WheelRL = MakeWheel(TEXT("WheelRL"));
	WheelRR = MakeWheel(TEXT("WheelRR"));

	// Surgeon meshes as Phase-1 defaults; UBLCharacterDataAsset takes over later.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyFinder(
		TEXT("/Game/BlackLedger/Characters/Surgeon/SM_Surgeon_Body"));
	if (BodyFinder.Succeeded())
	{
		BodyMesh->SetStaticMesh(BodyFinder.Object);
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WheelFinderFL(
		TEXT("/Game/BlackLedger/Characters/Surgeon/SM_Surgeon_Wheel_FL"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WheelFinderFR(
		TEXT("/Game/BlackLedger/Characters/Surgeon/SM_Surgeon_Wheel_FR"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WheelFinderRL(
		TEXT("/Game/BlackLedger/Characters/Surgeon/SM_Surgeon_Wheel_RL"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WheelFinderRR(
		TEXT("/Game/BlackLedger/Characters/Surgeon/SM_Surgeon_Wheel_RR"));
	if (WheelFinderFL.Succeeded()) { WheelFL->SetStaticMesh(WheelFinderFL.Object); }
	if (WheelFinderFR.Succeeded()) { WheelFR->SetStaticMesh(WheelFinderFR.Object); }
	if (WheelFinderRL.Succeeded()) { WheelRL->SetStaticMesh(WheelFinderRL.Object); }
	if (WheelFinderRR.Succeeded()) { WheelRR->SetStaticMesh(WheelFinderRR.Object); }

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(CollisionBox);
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 90.f));
	CameraBoom->SetRelativeRotation(FRotator(-8.f, 0.f, 0.f));
	CameraBoom->TargetArmLength = 850.f;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 55.f);
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 12.f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 9.f;
	CameraBoom->bDoCollisionTest = false; // chase cam must not collapse onto the roof

	ChaseCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	ChaseCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	Health = CreateDefaultSubobject<UBLHealthComponent>(TEXT("Health"));
	Weapon = CreateDefaultSubobject<UBLWeaponComponent>(TEXT("Weapon"));
	Special = CreateDefaultSubobject<UBLSpecialComponent>(TEXT("Special"));

	// ---- damage-state FX: a translucent smoke plume + a flickering fire light,
	// both off until the chassis is hurt (driven by UpdateDamageFX) ----
	DamageSmoke = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DamageSmoke"));
	DamageSmoke->SetupAttachment(BodyMesh);
	DamageSmoke->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamageSmoke->SetCastShadow(false);
	DamageSmoke->SetCanEverAffectNavigation(false);
	DamageSmoke->SetRelativeLocation(FVector(-40.f, 0.f, 150.f));   // over the engine bay
	DamageSmoke->SetVisibility(false);
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFinder(
			TEXT("/Engine/BasicShapes/Sphere.Sphere"));
		if (SphereFinder.Succeeded()) { DamageSmoke->SetStaticMesh(SphereFinder.Object); }
		static ConstructorHelpers::FObjectFinder<UMaterialInterface> SmokeMatFinder(
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
		if (SmokeMatFinder.Succeeded()) { DamageSmoke->SetMaterial(0, SmokeMatFinder.Object); }
	}

	DamageFire = CreateDefaultSubobject<UPointLightComponent>(TEXT("DamageFire"));
	DamageFire->SetupAttachment(BodyMesh);
	DamageFire->SetRelativeLocation(FVector(-40.f, 0.f, 60.f));
	DamageFire->SetLightColor(FColor(255, 120, 30));
	DamageFire->SetAttenuationRadius(900.f);
	DamageFire->SetCastShadows(false);
	DamageFire->SetIntensity(0.f);

	// any vehicle placed in a level (or spawned unpossessed) fights as AI
	AIControllerClass = ABLAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ABLCombatVehicle::InputFirePressed()
{
	if (Weapon)
	{
		Weapon->StartFirePrimary();
	}
}

void ABLCombatVehicle::InputFireReleased()
{
	if (Weapon)
	{
		Weapon->StopFirePrimary();
	}
}

void ABLCombatVehicle::InputFirePickup()
{
	if (Weapon)
	{
		Weapon->FirePickup();
	}
}

void ABLCombatVehicle::InputCycleNext()
{
	if (Weapon)
	{
		Weapon->CycleWeapon(1);
	}
}

void ABLCombatVehicle::InputCyclePrev()
{
	if (Weapon)
	{
		Weapon->CycleWeapon(-1);
	}
}

void ABLCombatVehicle::InputSpecial()
{
	if (Special && Health && !Health->IsDead())
	{
		Special->TryActivate();
	}
}

void ABLCombatVehicle::OnChassisHit(UPrimitiveComponent* /*HitComp*/, AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/, FVector /*NormalImpulse*/, const FHitResult& Hit)
{
	// each vehicle applies its OWN ram damage in its own callback: heavier opponent
	// (mass ratio) and faster closing speed hurt more; the other side runs the same
	// math from its perspective, so a heavy truck shrugs off what flattens a coupe
	ABLCombatVehicle* Other = Cast<ABLCombatVehicle>(OtherActor);
	if (!Other || Health->IsDead())
	{
		return;
	}
	const double Now = GetWorld()->GetTimeSeconds();
	if (Now - LastRamDamageTime < RamCooldownSeconds)
	{
		return;
	}

	const FVector RelVel = Other->GetVelocity() - GetVelocity();
	const float ClosingKph = FMath::Abs(FVector::DotProduct(RelVel, Hit.ImpactNormal)) * 0.036f;
	if (ClosingKph < RamMinKph)
	{
		return; // rubbing / parking contact
	}
	LastRamDamageTime = Now;

	const float MassRatio = FMath::Clamp(Other->MassKg / FMath::Max(MassKg, 1.f), 0.5f, 2.f);
	const float Dmg = FMath::Min((ClosingKph - RamMinKph) * RamDamagePerKph * MassRatio, RamMaxDamage);
	if (Dmg < 1.f)
	{
		return;
	}
	Health->ApplyDamage(Dmg);

	if (UBLImpactFXSubsystem* FX = GetWorld()->GetSubsystem<UBLImpactFXSubsystem>())
	{
		FX->PlayImpact(FVector(Hit.ImpactPoint),
			Dmg < 20.f ? EBLImpactWeight::Light : EBLImpactWeight::Medium);
	}
}

void ABLCombatVehicle::OnVehicleDeath()
{
	// with a 14-car field, AI-vs-AI kills happen constantly - only sell the death
	// moment when it involves the player or dies on their doorstep
	bool bPlayerRelevant = IsPlayerControlled();
	if (!bPlayerRelevant)
	{
		if (const APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (const APawn* PlayerPawn = PC->GetPawn())
			{
				bPlayerRelevant =
					FVector::DistSquared(PlayerPawn->GetActorLocation(), GetActorLocation())
					< FMath::Square(4000.f);
			}
		}
	}
	if (bPlayerRelevant)
	{
		if (UBLImpactFXSubsystem* FX = GetWorld()->GetSubsystem<UBLImpactFXSubsystem>())
		{
			FX->PlayDeathMoment();
		}
		if (UBLAudioSubsystem* Audio = GetWorld()->GetSubsystem<UBLAudioSubsystem>())
		{
			Audio->PostVehicleDeath(GetActorLocation());
			if (IsPlayerControlled())
			{
				Audio->PostBark(CharacterKey, EBLBark::Death, GetActorLocation());
			}
		}
	}

	if (ABLGameMode* GM = GetWorld()->GetAuthGameMode<ABLGameMode>())
	{
		GM->NotifyVehicleDeath(this);
	}
}

void ABLCombatVehicle::InputRestart()
{
	if (ABLGameMode* GM = GetWorld()->GetAuthGameMode<ABLGameMode>())
	{
		GM->RequestRestart();
	}
}

void ABLCombatVehicle::UpdateDamageFX()
{
	if (!Health || !DamageSmoke || !DamageFire)
	{
		return;
	}
	const float Frac = Health->GetHealth() / FMath::Max(Health->GetMaxHealth(), 1.f);
	const bool bDead = Health->IsDead();
	const float T = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	const float Offset = static_cast<float>(GetUniqueID() % 997); // desync flicker per car

	// smoke: from 55% HP down (or a burning wreck), growing + darkening with severity
	const bool bSmoke = bDead || Frac < 0.55f;
	DamageSmoke->SetVisibility(bSmoke);
	if (bSmoke)
	{
		const float Sev = bDead ? 1.f : FMath::Clamp((0.55f - Frac) / 0.55f, 0.f, 1.f);
		const float Pulse = 1.f + 0.12f * FMath::Sin(T * 3.5f + Offset);
		DamageSmoke->SetRelativeScale3D(FVector((0.7f + 1.6f * Sev) * Pulse));
		if (DamageSmokeMID)
		{
			DamageSmokeMID->SetScalarParameterValue(TEXT("Opacity"), 0.12f + 0.40f * Sev);
		}
	}

	// fire: from 28% HP down (or wrecked) - a flickering ember glow at the engine
	const bool bFire = bDead || (Frac > 0.f && Frac < 0.28f);
	if (bFire)
	{
		const float FireSev = bDead ? 1.f : FMath::Clamp((0.28f - Frac) / 0.28f, 0.f, 1.f);
		const float Flick = 0.55f + 0.45f * FMath::PerlinNoise1D(T * 20.f + Offset);
		DamageFire->SetIntensity(FMath::Lerp(2500.f, 16000.f, FireSev) * Flick);
	}
	else
	{
		DamageFire->SetIntensity(0.f);
	}
}

void ABLCombatVehicle::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->SetMassOverrideInKg(NAME_None, MassKg);
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, BodyMeshZOffset));

	Health->OnDeath.AddDynamic(this, &ABLCombatVehicle::OnVehicleDeath);
	CollisionBox->OnComponentHit.AddDynamic(this, &ABLCombatVehicle::OnChassisHit);

	Wheels.Reset();
	Wheels.Add({ WheelFL, FVector(AxleFrontX,  TrackHalfY, AnchorZ), FrontWheelRadius, true });
	Wheels.Add({ WheelFR, FVector(AxleFrontX, -TrackHalfY, AnchorZ), FrontWheelRadius, true });
	Wheels.Add({ WheelRL, FVector(AxleRearX,   TrackHalfY, AnchorZ), RearWheelRadius, false });
	Wheels.Add({ WheelRR, FVector(AxleRearX,  -TrackHalfY, AnchorZ), RearWheelRadius, false });

	// damage smoke uses the scripted translucent material when present
	if (DamageSmoke)
	{
		UMaterialInterface* SmokeMat = LoadObject<UMaterialInterface>(
			nullptr, TEXT("/Game/BlackLedger/FX/M_BL_Smoke.M_BL_Smoke"));
		DamageSmokeMID = DamageSmoke->CreateDynamicMaterialInstance(0, SmokeMat);
		if (DamageSmokeMID)
		{
			DamageSmokeMID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.04f, 0.04f, 0.045f));
		}
	}
}

void ABLCombatVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// player-only adaptive audio: engine pitch from speed, heartbeat from low HP
	if (IsPlayerControlled())
	{
		if (UBLAudioSubsystem* Audio = GetWorld()->GetSubsystem<UBLAudioSubsystem>())
		{
			const float SpeedKph = CollisionBox->GetPhysicsLinearVelocity().Size() * 0.036f;
			Audio->NotifyPlayerSpeed(this, SpeedKph);
			if (Health)
			{
				const float Frac = Health->GetHealth() / FMath::Max(Health->GetMaxHealth(), 1.f);
				Audio->NotifyLowHealth(Frac);
				// one "I'm in trouble" bark per low spell
				if (Frac > 0.f && Frac < 0.25f && !bSaidLowHealth)
				{
					Audio->PostBark(CharacterKey, EBLBark::LowHealth, GetActorLocation());
					bSaidLowHealth = true;
				}
				else if (Frac > 0.35f)
				{
					bSaidLowHealth = false;
				}
			}
		}
	}

	// damage smoke/fire for every vehicle (player + AI + wrecks)
	UpdateDamageFX();

	// the player vehicle must never sleep: a sleeping body ignores suspension forces,
	// settles onto its collision box, and "explodes" off the over-compressed springs on wake
	if (!CollisionBox->RigidBodyIsAwake())
	{
		CollisionBox->WakeAllRigidBodies();
	}

	// a dead vehicle is a wreck: no drive, no fire (proper death sequence later)
	if (Health->IsDead())
	{
		ThrottleInput = 0.f;
		SteerInput = 0.f;
		bHandbrake = false;
		if (Weapon)
		{
			Weapon->StopFirePrimary();
		}
	}

	StepSuspensionAndDrive(DeltaTime);
	StepWheelVisuals(DeltaTime);
	StepSelfRight(DeltaTime);

#if !(UE_BUILD_SHIPPING)
	if (GEngine && bDrawSuspensionDebug)
	{
		const float Speed = CollisionBox->GetPhysicsLinearVelocity().Size() * 0.036f; // kph
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Yellow,
			FString::Printf(TEXT("Throttle %.2f  Steer %.2f  Handbrake %d"),
				ThrottleInput, SteerInput, bHandbrake ? 1 : 0));
		GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::Cyan,
			FString::Printf(TEXT("Grounded %d/4  Speed %.1f kph  Awake %d  SimPhys %d"),
				GroundedCount, Speed, CollisionBox->RigidBodyIsAwake() ? 1 : 0,
				CollisionBox->IsSimulatingPhysics() ? 1 : 0));
	}
#endif
}

void ABLCombatVehicle::StepSuspensionAndDrive(float Dt)
{
	const FTransform BoxTM = CollisionBox->GetComponentTransform();
	const FVector Up = BoxTM.GetUnitAxis(EAxis::Z);
	const FVector Fwd = BoxTM.GetUnitAxis(EAxis::X);
	const FVector Right = BoxTM.GetUnitAxis(EAxis::Y);

	FCollisionQueryParams Query(SCENE_QUERY_STAT(BLSuspension), false, this);
	GroundedCount = 0;

	for (FBLWheel& W : Wheels)
	{
		const FVector Anchor = BoxTM.TransformPosition(W.AnchorLocal);
		const float TraceLen = W.Radius + SuspensionTravel;
		const FVector End = Anchor - Up * TraceLen;

		FHitResult Hit;
		W.bGrounded = GetWorld()->LineTraceSingleByChannel(Hit, Anchor, End, ECC_Visibility, Query);
		if (W.bGrounded)
		{
			GroundedCount++;
			// clamp to travel so a deep penetration can never store explosive force
			const float Compression = FMath::Min(TraceLen - Hit.Distance, SuspensionTravel);
			const FVector PointVel = CollisionBox->GetPhysicsLinearVelocityAtPoint(Anchor);
			const float UpVel = FVector::DotProduct(PointVel, Up);
			// cap total corner force so a hard landing's compression+damping spike
			// can't fire the chassis back into the air (the classic raycast pogo)
			const float Force = FMath::Clamp(
				SpringStrength * Compression - SpringDamping * UpVel, 0.f, MaxSpringForce);
			if (Force > 0.f)
			{
				CollisionBox->AddForceAtLocation(Up * Force, Anchor);
			}
		}

		if (bDrawSuspensionDebug)
		{
			DrawDebugLine(GetWorld(), Anchor, End, W.bGrounded ? FColor::Green : FColor::Red,
				false, -1.f, 0, 2.f);
		}
	}

	const FVector Vel = CollisionBox->GetPhysicsLinearVelocity();
	const float FwdSpeed = FVector::DotProduct(Vel, Fwd);

	// airborne: weighty fall + tumble damping + auto-level so it lands on its wheels
	if (GroundedCount == 0)
	{
		CollisionBox->AddForce(FVector(0.f, 0.f, -ExtraFallGravity) * MassKg);

		FVector AngVel = CollisionBox->GetPhysicsAngularVelocityInDegrees();
		AngVel *= FMath::Clamp(1.f - AirAngularDamping * Dt, 0.f, 1.f);
		const FVector LevelAxis = FVector::CrossProduct(Up, FVector::UpVector); // toward upright
		AngVel += LevelAxis * AirLevelStrength * Dt;
		CollisionBox->SetPhysicsAngularVelocityInDegrees(AngVel);
	}

	if (GroundedCount >= 2)
	{
		// drive force with speed clamps
		const float MaxFwd = MaxSpeedKph * KphToCms;
		const float MaxRev = MaxReverseKph * KphToCms;
		const bool bBlockedFwd = (ThrottleInput > 0.f && FwdSpeed > MaxFwd);
		const bool bBlockedRev = (ThrottleInput < 0.f && FwdSpeed < -MaxRev);
		if (!bBlockedFwd && !bBlockedRev && FMath::Abs(ThrottleInput) > 0.02f)
		{
			const FVector FwdOnGround = FVector::VectorPlaneProject(Fwd, FVector::UpVector).GetSafeNormal();
			CollisionBox->AddForce(FwdOnGround * ThrottleInput * EngineForce);
		}

		// steering: command yaw RATE directly (arcade). Torque gets eaten by the
		// chassis inertia; setting the Z angular velocity is crisp and reliable.
		// Preserve roll/pitch (X/Y) from the suspension; only drive yaw (Z).
		const float SpeedFactor = FMath::Clamp(FMath::Abs(FwdSpeed) / 600.f, 0.f, 1.f);
		const float DirSign = (FwdSpeed >= -30.f) ? 1.f : -1.f;
		const float YawBoost = bHandbrake ? HandbrakeYawBoost : 1.f;
		const float DesiredYaw = SteerInput * DirSign * MaxYawRateDeg * SpeedFactor * YawBoost;
		FVector AngVel = CollisionBox->GetPhysicsAngularVelocityInDegrees();
		AngVel.Z = FMath::FInterpTo(AngVel.Z, DesiredYaw, Dt, SteerResponse);
		CollisionBox->SetPhysicsAngularVelocityInDegrees(AngVel);

		// lateral grip (cut hard on handbrake so the rear breaks loose into a drift)
		const float LatSpeed = FVector::DotProduct(Vel, Right);
		const float Grip = GripCoefficient * (bHandbrake ? HandbrakeGripScale : 1.f);
		CollisionBox->AddForce(-Right * LatSpeed * Grip * MassKg);

		// rolling + coast drag
		const float Drag = RollingDrag + ((FMath::Abs(ThrottleInput) < 0.05f) ? CoastDrag : 0.f);
		const FVector PlanarVel = FVector::VectorPlaneProject(Vel, Up);
		CollisionBox->AddForce(-PlanarVel * Drag * MassKg);

		// handbrake: strong longitudinal braking (scrubs speed fast for a drift entry)
		if (bHandbrake && FMath::Abs(FwdSpeed) > 1.f)
		{
			const FVector FwdOnGround = FVector::VectorPlaneProject(Fwd, FVector::UpVector).GetSafeNormal();
			CollisionBox->AddForce(-FwdOnGround * FMath::Sign(FwdSpeed) * HandbrakeBrakeForce);
		}
	}
}

void ABLCombatVehicle::StepWheelVisuals(float Dt)
{
	const FTransform BoxTM = CollisionBox->GetComponentTransform();
	const FVector Up = BoxTM.GetUnitAxis(EAxis::Z);
	const FVector Fwd = BoxTM.GetUnitAxis(EAxis::X);
	const float FwdSpeed = FVector::DotProduct(CollisionBox->GetPhysicsLinearVelocity(), Fwd);

	FCollisionQueryParams Query(SCENE_QUERY_STAT(BLWheelVis), false, this);

	for (FBLWheel& W : Wheels)
	{
		if (!W.Mesh)
		{
			continue;
		}
		// vertical placement from a fresh short trace (cheap, and decoupled from physics step)
		const FVector Anchor = BoxTM.TransformPosition(W.AnchorLocal);
		const float TraceLen = W.Radius + SuspensionTravel;
		FHitResult Hit;
		float Drop = SuspensionTravel; // full droop in air
		if (GetWorld()->LineTraceSingleByChannel(Hit, Anchor, Anchor - Up * TraceLen, ECC_Visibility, Query))
		{
			Drop = FMath::Clamp(Hit.Distance - W.Radius, -SuspensionTravel * 0.4f, SuspensionTravel);
		}
		FVector Local = W.AnchorLocal;
		Local.Z -= Drop;
		W.Mesh->SetRelativeLocation(Local);

		// spin + steer
		W.SpinDeg = FMath::Fmod(W.SpinDeg + FMath::RadiansToDegrees((FwdSpeed / W.Radius)) * Dt, 360.f);
		const float SteerYaw = W.bFront ? SteerInput * 28.f : 0.f;
		W.Mesh->SetRelativeRotation(FRotator(-W.SpinDeg, SteerYaw, 0.f));
	}
}

void ABLCombatVehicle::StepSelfRight(float Dt)
{
	const float UpZ = GetActorUpVector().Z;
	const float Speed = CollisionBox->GetPhysicsLinearVelocity().Size();
	if (UpZ < 0.15f && Speed < 150.f)
	{
		UpsideDownTime += Dt;
		if (UpsideDownTime >= SelfRightDelay)
		{
			UpsideDownTime = 0.f;
			const FVector Loc = GetActorLocation() + FVector(0.f, 0.f, 120.f);
			const FRotator YawOnly(0.f, GetActorRotation().Yaw, 0.f);
			CollisionBox->SetPhysicsLinearVelocity(FVector::ZeroVector);
			CollisionBox->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
			SetActorLocationAndRotation(Loc, YawOnly, false, nullptr, ETeleportType::TeleportPhysics);
		}
	}
	else
	{
		UpsideDownTime = 0.f;
	}
}

void ABLCombatVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("BL_Throttle"), this, &ABLCombatVehicle::InputThrottle);
	PlayerInputComponent->BindAxis(TEXT("BL_Steer"), this, &ABLCombatVehicle::InputSteer);
	PlayerInputComponent->BindAction(TEXT("BL_Handbrake"), IE_Pressed, this, &ABLCombatVehicle::InputHandbrakePressed);
	PlayerInputComponent->BindAction(TEXT("BL_Handbrake"), IE_Released, this, &ABLCombatVehicle::InputHandbrakeReleased);
	PlayerInputComponent->BindAction(TEXT("BL_Fire"), IE_Pressed, this, &ABLCombatVehicle::InputFirePressed);
	PlayerInputComponent->BindAction(TEXT("BL_Fire"), IE_Released, this, &ABLCombatVehicle::InputFireReleased);
	PlayerInputComponent->BindAction(TEXT("BL_FirePickup"), IE_Pressed, this, &ABLCombatVehicle::InputFirePickup);
	PlayerInputComponent->BindAction(TEXT("BL_Special"), IE_Pressed, this, &ABLCombatVehicle::InputSpecial);
	PlayerInputComponent->BindAction(TEXT("BL_Restart"), IE_Pressed, this, &ABLCombatVehicle::InputRestart);
	PlayerInputComponent->BindAction(TEXT("BL_CycleNext"), IE_Pressed, this, &ABLCombatVehicle::InputCycleNext);
	PlayerInputComponent->BindAction(TEXT("BL_CyclePrev"), IE_Pressed, this, &ABLCombatVehicle::InputCyclePrev);

	// The menu + intro cinematic run in UI-only input, which puts the *game
	// viewport* (which survives OpenLevel into the arena) into ignore-input /
	// no-mouse-capture. Restore game input now that we're driving, or W/fire are
	// dead until something else flips it back.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}

	// the contestant's entry line as the player takes the wheel
	if (UBLAudioSubsystem* Audio = GetWorld()->GetSubsystem<UBLAudioSubsystem>())
	{
		Audio->PostBark(CharacterKey, EBLBark::Taunt, GetActorLocation());
	}

	// Pause must still fire while the game is paused so the same key resumes it.
	FInputActionBinding& PauseBind = PlayerInputComponent->BindAction(
		TEXT("BL_Pause"), IE_Pressed, this, &ABLCombatVehicle::InputPause);
	PauseBind.bExecuteWhenPaused = true;
}

void ABLCombatVehicle::InputPause()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UBLUISubsystem* UI = GI->GetSubsystem<UBLUISubsystem>())
		{
			UI->TogglePause();
		}
	}
}
