// Black Ledger - front-end game mode (L_MainMenu).
//
// No roster spawn, no boss gate - it exists only to hold the menu map and raise
// the main menu through UBLUISubsystem once a local PlayerController exists.
// The arena uses ABLGameMode; this is its quiet sibling for the title screen.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BLMenuGameMode.generated.h"

UCLASS()
class BLACKLEDGER_API ABLMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	void RaiseMainMenu();
	FTimerHandle MenuTimer;
};
