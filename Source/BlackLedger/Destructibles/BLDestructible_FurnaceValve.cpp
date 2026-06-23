// Black Ledger - "Blow the Furnace"

#include "BLDestructible_FurnaceValve.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Vehicles/BLCombatVehicle.h"
#include "Vehicles/BLHealthComponent.h"

ABLDestructible_FurnaceValve::ABLDestructible_FurnaceValve()
{
	Health->BaseHP = 400.f;

	// BEFORE: a glowing pressure-valve cluster on the furnace lip (the shootable target)
	MakePiece(TEXT("Valve_0"), FVector(0.f, 0.f, 450.f),
		FVector(1.2f, 1.2f, 1.6f), FRotator::ZeroRotator, false);
	MakePiece(TEXT("Valve_1"), FVector(160.f, 120.f, 350.f),
		FVector(0.8f, 0.8f, 1.1f), FRotator::ZeroRotator, false);

	// AFTER: a scorched stump
	MakePiece(TEXT("Valve_Wrecked"), FVector(0.f, 0.f, 250.f),
		FVector(1.6f, 1.6f, 0.6f), FRotator(0.f, 0.f, 6.f), true);
}

void ABLDestructible_FurnaceValve::OnCollapsed()
{
	UWorld* W = GetWorld();
	if (!W)
	{
		return;
	}
	const FVector C = GetActorLocation();

	// detonate once: kill anything too close (the boss is hazard-immune)
	for (TActorIterator<APawn> It(W); It; ++It)
	{
		APawn* Pawn = *It;
		if (FVector::DistSquared2D(Pawn->GetActorLocation(), C) > FMath::Square(BlastRadius))
		{
			continue;
		}
		if (const ABLCombatVehicle* V = Cast<ABLCombatVehicle>(Pawn))
		{
			if (V->bHazardImmune)
			{
				continue;
			}
		}
		if (UBLHealthComponent* H = Pawn->FindComponentByClass<UBLHealthComponent>())
		{
			H->ApplyDamage(BlastDamage);
		}
	}

	// open the sealed bay: drop any tagged blocker -> the collision change re-knits navmesh
	for (TActorIterator<AActor> It(W); It; ++It)
	{
		if (It->ActorHasTag(SealedDoorTag))
		{
			It->SetActorHiddenInGame(true);
			It->SetActorEnableCollision(false);
		}
	}
}
