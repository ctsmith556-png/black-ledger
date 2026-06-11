// Black Ledger - game instance

#include "BLGameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"

// dev/test hook until the options menu exists: bl.SetDifficulty 0|1|2
// (behavior applies immediately; AI HP scale applies to newly possessed vehicles)
static FAutoConsoleCommandWithWorldAndArgs GBLSetDifficultyCmd(
	TEXT("bl.SetDifficulty"),
	TEXT("Set difficulty: 0=Easy 1=Medium 2=Hard"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (!World || Args.Num() < 1)
			{
				return;
			}
			if (UBLGameInstance* GI = World->GetGameInstance<UBLGameInstance>())
			{
				const int32 Value = FMath::Clamp(FCString::Atoi(*Args[0]), 0, 2);
				GI->SetDifficulty(static_cast<EBLDifficulty>(Value));
				if (GEngine)
				{
					static const TCHAR* Names[3] = { TEXT("Easy"), TEXT("Medium"), TEXT("Hard") };
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
						FString::Printf(TEXT("Difficulty -> %s"), Names[Value]));
				}
			}
		}));
