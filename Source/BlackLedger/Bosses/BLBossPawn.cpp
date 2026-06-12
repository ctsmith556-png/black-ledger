// Black Ledger - Collector boss pawn base (Foundryman defaults)

#include "BLBossPawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "FX/BLImpactFXSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Vehicles/BLHealthComponent.h"

ABLBossPawn::ABLBossPawn()
{
	// ---- The Foundryman "Tap" (tuning sheet Bosses tab) ----
	// Speed 2 / Armor 14 / HP x6 -> 11,133 effective. Immovable, furnace-immune.
	Health->BaseHP = 6000.f;        // 6000 * (0.7 + 13 * 0.0889) = 11,133
	Health->ArmorStat = 14.f;       // over-budget by design
	bHazardImmune = true;           // he is the furnace

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
}

void ABLBossPawn::BeginPlay()
{
	Super::BeginPlay();
	Health->OnDamaged.AddDynamic(this, &ABLBossPawn::OnBossDamaged);
	Health->OnDeath.AddDynamic(this, &ABLBossPawn::OnBossDeath);
}

void ABLBossPawn::RiseAt(const FVector& Location)
{
	SetActorLocation(Location + FVector(0.f, 0.f, 200.f), false, nullptr, ETeleportType::TeleportPhysics);
	if (UBLImpactFXSubsystem* FX = GetWorld()->GetSubsystem<UBLImpactFXSubsystem>())
	{
		FX->PlayImpact(Location, EBLImpactWeight::Massive); // placeholder arrival quake
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
	// TODO(unlock flow): flag the Collector unlocked in UBLSaveGame -> roster
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
