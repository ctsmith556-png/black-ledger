// Black Ledger - front-end game mode

#include "BLMenuGameMode.h"
#include "UI/BLUISubsystem.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

void ABLMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
	// the local PlayerController isn't guaranteed to exist at GameMode BeginPlay;
	// a short timer lets login finish before we grab input/cursor for the menu.
	GetWorldTimerManager().SetTimer(MenuTimer, this, &ABLMenuGameMode::RaiseMainMenu, 0.2f, false);
}

void ABLMenuGameMode::RaiseMainMenu()
{
	UGameInstance* GI = GetGameInstance();
	// need both the subsystem AND a local PlayerController, or the menu would draw
	// with no cursor/input bound
	if (GI && GI->GetFirstLocalPlayerController())
	{
		if (UBLUISubsystem* UI = GI->GetSubsystem<UBLUISubsystem>())
		{
			UI->ShowMainMenu();
			return;
		}
	}
	// not ready yet - try again shortly
	GetWorldTimerManager().SetTimer(MenuTimer, this, &ABLMenuGameMode::RaiseMainMenu, 0.2f, false);
}
