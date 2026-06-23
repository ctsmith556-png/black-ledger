// Black Ledger - Collector boss pawn base (Foundryman defaults)

#include "BLBossPawn.h"
#include "Audio/BLAudioSubsystem.h"
#include "Bosses/BLBossAIController.h"
#include "Bosses/BLMoltenWall.h"
#include "Bosses/BLTelegraphRing.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/BLGameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FX/BLImpactBurst.h"
#include "FX/BLImpactFXSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Hazards/BLHazard_FurnacePour.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Vehicles/BLHealthComponent.h"
#include "Weapons/BLProjectile_Slag.h"

ABLBossPawn::ABLBossPawn()
{
	// ---- The Foundryman "Tap" (tuning sheet Bosses tab) ----
	// Speed 2 / Armor 14 / HP x6 -> 11,133 effective. Immovable, furnace-immune.
	Health->BaseHP = 6000.f;        // 6000 * (0.7 + 13 * 0.0889) = 11,133
	Health->ArmorStat = 14.f;       // over-budget by design
	bHazardImmune = true;           // he is the furnace
	AIControllerClass = ABLBossAIController::StaticClass(); // hunts, never wanders

	MassKg = 12000.f;               // "immovable": rams shrug, knockback barely moves him
	MaxSpeedKph = 55.f;             // Speed 2 - a glacier that will not stop
	EngineForce = 7000000.f;
	HandbrakeGripScale = 0.6f;      // no drifting for the cupola car
	bDrawSuspensionDebug = false;

	// ---- treads, not wheels: hide the inherited Surgeon wheel meshes; the
	// suspension raycasts don't care. Tread-scroll material sells motion later. ----
	for (UStaticMeshComponent* W : { WheelFL.Get(), WheelFR.Get(), WheelRL.Get(), WheelRR.Get() })
	{
		if (W)
		{
			W->SetStaticMesh(nullptr);
			W->SetVisibility(false);
		}
	}

	// Foundryman body once imported (falls back to the Surgeon stand-in until then)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BossBodyFinder(
		TEXT("/Game/BlackLedger/Characters/Foundryman/SM_Foundryman_Body"));
	if (BossBodyFinder.Succeeded())
	{
		BodyMesh->SetStaticMesh(BossBodyFinder.Object);
	}

	// cupola-car chassis at kaiju scale (18 m prep -> ~18.0 x 6.6 x 6.1 m).
	// Anchors sit at the tread corners; suspension stiffened for the tonnage.
	CollisionBox->SetBoxExtent(FVector(895.f, 325.f, 220.f));
	AxleFrontX = 685.f;
	AxleRearX = -685.f;
	TrackHalfY = 260.f;
	FrontWheelRadius = 94.f;
	RearWheelRadius = 94.f;
	AnchorZ = -150.f;
	BodyMeshZOffset = -290.f;
	SpringStrength = 215000.f;      // 12 t on four corners
	SpringDamping = 30000.f;
	MaxSpringForce = 12000000.f;

	// the incandescent core in the cage: the weak point AND the white-hot tell.
	// Dim ember normally; ramps white before a slam; blazes when exposed.
	Core = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Core"));
	Core->SetupAttachment(BodyMesh);
	Core->SetRelativeLocation(FVector(0.f, 0.f, 360.f));   // cage height on the cupola car
	Core->SetRelativeScale3D(FVector(1.4f));
	Core->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Core->SetCastShadow(false);
	Core->SetCanEverAffectNavigation(false);
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFinder(
			TEXT("/Engine/BasicShapes/Sphere.Sphere"));
		if (SphereFinder.Succeeded()) { Core->SetStaticMesh(SphereFinder.Object); }
		static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
		if (MatFinder.Succeeded()) { Core->SetMaterial(0, MatFinder.Object); }
	}

	// ---- tread belts down each flank (the inherited wheels are hidden). Cleated
	// strips whose scrolling material (M_BL_Tread) sells the crawler's motion. ----
	UStaticMesh* CubeMesh = nullptr;
	UMaterialInterface* CubeMat = nullptr;
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(
			TEXT("/Engine/BasicShapes/Cube.Cube"));
		CubeMesh = CubeFinder.Succeeded() ? CubeFinder.Object : nullptr;
		static ConstructorHelpers::FObjectFinder<UMaterialInterface> CubeMatFinder(
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
		CubeMat = CubeMatFinder.Succeeded() ? CubeMatFinder.Object : nullptr;
	}
	TreadL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TreadL"));
	TreadR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TreadR"));
	for (UStaticMeshComponent* Tread : { TreadL.Get(), TreadR.Get() })
	{
		Tread->SetupAttachment(CollisionBox);
		Tread->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Tread->SetCastShadow(false);
		Tread->SetCanEverAffectNavigation(false);
		Tread->SetRelativeScale3D(FVector(17.9f, 0.85f, 1.7f));   // ~1790 x 85 x 170 cm
		if (CubeMesh) { Tread->SetStaticMesh(CubeMesh); }
		if (CubeMat) { Tread->SetMaterial(0, CubeMat); }
	}
	TreadL->SetRelativeLocation(FVector(0.f, 270.f, -120.f));
	TreadR->SetRelativeLocation(FVector(0.f, -270.f, -120.f));
}

void ABLBossPawn::BeginPlay()
{
	Super::BeginPlay();
	Health->OnDamaged.AddDynamic(this, &ABLBossPawn::OnBossDamaged);
	Health->OnDeath.AddDynamic(this, &ABLBossPawn::OnBossDeath);

	if (Core)
	{
		UMaterialInterface* Emissive = LoadObject<UMaterialInterface>(
			nullptr, TEXT("/Game/BlackLedger/FX/M_BL_Emissive.M_BL_Emissive"));
		CoreMID = Core->CreateDynamicMaterialInstance(0, Emissive);
		if (CoreMID)
		{
			CoreMID->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.f, 0.30f, 0.06f));
			CoreMID->SetScalarParameterValue(TEXT("Strength"), 1.5f);  // dim ember at rest
		}
	}

	// Procedural cube treads hidden by default (they read as glowing diagonal stripes on
	// the detailed hull). Toggle bShowTreadOverlays to bring them back with a real belt.
	if (TreadL) { TreadL->SetVisibility(bShowTreadOverlays); }
	if (TreadR) { TreadR->SetVisibility(bShowTreadOverlays); }

	// Seat the tread overlays on the body's REAL flanks (the ctor's ±270 guess floats
	// beside this mesh). Compute the body's local bounds and snap the strips just inside
	// the widest point, along the lower body, sized to the body length. Mesh-independent.
	if (bShowTreadOverlays && BodyMesh && BodyMesh->GetStaticMesh() && TreadL && TreadR)
	{
		const FBox LocalBox = BodyMesh->GetStaticMesh()->GetBoundingBox();
		const FVector BScale = BodyMesh->GetRelativeScale3D();
		const FVector Ext = LocalBox.GetExtent() * BScale;                       // half-extents
		const FVector Ctr = LocalBox.GetCenter() * BScale + BodyMesh->GetRelativeLocation();
		const float StripLen = Ext.X * 2.f * 0.92f;     // ~body length
		const float StripH = Ext.Z * 0.5f;              // ~half body height
		const float CubeUU = 100.f;                     // engine cube = 100 cm
		const float YPos = FMath::Max(Ext.Y - TreadInsetY, 0.f);
		const float ZPos = (Ctr.Z - Ext.Z) + StripH * 0.5f + TreadDropZ;  // seated low
		for (UStaticMeshComponent* T : { TreadL.Get(), TreadR.Get() })
		{
			T->SetRelativeScale3D(FVector(StripLen / CubeUU, 0.85f, StripH / CubeUU));
		}
		TreadL->SetRelativeLocation(FVector(Ctr.X, YPos, ZPos));
		TreadR->SetRelativeLocation(FVector(Ctr.X, -YPos, ZPos));
	}

	// Treads are the body mesh's own modeled tracks (static for the slice; animated treads
	// are deferred to the boss art pass). No procedural overlay/scroll on the body.

	// tread belts use the scrolling cleat material when present (lit cube fallback)
	if (bShowTreadOverlays)
	{
		if (UMaterialInterface* TreadMat = LoadObject<UMaterialInterface>(
			nullptr, TEXT("/Game/BlackLedger/FX/M_BL_Tread.M_BL_Tread")))
		{
			if (TreadL) { TreadLMID = TreadL->CreateDynamicMaterialInstance(0, TreadMat); }
			if (TreadR) { TreadRMID = TreadR->CreateDynamicMaterialInstance(0, TreadMat); }
		}
	}
}

void ABLBossPawn::RiseAt(const FVector& Location)
{
	SetActorLocation(Location + FVector(0.f, 0.f, 200.f), false, nullptr, ETeleportType::TeleportPhysics);
	if (UBLImpactFXSubsystem* FX = GetWorld()->GetSubsystem<UBLImpactFXSubsystem>())
	{
		FX->PlayImpact(Location, EBLImpactWeight::Massive); // placeholder arrival quake
	}
	if (UBLAudioSubsystem* A = GetWorld()->GetSubsystem<UBLAudioSubsystem>())
	{
		A->PostBossVO(TEXT("Foundryman"), EBLBossVO::Rise, GetActorLocation());
	}
	// TODO(slice flow): cinematic rise from inside Furnace 3, commandeer its pour
	// hazard (faster cycle, boss-controlled), boss HP bar HUD hookup
#if !(UE_BUILD_SHIPPING)
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red,
			TEXT("THE FOUNDRYMAN RISES"));
	}
#endif
}

void ABLBossPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Health->IsDead())
	{
		UpdateAttacks(DeltaTime);
	}
	UpdateCoreGlow(DeltaTime);
	UpdateTreads(DeltaTime);
}

float ABLBossPawn::DifficultyIntervalScale() const
{
	// Bible 4.6: difficulty scales attack FREQUENCY, never the tells
	const UBLGameInstance* GI = Cast<UBLGameInstance>(GetGameInstance());
	switch (GI ? GI->GetDifficulty() : EBLDifficulty::Medium)
	{
	case EBLDifficulty::Easy: return 1.35f;
	case EBLDifficulty::Hard: return 0.75f;
	default:                  return 1.f;
	}
}

APawn* ABLBossPawn::FindPlayerTarget() const
{
	const APlayerController* PC = GetWorld()->GetFirstPlayerController();
	APawn* Player = PC ? PC->GetPawn() : nullptr;
	if (!Player)
	{
		return nullptr;
	}
	const UBLHealthComponent* PlayerHealth = Player->FindComponentByClass<UBLHealthComponent>();
	return (PlayerHealth && PlayerHealth->IsDead()) ? nullptr : Player;
}

void ABLBossPawn::UpdateAttacks(float DeltaTime)
{
	APawn* Target = FindPlayerTarget();
	if (!Target)
	{
		return;
	}
	const float Scale = DifficultyIntervalScale();
	const float Dist = FVector::Dist2D(Target->GetActorLocation(), GetActorLocation());
	const float PhaseTempo = (Phase >= 3) ? 0.7f : 1.f;

	MortarClock += DeltaTime;
	if (MortarClock >= MortarInterval * Scale * PhaseTempo && Dist < 14000.f)
	{
		MortarClock = 0.f;
		BeginMortar(Target);
	}

	if (Phase >= 2)
	{
		WallClock += DeltaTime;
		if (WallClock >= WallInterval * Scale * PhaseTempo && Dist < 10000.f)
		{
			WallClock = 0.f;
			SpawnMoltenWall(Target);
		}
	}

	// Phase 3: the slam + weak-point window. Only slams when the player is close
	// enough to bait it - the whole loop is "get close, dodge the slam, punish the core".
	if (Phase >= 3 && !bSlamTelegraph && !bCoreExposed)
	{
		SlamClock += DeltaTime;
		if (SlamClock >= SlamInterval * Scale && Dist < SlamRange)
		{
			SlamClock = 0.f;
			BeginSlam();
		}
	}

	// (boss bar now drawn by ABLHUD)
}

void ABLBossPawn::BeginMortar(APawn* Target)
{
	// under-lead the target + scatter, ring the landing zone, shell arrives as
	// the ring expires: keeping your speed up OR breaking your line both escape
	const FVector2D Scatter = FMath::RandPointInCircle(MortarScatter);
	const FVector Predicted = Target->GetActorLocation()
		+ Target->GetVelocity() * (MortarTelegraph * MortarLeadFactor)
		+ FVector(Scatter.X, Scatter.Y, 0.f);
	PendingMortarTarget = FVector(Predicted.X, Predicted.Y, 30.f);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ABLTelegraphRing* Ring = GetWorld()->SpawnActor<ABLTelegraphRing>(
		PendingMortarTarget, FRotator::ZeroRotator, Params))
	{
		Ring->Configure(800.f, MortarTelegraph, FLinearColor(1.f, 0.45f, 0.06f)); // = slag splash radius
	}
	GetWorld()->GetTimerManager().SetTimer(
		MortarLaunchTimer, this, &ABLBossPawn::LaunchMortar, MortarTelegraph * 0.35f, false);
}

void ABLBossPawn::LaunchMortar()
{
	if (Health->IsDead())
	{
		return;
	}
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FVector Muzzle = GetActorLocation() + FVector(0.f, 0.f, 500.f); // vessel top
	if (ABLProjectile_Slag* Shell = GetWorld()->SpawnActor<ABLProjectile_Slag>(
		Muzzle, FRotator::ZeroRotator, Params))
	{
		Shell->LaunchBallistic(PendingMortarTarget, MortarTelegraph * 0.65f);
	}
}

void ABLBossPawn::SpawnMoltenWall(APawn* Target)
{
	// the wall cuts ACROSS the player's lane: perpendicular to boss->target
	const FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
	const FRotator WallRot(0.f, ToTarget.Rotation().Yaw + 90.f, 0.f);
	const FVector Center = Target->GetActorLocation()
		+ Target->GetVelocity() * 0.8f; // lead so it lands in their path

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ABLMoltenWall* Wall = GetWorld()->SpawnActor<ABLMoltenWall>(
		FVector(Center.X, Center.Y, 25.f), WallRot, Params))
	{
		Wall->Configure(WallLength, 350.f, 1.2f, 4.f, 70.f, this);
	}
}

void ABLBossPawn::BeginSlam()
{
	bSlamTelegraph = true;
	SlamTelegraphT = 0.f;

	// danger ring on the ground under him; the cage glows white (UpdateCoreGlow)
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FVector Ground(GetActorLocation().X, GetActorLocation().Y, 30.f);
	if (ABLTelegraphRing* Ring = GetWorld()->SpawnActor<ABLTelegraphRing>(
		Ground, FRotator::ZeroRotator, Params))
	{
		Ring->Configure(SlamRadius, SlamTelegraph, FLinearColor(1.f, 0.85f, 0.4f));
	}
	GetWorldTimerManager().SetTimer(SlamHitTimer, this, &ABLBossPawn::DoSlam, SlamTelegraph, false);
	if (UBLAudioSubsystem* A = GetWorld()->GetSubsystem<UBLAudioSubsystem>())
	{
		A->PostBossVO(TEXT("Foundryman"), EBLBossVO::Attack, GetActorLocation());
	}
}

void ABLBossPawn::DoSlam()
{
	bSlamTelegraph = false;
	if (Health->IsDead())
	{
		return;
	}
	const FVector Center = GetActorLocation();
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* Pawn = *It;
		if (Pawn == this)
		{
			continue;
		}
		const float Dist = FVector::Dist2D(Pawn->GetActorLocation(), Center);
		if (Dist > SlamRadius)
		{
			continue;
		}
		const float Falloff = 1.f - 0.7f * (Dist / SlamRadius);
		if (UBLHealthComponent* V = Pawn->FindComponentByClass<UBLHealthComponent>())
		{
			V->ApplyDamage(SlamDamage * Falloff);
		}
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Pawn->GetRootComponent()))
		{
			if (Prim->IsSimulatingPhysics())
			{
				FVector Dir = (Pawn->GetActorLocation() - Center).GetSafeNormal();
				Dir.Z = FMath::Max(Dir.Z, 0.35f);   // launch them off their wheels
				Prim->AddImpulse(Dir * 950.f * Falloff, NAME_None, true);
			}
		}
	}
	if (UBLImpactFXSubsystem* FX = GetWorld()->GetSubsystem<UBLImpactFXSubsystem>())
	{
		FX->PlayImpact(Center, EBLImpactWeight::Massive);
	}
	// the punish window opens
	ExposeCore();
}

void ABLBossPawn::ExposeCore()
{
	bCoreExposed = true;
	Health->IncomingDamageMultiplier = ExposedDamageMultiplier;   // hit the core NOW
	GetWorldTimerManager().SetTimer(ExposeTimer, this, &ABLBossPawn::HideCore, ExposeSeconds, false);
}

void ABLBossPawn::HideCore()
{
	bCoreExposed = false;
	if (Health)
	{
		Health->IncomingDamageMultiplier = 1.f;
	}
}

void ABLBossPawn::DropFireTrail()
{
	if (Health->IsDead())
	{
		return;
	}
	// pure-visual lingering fire behind the treads (no shake/audio - that would spam)
	const FVector Behind = GetActorLocation() - GetActorForwardVector() * 600.f + FVector(0.f, 0.f, 40.f);
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ABLImpactBurst* Burst = GetWorld()->SpawnActor<ABLImpactBurst>(
		Behind, FRotator::ZeroRotator, Params))
	{
		Burst->Configure(EBLImpactWeight::Light);
	}
}

void ABLBossPawn::UpdateCoreGlow(float DeltaTime)
{
	if (!CoreMID)
	{
		return;
	}
	if (Health && Health->IsDead())
	{
		CoreMID->SetScalarParameterValue(TEXT("Strength"), 0.f);  // stays dark once cooled
		return;
	}
	const float T = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	float Strength = 1.5f;
	FLinearColor Color(1.f, 0.30f, 0.06f);

	if (bSlamTelegraph)
	{
		// ramp to white-hot as the slam lands - the readable "tell"
		SlamTelegraphT = FMath::Min(1.f, SlamTelegraphT + DeltaTime / FMath::Max(SlamTelegraph, 0.1f));
		Strength = FMath::Lerp(1.5f, 15.f, SlamTelegraphT);
		Color = FLinearColor::LerpUsingHSV(
			FLinearColor(1.f, 0.30f, 0.06f), FLinearColor(1.f, 0.95f, 0.85f), SlamTelegraphT);
	}
	else if (bCoreExposed)
	{
		// blazing + pulsing: STRIKE HERE
		Strength = 9.f + 4.f * FMath::Sin(T * 12.f);
		Color = FLinearColor(1.f, 0.7f, 0.3f);
	}
	else
	{
		Strength = 1.5f + 0.4f * FMath::Sin(T * 2.f);   // idle ember breathing
	}
	CoreMID->SetScalarParameterValue(TEXT("Strength"), Strength);
	CoreMID->SetVectorParameterValue(TEXT("Color"), Color);
}

void ABLBossPawn::UpdateTreads(float DeltaTime)
{
	if (!TreadLMID && !TreadRMID)
	{
		return;
	}
	// scroll the cleats by distance travelled along forward (signed), so they
	// march when he advances and reverse when he backs up
	const float Fwd = FVector::DotProduct(
		CollisionBox->GetPhysicsLinearVelocity(), GetActorForwardVector());
	TreadScroll += Fwd * DeltaTime * TreadScrollScale;
	if (TreadLMID) { TreadLMID->SetScalarParameterValue(TEXT("Scroll"), TreadScroll); }
	if (TreadRMID) { TreadRMID->SetScalarParameterValue(TEXT("Scroll"), TreadScroll); }
}

void ABLBossPawn::OnBossDamaged(float /*Amount*/, float HealthRemaining)
{
	const float Fraction = HealthRemaining / FMath::Max(Health->GetMaxHealth(), 1.f);
	if (Phase < 3 && Fraction <= Phase3Fraction)
	{
		Phase = 3;
		EnterPhase(3);
	}
	else if (Phase < 2 && Fraction <= Phase2Fraction)
	{
		Phase = 2;
		EnterPhase(2);
	}
}

void ABLBossPawn::EnterPhase(int32 NewPhase)
{
	OnPhaseChanged.Broadcast(NewPhase);

	// escalate the boss music bed at the 66% / 33% thresholds (Bible 4.6)
	if (UBLAudioSubsystem* A = GetWorld()->GetSubsystem<UBLAudioSubsystem>())
	{
		A->SetMusicState(NewPhase >= 3 ? EBLMusicState::BossP3
			: (NewPhase == 2 ? EBLMusicState::BossP2 : EBLMusicState::BossP1));
		A->PostBossVO(TEXT("Foundryman"),
			NewPhase >= 3 ? EBLBossVO::Phase3 : EBLBossVO::Phase2, GetActorLocation());
	}

	// Phase 2 - Tap starts working the mill against you: pours come twice as often
	// (Mill Build Plan section 23: ~90s normal -> ~45s in Phase 2).
	if (NewPhase == 2)
	{
		int32 Index = 0;
		for (TActorIterator<ABLHazard_FurnacePour> It(GetWorld()); It; ++It)
		{
			It->CommandeerCycle(45.f, 5.f + Index * 6.f); // staggered, doubled cadence
			Index++;
		}
	}

	// Phase 3 - "Tap the Heat": commandeer every furnace; the Mill itself attacks
	if (NewPhase >= 3)
	{
		int32 Index = 0;
		for (TActorIterator<ABLHazard_FurnacePour> It(GetWorld()); It; ++It)
		{
			It->CommandeerCycle(18.f, 2.f + Index * 5.f); // staggered pours, relentless
			Index++;
		}

		// the shell cracks: he overheats - faster, and leaves a permanent fire trail
		bOverheated = true;
		MaxSpeedKph = OverheatSpeedKph;
		SlamClock = SlamInterval * 0.5f;   // first slam comes fairly soon after the crack
		GetWorldTimerManager().SetTimer(
			FireTrailTimer, this, &ABLBossPawn::DropFireTrail, 0.18f, true);
	}
	// every phase entry is a moment: big shake, brief stagger window comes with attacks
	if (UBLImpactFXSubsystem* FX = GetWorld()->GetSubsystem<UBLImpactFXSubsystem>())
	{
		FX->PlayImpact(GetActorLocation(), EBLImpactWeight::Massive);
	}
	// TODO: per-phase attack sets (telegraph -> window), Bible 4.6 - difficulty
	// scales frequency/HP, never the tells
#if !(UE_BUILD_SHIPPING)
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red,
			FString::Printf(TEXT("FOUNDRYMAN PHASE %d"), NewPhase));
	}
#endif
}

void ABLBossPawn::OnBossDeath()
{
	// stop the fight: clear timers, close the window, let the shell go dark
	GetWorldTimerManager().ClearTimer(FireTrailTimer);
	GetWorldTimerManager().ClearTimer(SlamHitTimer);
	GetWorldTimerManager().ClearTimer(ExposeTimer);
	GetWorldTimerManager().ClearTimer(MortarLaunchTimer);
	bSlamTelegraph = false;
	bCoreExposed = false;
	if (Health)
	{
		Health->IncomingDamageMultiplier = 1.f;
	}
	if (CoreMID)
	{
		CoreMID->SetScalarParameterValue(TEXT("Strength"), 0.f);   // the man inside finally cools
	}
	// a final detonation on the cupola
	if (UBLImpactFXSubsystem* FX = GetWorld()->GetSubsystem<UBLImpactFXSubsystem>())
	{
		FX->PlayImpact(GetActorLocation() + FVector(0.f, 0.f, 300.f), EBLImpactWeight::Massive);
	}
	if (UBLAudioSubsystem* A = GetWorld()->GetSubsystem<UBLAudioSubsystem>())
	{
		A->PostBossVO(TEXT("Foundryman"), EBLBossVO::Death, GetActorLocation());
	}
	// the unlock + result screen are driven by ABLGameMode::NotifyVehicleDeath
#if !(UE_BUILD_SHIPPING)
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Cyan,
			TEXT("THE FOUNDRYMAN FALLS - COLLECTOR CLAIMED (unlock flow TODO)"));
	}
#endif
}

// dev/test: spawn the boss ~40 m ahead of the player, facing them.
//   bl.SpawnBoss
static FAutoConsoleCommandWithWorldAndArgs GBLSpawnBossCmd(
	TEXT("bl.SpawnBoss"),
	TEXT("Spawn the Foundryman boss pawn in front of the player"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>&, UWorld* World)
		{
			if (!World)
			{
				return;
			}
			const APlayerController* PC = World->GetFirstPlayerController();
			const APawn* Player = PC ? PC->GetPawn() : nullptr;
			if (!Player)
			{
				return;
			}
			const FVector Loc = Player->GetActorLocation()
				+ Player->GetActorForwardVector() * 4000.f;
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			ABLBossPawn* Boss = World->SpawnActor<ABLBossPawn>(
				ABLBossPawn::StaticClass(), Loc,
				(-Player->GetActorForwardVector()).Rotation(), Params);
			if (Boss)
			{
				Boss->RiseAt(Loc);
			}
		}));
