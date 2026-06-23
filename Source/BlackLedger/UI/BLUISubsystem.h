// Black Ledger - front-end UI manager (TDD section 9; SCREEN_FLOW.md).
//
// One lightweight manager that owns the whole out-of-match flow and the in-match
// overlays that are NOT the HUD: Cold-Open -> Main Menu -> Roster -> (match) ->
// Pause / Result / Unlock / Ending. Lives on the GameInstance so it survives the
// OpenLevel between the menu map and the arena.
//
// Built in hand-rolled C++ Slate (no UMG widgets / .uassets) so the entire menu
// layer compiles through Build.bat - matching how the rest of this project is
// authored from scripts. The dossier-styled UMG art pass can replace it later
// without touching the flow logic, which all lives here.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/TimerHandle.h"
#include "Core/BLGameInstance.h"   // EBLDifficulty
#include "BLUISubsystem.generated.h"

class SWidget;
class APlayerController;
class UBLSaveGame;
struct FBLVehicleBio;

UCLASS()
class BLACKLEDGER_API UBLUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// ---- screens (SCREEN_FLOW.md) ----
	void ShowMainMenu();
	void ShowRoster();
	void ShowOptions();

	// roster drill-down: a contestant's dossier, and its intro storyboard
	void ShowBiography(const FBLVehicleBio* Bio);
	void ShowIntro(const FBLVehicleBio* Bio);          // replays the intro motion-comic

	/** Paced, skippable motion-comic: shows Beats one at a time (auto-advance +
	 *  Next/Skip), then fires OnComplete. Used for both intro and ending. */
	void PlayCinematic(const TArray<FString>& Beats, TFunction<void()> OnComplete);

	/** Called by the GameMode when the match resolves. Routes to the right
	 *  end-screen: Defeat, plain Victory, or (first kill) the Unlock screen. */
	void ShowResult(bool bVictory, bool bNewUnlock);

	/** Esc / Start in-match: pause the world and raise the pause overlay, or
	 *  lower it and resume. Safe to call from a paused input binding. */
	void TogglePause();

	// ---- flow actions (bound to the menu buttons) ----
	void StartMatch();          // open the Mill with the selected contestant
	void RetryMatch();          // reload the current arena (Defeat -> Retry)
	void QuitToMenu();          // back to L_MainMenu
	void QuitGame();

	// ---- persistence helpers the screens read ----
	UFUNCTION(BlueprintPure, Category = "BL|UI")
	bool HasSaveProgress() const;

	UFUNCTION(BlueprintPure, Category = "BL|UI")
	bool IsCollectorUnlocked(FName CollectorName) const;

	EBLDifficulty GetDifficulty() const;
	void SetDifficulty(EBLDifficulty NewDifficulty);   // writes GameInstance + save

private:
	// screen factory methods - each returns a fresh Slate tree wired to the
	// flow actions above; lambdas capture `this` (the subsystem outlives them).
	TSharedRef<SWidget> BuildMainMenu();
	TSharedRef<SWidget> BuildRoster();
	TSharedRef<SWidget> BuildBiography(const FBLVehicleBio* Bio);
	TSharedRef<SWidget> BuildOptions();
	TSharedRef<SWidget> BuildPause();
	TSharedRef<SWidget> BuildResult(bool bVictory);
	TSharedRef<SWidget> BuildUnlock();
	TSharedRef<SWidget> BuildCinematic();   // one motion-comic panel (intro/ending)

	// deploy = play the chosen contestant's intro, then enter the match
	void DeployContestant(const FBLVehicleBio* Bio);

	// cinematic player internals
	void ShowCineBeat();
	void CineNext();        // advance one beat (or finish on the last)
	void CineFinish();      // tear down + fire the completion callback

	// viewport + input plumbing
	void SetScreen(TSharedRef<SWidget> NewScreen, bool bWantCursor);
	void ClearScreen();
	void ResumeGame();          // tears down any overlay + returns game input
	APlayerController* LocalPC() const;

	// save-slot access
	UBLSaveGame* LoadOrCreateSave() const;
	void CommitSave(UBLSaveGame* Save) const;

	TSharedPtr<SWidget> CurrentScreen;
	bool bPaused = false;

	// cinematic state
	TArray<FString> CineBeats;
	int32 CineIndex = 0;
	TFunction<void()> CineOnComplete;
	FTimerHandle CineTimer;

	// the contestant the player deployed - drives which ending plays
	const FBLVehicleBio* DeployedBio = nullptr;
};
