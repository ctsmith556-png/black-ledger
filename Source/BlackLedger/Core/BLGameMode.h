// Black Ledger - match rules / spawning (TDD section 1).
// Phase 2: spawns the AI field - a TM:B-sized lobby where everyone fights
// everyone (the AI targets the nearest living vehicle, player or AI).
// Roster variety, win/lose conditions, and boss gates come later.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BLGameMode.generated.h"

UCLASS()
class BLACKLEDGER_API ABLGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABLGameMode();

	/** 13 AI + the player = 14 vehicles, TM:B-style. Set 1 for a duel while iterating. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BL|Match")
	int32 NumAIVehicles = 13;

	UPROPERTY(EditAnywhere, Category = "BL|Match")
	float SpawnRingRadius = 5000.f;   // cm from the ring center

	UPROPERTY(EditAnywhere, Category = "BL|Match")
	float SpawnHeight = 150.f;        // drop-in height; they settle on suspension

protected:
	virtual void BeginPlay() override;
};
