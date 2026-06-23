// Black Ledger - game mode

#include "BLGameMode.h"
#include "Audio/BLAudioSubsystem.h"
#include "Bosses/BLBossPawn.h"
#include "Core/BLSaveGame.h"
#include "Engine/GameInstance.h"
#include "Engine/TargetPoint.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/PlayerStart.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "UI/BLHUD.h"
#include "UI/BLUISubsystem.h"
#include "Vehicles/BLCombatVehicle.h"
#include "Vehicles/BLHealthComponent.h"

// test override for the field size: set in the PIE console, takes effect on the
// next PIE session (spawning happens at match start). -1 = use the GameMode value.
//   bl.NumAI 3
static TAutoConsoleVariable<int32> CVarBLNumAI(
	TEXT("bl.NumAI"), -1,
	TEXT("Override the number of AI vehicles spawned at match start (-1 = GameMode default)"));

ABLGameMode::ABLGameMode()
{
	DefaultPawnClass = ABLCombatVehicle::StaticClass();
	HUDClass = ABLHUD::StaticClass();
}

void ABLGameMode::NotifyVehicleDeath(ABLCombatVehicle* Dead)
{
	if (!Dead || MatchState == EBLMatchState::Victory || MatchState == EBLMatchState::Defeat)
	{
		return;
	}
	if (Dead->IsA<ABLBossPawn>())
	{
		MatchState = EBLMatchState::Victory;
		SaveCollectorUnlock(TEXT("Foundryman"));
	}
	else if (Dead->IsPlayerControlled())
	{
		MatchState = EBLMatchState::Defeat;
	}
	else
	{
		return; // an AI-vs-AI death - match continues, no end-screen
	}

	// let the death moment / kill-cam breathe, then raise the result screen
	GetWorldTimerManager().SetTimer(
		ResultTimer, this, &ABLGameMode::RaiseResultScreen, 1.6f, false);
}

void ABLGameMode::RaiseMenu()
{
	UGameInstance* GI = GetGameInstance();
	// need a local PlayerController too, or the menu draws with no cursor/input
	if (GI && GI->GetFirstLocalPlayerController())
	{
		if (UBLUISubsystem* UI = GI->GetSubsystem<UBLUISubsystem>())
		{
			UI->ShowMainMenu();
			return;
		}
	}
	GetWorldTimerManager().SetTimer(MenuTimer, this, &ABLGameMode::RaiseMenu, 0.2f, false);
}

void ABLGameMode::RaiseResultScreen()
{
	const bool bVictory = (MatchState == EBLMatchState::Victory);
	if (UBLAudioSubsystem* A = GetWorld()->GetSubsystem<UBLAudioSubsystem>())
	{
		A->SetMusicState(bVictory ? EBLMusicState::Victory : EBLMusicState::Death);
	}
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UBLUISubsystem* UI = GI->GetSubsystem<UBLUISubsystem>())
		{
			UI->ShowResult(bVictory, bNewUnlock);
		}
	}
}

void ABLGameMode::SaveCollectorUnlock(FName CollectorName)
{
	UBLSaveGame* Save = Cast<UBLSaveGame>(
		UGameplayStatics::LoadGameFromSlot(UBLSaveGame::SlotName, 0));
	if (!Save)
	{
		Save = Cast<UBLSaveGame>(UGameplayStatics::CreateSaveGameObject(UBLSaveGame::StaticClass()));
	}
	if (Save)
	{
		bNewUnlock = !Save->UnlockedCollectors.Contains(CollectorName);
		Save->UnlockedCollectors.AddUnique(CollectorName);
		UGameplayStatics::SaveGameToSlot(Save, UBLSaveGame::SlotName, 0);
	}
}

void ABLGameMode::RequestRestart()
{
	if (MatchState == EBLMatchState::Victory || MatchState == EBLMatchState::Defeat)
	{
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
	}
}

bool ABLGameMode::IsMenuLevel() const
{
	// GetMapName carries the PIE prefix (e.g. "UEDPIE_0_L_MainMenu"), so match loosely
	return GetWorld() && GetWorld()->GetMapName().Contains(TEXT("MainMenu"));
}

UClass* ABLGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (IsMenuLevel())
	{
		// a camera, not a combat vehicle - the menu draws on top of it
		return ASpectatorPawn::StaticClass();
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void ABLGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Front-end map: this global default GameMode doubles as the menu host (the
	// per-map World Settings override to ABLMenuGameMode is belt-and-suspenders;
	// this path works even when it isn't applied). No roster, no AI, no boss.
	if (IsMenuLevel())
	{
		RaiseMenu();
		if (UBLAudioSubsystem* A = GetWorld()->GetSubsystem<UBLAudioSubsystem>())
		{
			A->SetMusicState(EBLMusicState::Menu);
		}
		return;
	}

	// ring center = the player start (falls back to world origin in a bare map)
	FVector Center = FVector::ZeroVector;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		Center = It->GetActorLocation();
		break;
	}

	const int32 Override = CVarBLNumAI.GetValueOnGameThread();
	const int32 NumToSpawn = (Override >= 0) ? Override : NumAIVehicles;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// preferred: FFA spawn nodes (TargetPoints tagged "BLSpawn", per the arena
	// build briefs - start zones with no spawn-kill sightlines)
	TArray<ATargetPoint*> Nodes;
	for (TActorIterator<ATargetPoint> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(FName(TEXT("BLSpawn"))))
		{
			Nodes.Add(*It);
		}
	}

	for (int32 i = 0; i < NumToSpawn; ++i)
	{
		FVector Loc;
		FRotator Rot;
		if (Nodes.Num() > 0)
		{
			const ATargetPoint* Node = Nodes[i % Nodes.Num()];
			// wrap-around spawns offset sideways so they never stack
			const float Wrap = static_cast<float>(i / Nodes.Num()) * 450.f;
			Loc = Node->GetActorLocation()
				+ Node->GetActorRightVector() * Wrap + FVector(0, 0, SpawnHeight);
			Rot = FRotator(0.f, Node->GetActorRotation().Yaw, 0.f);
		}
		else
		{
			// fallback: even ring around the player start, facing inward
			const float Angle = 2.f * PI * static_cast<float>(i) / FMath::Max(NumToSpawn, 1);
			const FVector Offset(FMath::Cos(Angle) * SpawnRingRadius,
				FMath::Sin(Angle) * SpawnRingRadius, SpawnHeight);
			Loc = Center + Offset;
			Rot = (-Offset).GetSafeNormal2D().Rotation();
		}
		GetWorld()->SpawnActor<ABLCombatVehicle>(ABLCombatVehicle::StaticClass(), Loc, Rot, Params);
		// AutoPossessAI (PlacedInWorldOrSpawned) gives each one an ABLAIController
	}

	if (bBossFight)
	{
		GetWorldTimerManager().SetTimer(
			BossCheckTimer, this, &ABLGameMode::CheckBossRise, 2.f, true);
	}

	// the field fight bed comes up with the brawl; the Auditor opens the round
	if (UBLAudioSubsystem* A = GetWorld()->GetSubsystem<UBLAudioSubsystem>())
	{
		A->SetMusicState(EBLMusicState::Combat);
		A->PostAuditorLine();
	}
}

void ABLGameMode::CheckBossRise()
{
	if (bBossSpawned)
	{
		GetWorldTimerManager().ClearTimer(BossCheckTimer);
		return;
	}
	// the Collector arrives when the pre-boss field is cleared
	for (TActorIterator<ABLCombatVehicle> It(GetWorld()); It; ++It)
	{
		ABLCombatVehicle* V = *It;
		if (V->IsA<ABLBossPawn>() || V->IsPlayerControlled())
		{
			continue;
		}
		if (V->Health && !V->Health->IsDead())
		{
			return; // brawl still going
		}
	}

	// find the rise point (Furnace 3); no point = no boss on this map
	const ATargetPoint* RisePoint = nullptr;
	for (TActorIterator<ATargetPoint> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(FName(TEXT("BLBossRise"))))
		{
			RisePoint = *It;
			break;
		}
	}
	if (!RisePoint)
	{
		GetWorldTimerManager().ClearTimer(BossCheckTimer);
		return;
	}

	bBossSpawned = true;
	MatchState = EBLMatchState::BossFight;
	GetWorldTimerManager().ClearTimer(BossCheckTimer);
	if (UBLAudioSubsystem* A = GetWorld()->GetSubsystem<UBLAudioSubsystem>())
	{
		A->SetMusicState(EBLMusicState::BossP1);   // phases 2/3 escalate from BLBossPawn
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	const FVector Loc = RisePoint->GetActorLocation();
	if (ABLBossPawn* Boss = GetWorld()->SpawnActor<ABLBossPawn>(
		ABLBossPawn::StaticClass(), Loc + FVector(0, 0, 300.f),
		RisePoint->GetActorRotation(), Params))
	{
		Boss->RiseAt(Loc);
	}
}
