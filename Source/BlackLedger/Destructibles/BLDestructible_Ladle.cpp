// Black Ledger - "Tap the Ladle"

#include "BLDestructible_Ladle.h"
#include "Components/StaticMeshComponent.h"
#include "Hazards/BLHazard_FurnacePour.h"
#include "Kismet/GameplayStatics.h"
#include "Vehicles/BLHealthComponent.h"

ABLDestructible_Ladle::ABLDestructible_Ladle()
{
	// the centrepiece: tougher than a catwalk support (it's "three pins" of HP)
	Health->BaseHP = 500.f;

	// BEFORE: the hanging torpedo ladle body (deck top is 8.5 m; it hangs at ~15 m) + 3
	// release pins on the chains. One health pool reads as "tap the pins until it gives".
	MakePiece(TEXT("Ladle_Body"), FVector(0.f, 0.f, 1500.f),
		FVector(3.5f, 3.5f, 3.f), FRotator::ZeroRotator, false);
	MakePiece(TEXT("Pin_0"), FVector(180.f, 0.f, 1820.f),
		FVector(0.5f, 0.5f, 1.2f), FRotator::ZeroRotator, false);
	MakePiece(TEXT("Pin_1"), FVector(-120.f, 150.f, 1820.f),
		FVector(0.5f, 0.5f, 1.2f), FRotator::ZeroRotator, false);
	MakePiece(TEXT("Pin_2"), FVector(-120.f, -150.f, 1820.f),
		FVector(0.5f, 0.5f, 1.2f), FRotator::ZeroRotator, false);

	// AFTER: the emptied ladle drops to a low central cover blob
	MakePiece(TEXT("Ladle_Dropped"), FVector(0.f, 0.f, 300.f),
		FVector(4.f, 4.f, 2.4f), FRotator(0.f, 0.f, 8.f), true);
}

void ABLDestructible_Ladle::OnCollapsed()
{
	UWorld* W = GetWorld();
	if (!W)
	{
		return;
	}
	// dump slag into the centre: a recurring circular pour that weaponises the middle.
	// deferred-spawn so the hazard reads its shape/radius in BeginPlay.
	const FVector Loc(GetActorLocation().X, GetActorLocation().Y, 150.f);
	const FTransform Xf(FRotator::ZeroRotator, Loc);
	ABLHazard_FurnacePour* H = W->SpawnActorDeferred<ABLHazard_FurnacePour>(
		ABLHazard_FurnacePour::StaticClass(), Xf);
	if (H)
	{
		H->PourShape = EBLPourShape::Circular;
		H->ZoneRadius = DumpHazardRadius;
		H->CooldownSeconds = DumpHazardCooldown;
		UGameplayStatics::FinishSpawningActor(H, Xf);
		H->CommandeerCycle(DumpHazardCooldown, 1.5f); // first dump almost immediately
	}
}
