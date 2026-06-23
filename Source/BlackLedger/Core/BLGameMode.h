// Black Ledger - match rules / spawning (TDD section 1).
// Phase 2: spawns the AI field - a TM:B-sized lobby where everyone fights
// everyone (the AI targets the nearest living vehicle, player or AI).
// Roster variety, win/lose conditions, and boss gates come later.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BLGameMode.generated.h"

class ABLCombatVehicle;
class AController;

UENUM(BlueprintType)
enum class EBLMatchState : uint8
{
	Brawl,      // the pre-boss field fight
	BossFight,  // the Collector has risen
	Victory,    // boss claimed - unlock saved
	Defeat      // account closed
};

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

	/** Slice flow: clear the AI field -> the Collector rises at the BLBossRise
	 *  point (Foundryman from Furnace 3). Off for pure-brawl testing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BL|Match")
	bool bBossFight = true;

	UFUNCTION(BlueprintPure, Category = "BL|Match")
	EBLMatchState GetMatchState() const { return MatchState; }

	/** True only on the victory that first unlocked the Collector (HUD callout). */
	UFUNCTION(BlueprintPure, Category = "BL|Match")
	bool WasNewUnlock() const { return bNewUnlock; }

	/** Called by vehicles as they die; drives Victory/Defeat. */
	void NotifyVehicleDeath(ABLCombatVehicle* Dead);

	/** R after Victory/Defeat: reload the arena. */
	void RequestRestart();

protected:
	virtual void BeginPlay() override;

	/** On the title map (L_MainMenu) the player gets no vehicle - just a camera,
	 *  so the Slate menu has something behind it instead of a spawned brawl. */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

private:
	/** True when this GameMode is running on the front-end map. Lets the global
	 *  default GameMode double as the menu host even if the map's World Settings
	 *  GameMode override never got applied. */
	bool IsMenuLevel() const;

	void CheckBossRise();
	void SaveCollectorUnlock(FName CollectorName);
	void RaiseResultScreen();   // hands match-end off to UBLUISubsystem after a beat
	void RaiseMenu();           // front-end map: show the main menu once a PC exists

	FTimerHandle BossCheckTimer;
	FTimerHandle ResultTimer;
	FTimerHandle MenuTimer;
	bool bBossSpawned = false;
	bool bNewUnlock = false;
	EBLMatchState MatchState = EBLMatchState::Brawl;
};
