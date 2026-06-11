// Black Ledger - game instance (TDD section 1): persists across levels.
// Phase 2: holds the difficulty setting the AI scales from. Later: current
// character, unlocks, save handle. The options menu (Phase 3 UI) writes
// SetDifficulty; until then use the console: bl.SetDifficulty 0|1|2

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BLGameInstance.generated.h"

UENUM(BlueprintType)
enum class EBLDifficulty : uint8
{
	Easy,
	Medium,
	Hard
};

UCLASS()
class BLACKLEDGER_API UBLGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "BL|Settings")
	void SetDifficulty(EBLDifficulty NewDifficulty) { Difficulty = NewDifficulty; }

	UFUNCTION(BlueprintPure, Category = "BL|Settings")
	EBLDifficulty GetDifficulty() const { return Difficulty; }

private:
	UPROPERTY()
	EBLDifficulty Difficulty = EBLDifficulty::Medium;
};
