// Black Ledger - game mode

#include "BLGameMode.h"
#include "Engine/TargetPoint.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "HAL/IConsoleManager.h"
#include "Vehicles/BLCombatVehicle.h"

// test override for the field size: set in the PIE console, takes effect on the
// next PIE session (spawning happens at match start). -1 = use the GameMode value.
//   bl.NumAI 3
static TAutoConsoleVariable<int32> CVarBLNumAI(
	TEXT("bl.NumAI"), -1,
	TEXT("Override the number of AI vehicles spawned at match start (-1 = GameMode default)"));

ABLGameMode::ABLGameMode()
{
	DefaultPawnClass = ABLCombatVehicle::StaticClass();
}

void ABLGameMode::BeginPlay()
{
	Super::BeginPlay();

	// ring center = the player start (falls back to world origin in a bare map)
	FVector Center = FVector::ZeroVector;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		Center = It->GetActorLocation();
		break;
	}

	const int32 Override = CVarBLNumAI.GetValueOnGameThread();
	const int32 NumToSpawn = (Override >= 0) ? Override : NumAIVehicles;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// preferred: FFA spawn nodes (TargetPoints tagged "BLSpawn", per the arena
	// build briefs - start zones with no spawn-kill sightlines)
	TArray<ATargetPoint*> Nodes;
	for (TActorIterator<ATargetPoint> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(FName(TEXT("BLSpawn"))))
		{
			Nodes.Add(*It);
		}
	}

	for (int32 i = 0; i < NumToSpawn; ++i)
	{
		FVector Loc;
		FRotator Rot;
		if (Nodes.Num() > 0)
		{
			const ATargetPoint* Node = Nodes[i % Nodes.Num()];
			// wrap-around spawns offset sideways so they never stack
			const float Wrap = static_cast<float>(i / Nodes.Num()) * 450.f;
			Loc = Node->GetActorLocation()
				+ Node->GetActorRightVector() * Wrap + FVector(0, 0, SpawnHeight);
			Rot = FRotator(0.f, Node->GetActorRotation().Yaw, 0.f);
		}
		else
		{
			// fallback: even ring around the player start, facing inward
			const float Angle = 2.f * PI * static_cast<float>(i) / FMath::Max(NumToSpawn, 1);
			const FVector Offset(FMath::Cos(Angle) * SpawnRingRadius,
				FMath::Sin(Angle) * SpawnRingRadius, SpawnHeight);
			Loc = Center + Offset;
			Rot = (-Offset).GetSafeNormal2D().Rotation();
		}
		GetWorld()->SpawnActor<ABLCombatVehicle>(ABLCombatVehicle::StaticClass(), Loc, Rot, Params);
		// AutoPossessAI (PlacedInWorldOrSpawned) gives each one an ABLAIController
	}
}
