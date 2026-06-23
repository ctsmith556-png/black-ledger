// Black Ledger - save data (TDD section 10)
// Slice scope: unlocked Collectors (beat the Foundryman -> he's playable),
// the last-played character (so Continue knows where to land), the chosen
// difficulty (so Options persists), and a "has progress" flag the Main Menu
// reads to enable/disable Continue. Endings-seen + per-character campaign
// progress fill in when the campaign hub lands.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Core/BLGameInstance.h"   // EBLDifficulty
#include "BLSaveGame.generated.h"

UCLASS()
class BLACKLEDGER_API UBLSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	static constexpr const TCHAR* SlotName = TEXT("BlackLedger");

	/** Collectors the player has defeated (slice: just "Foundryman"). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Save")
	TArray<FName> UnlockedCollectors;

	/** Last contestant the player opened a file on - where Continue resumes. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Save")
	FName LastCharacter = TEXT("Surgeon");

	/** Persisted difficulty (Options menu writes it; AI scales from it). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Save")
	EBLDifficulty Difficulty = EBLDifficulty::Medium;

	/** Set once the player has started a run - gates "Continue" on the menu. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Save")
	bool bHasProgress = false;
};
