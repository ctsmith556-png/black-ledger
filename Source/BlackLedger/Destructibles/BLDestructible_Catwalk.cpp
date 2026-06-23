// Black Ledger - the Mill catwalk collapse
// Greybox dimensions (cm). Deck top at z=850 (8.5 m) - raised so the 18 m
// Foundryman (6.1 m tall) passes clean underneath. 12 m lane per the brief.
// Entry ramps ~13.4 deg; wreck-ramps ~11.4 deg.

#include "BLDestructible_Catwalk.h"
#include "Components/StaticMeshComponent.h"

ABLDestructible_Catwalk::ABLDestructible_Catwalk()
{
	// ---- permanent: end gantries + entry ramps (survive the collapse) ----
	MakePiece(TEXT("Gantry_W"), FVector(-5500.f, 0.f, 825.f),
		FVector(20.f, 12.f, 0.5f), FRotator::ZeroRotator, false, /*bPermanent*/ true);
	MakePiece(TEXT("Gantry_E"), FVector(5500.f, 0.f, 825.f),
		FVector(20.f, 12.f, 0.5f), FRotator::ZeroRotator, false, true);
	// gentler 9 deg entry ramps (was 13.4 deg - too steep for the arcade pawn), lengthened
	// so they still meet the deck top (z=850) at the gantry edge and the floor.
	// Ramp ends flush AT the gantry outer edge (x=+/-6500), top surface ~6 cm above the gantry
	// (z=850), so the only transition is a tiny roll-DOWN onto the deck - never an up-step
	// wall. No separate bridge piece (that one added a lip the car caught on).
	EntryRampW = MakePiece(TEXT("RampEntry_W"), FVector(-9216.f, 0.f, 401.f),
		FVector(55.f, 12.f, 0.5f), FRotator(9.f, 0.f, 0.f), false, true);
	EntryRampE = MakePiece(TEXT("RampEntry_E"), FVector(9216.f, 0.f, 401.f),
		FVector(55.f, 12.f, 0.5f), FRotator(-9.f, 0.f, 0.f), false, true);

	// ---- before: the mid-span deck + two shootable supports ----
	MakePiece(TEXT("Deck_Mid"), FVector(0.f, 0.f, 825.f),
		FVector(90.f, 12.f, 0.5f), FRotator::ZeroRotator, false);
	MakePiece(TEXT("Support_W"), FVector(-2200.f, 0.f, 400.f),
		FVector(2.2f, 2.2f, 8.f), FRotator::ZeroRotator, false);
	MakePiece(TEXT("Support_E"), FVector(2200.f, 0.f, 400.f),
		FVector(2.2f, 2.2f, 8.f), FRotator::ZeroRotator, false);

	// ---- after: wreck-ramps from the floor up to the surviving gantries ----
	MakePiece(TEXT("RampFall_W"), FVector(-2400.f, 0.f, 405.f),
		FVector(43.f, 12.f, 0.5f), FRotator(-11.4f, 0.f, 0.f), true);
	MakePiece(TEXT("RampFall_E"), FVector(2400.f, 0.f, 405.f),
		FVector(43.f, 12.f, 0.5f), FRotator(11.4f, 0.f, 0.f), true);
	MakePiece(TEXT("Rubble_0"), FVector(180.f, 420.f, 110.f),
		FVector(2.6f, 2.2f, 2.2f), FRotator(0.f, 24.f, 0.f), true);
	MakePiece(TEXT("Rubble_1"), FVector(-380.f, -300.f, 90.f),
		FVector(2.f, 1.7f, 1.7f), FRotator(0.f, -37.f, 0.f), true);
	MakePiece(TEXT("Rubble_2"), FVector(60.f, -480.f, 70.f),
		FVector(1.5f, 1.3f, 1.3f), FRotator(0.f, 58.f, 0.f), true);
}

void ABLDestructible_Catwalk::BeginPlay()
{
	Super::BeginPlay();
	// loop-section mode: hide the standalone entry ramps (the loop road feeds the deck)
	if (!bEntryRamps)
	{
		for (UStaticMeshComponent* R : {EntryRampW.Get(), EntryRampE.Get()})
		{
			if (R)
			{
				R->SetVisibility(false);
				R->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}
