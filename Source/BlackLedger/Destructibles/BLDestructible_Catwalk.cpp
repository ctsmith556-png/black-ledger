// Black Ledger - the Mill catwalk collapse
// Greybox dimensions (cm) at the Arena Build Brief scale (260 m floor, deck
// width 12 m per the brief's 10-14 m catwalk lane). Deck top at z=620 -
// drivable speed-skill shortcut. Entry ramps ~13 deg; wreck-ramps gentler.

#include "BLDestructible_Catwalk.h"

ABLDestructible_Catwalk::ABLDestructible_Catwalk()
{
	// ---- permanent: end gantries + entry ramps (survive the collapse) ----
	MakePiece(TEXT("Gantry_W"), FVector(-5500.f, 0.f, 595.f),
		FVector(20.f, 12.f, 0.5f), FRotator::ZeroRotator, false, /*bPermanent*/ true);
	MakePiece(TEXT("Gantry_E"), FVector(5500.f, 0.f, 595.f),
		FVector(20.f, 12.f, 0.5f), FRotator::ZeroRotator, false, true);
	MakePiece(TEXT("RampEntry_W"), FVector(-7800.f, 0.f, 310.f),
		FVector(27.f, 12.f, 0.5f), FRotator(13.4f, 0.f, 0.f), false, true);
	MakePiece(TEXT("RampEntry_E"), FVector(7800.f, 0.f, 310.f),
		FVector(27.f, 12.f, 0.5f), FRotator(-13.4f, 0.f, 0.f), false, true);

	// ---- before: the mid-span deck + two shootable supports ----
	MakePiece(TEXT("Deck_Mid"), FVector(0.f, 0.f, 595.f),
		FVector(90.f, 12.f, 0.5f), FRotator::ZeroRotator, false);
	MakePiece(TEXT("Support_W"), FVector(-2200.f, 0.f, 285.f),
		FVector(1.8f, 1.8f, 5.7f), FRotator::ZeroRotator, false);
	MakePiece(TEXT("Support_E"), FVector(2200.f, 0.f, 285.f),
		FVector(1.8f, 1.8f, 5.7f), FRotator::ZeroRotator, false);

	// ---- after: wreck-ramps from the floor up to the surviving gantries ----
	MakePiece(TEXT("RampFall_W"), FVector(-2400.f, 0.f, 290.f),
		FVector(42.f, 12.f, 0.5f), FRotator(-8.4f, 0.f, 0.f), true);
	MakePiece(TEXT("RampFall_E"), FVector(2400.f, 0.f, 290.f),
		FVector(42.f, 12.f, 0.5f), FRotator(8.4f, 0.f, 0.f), true);
	MakePiece(TEXT("Rubble_0"), FVector(180.f, 420.f, 110.f),
		FVector(2.6f, 2.2f, 2.2f), FRotator(0.f, 24.f, 0.f), true);
	MakePiece(TEXT("Rubble_1"), FVector(-380.f, -300.f, 90.f),
		FVector(2.f, 1.7f, 1.7f), FRotator(0.f, -37.f, 0.f), true);
	MakePiece(TEXT("Rubble_2"), FVector(60.f, -480.f, 70.f),
		FVector(1.5f, 1.3f, 1.3f), FRotator(0.f, 58.f, 0.f), true);
}
