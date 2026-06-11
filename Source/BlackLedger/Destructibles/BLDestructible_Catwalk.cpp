// Black Ledger - the Mill catwalk collapse
// Greybox dimensions (cm). Deck top sits at z=620 (6.2 m) - drivable shortcut.
// Entry ramps rise over 26 m (~13.4 deg); wreck-ramps are a touch steeper.

#include "BLDestructible_Catwalk.h"

ABLDestructible_Catwalk::ABLDestructible_Catwalk()
{
	// ---- permanent: end gantries + entry ramps (survive the collapse) ----
	MakePiece(TEXT("Gantry_W"), FVector(-3300.f, 0.f, 595.f),
		FVector(14.f, 7.f, 0.5f), FRotator::ZeroRotator, false, /*bPermanent*/ true);
	MakePiece(TEXT("Gantry_E"), FVector(3300.f, 0.f, 595.f),
		FVector(14.f, 7.f, 0.5f), FRotator::ZeroRotator, false, true);
	MakePiece(TEXT("RampEntry_W"), FVector(-5300.f, 0.f, 310.f),
		FVector(27.f, 7.f, 0.5f), FRotator(13.4f, 0.f, 0.f), false, true);
	MakePiece(TEXT("RampEntry_E"), FVector(5300.f, 0.f, 310.f),
		FVector(27.f, 7.f, 0.5f), FRotator(-13.4f, 0.f, 0.f), false, true);

	// ---- before: the mid-span deck + two shootable supports ----
	MakePiece(TEXT("Deck_Mid"), FVector(0.f, 0.f, 595.f),
		FVector(52.f, 7.f, 0.5f), FRotator::ZeroRotator, false);
	MakePiece(TEXT("Support_W"), FVector(-1300.f, 0.f, 285.f),
		FVector(1.6f, 1.6f, 5.7f), FRotator::ZeroRotator, false);
	MakePiece(TEXT("Support_E"), FVector(1300.f, 0.f, 285.f),
		FVector(1.6f, 1.6f, 5.7f), FRotator::ZeroRotator, false);

	// ---- after: wreck-ramps from the floor up to the surviving gantries ----
	MakePiece(TEXT("RampFall_W"), FVector(-1475.f, 0.f, 290.f),
		FVector(23.f, 7.f, 0.5f), FRotator(-15.4f, 0.f, 0.f), true);
	MakePiece(TEXT("RampFall_E"), FVector(1475.f, 0.f, 290.f),
		FVector(23.f, 7.f, 0.5f), FRotator(15.4f, 0.f, 0.f), true);
	MakePiece(TEXT("Rubble_0"), FVector(120.f, 260.f, 90.f),
		FVector(2.2f, 1.8f, 1.8f), FRotator(0.f, 24.f, 0.f), true);
	MakePiece(TEXT("Rubble_1"), FVector(-260.f, -180.f, 70.f),
		FVector(1.6f, 1.4f, 1.4f), FRotator(0.f, -37.f, 0.f), true);
	MakePiece(TEXT("Rubble_2"), FVector(40.f, -320.f, 55.f),
		FVector(1.2f, 1.1f, 1.1f), FRotator(0.f, 58.f, 0.f), true);
}
