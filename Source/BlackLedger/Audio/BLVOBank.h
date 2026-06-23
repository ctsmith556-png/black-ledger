// Black Ledger - voice-over bank (the data side of the VO casting brief in
// AUDIO_DESIGN_BRIEF.md section 5 + the opponent barks in vo/).
//
// Per-character bark sets, the Auditor's lines, and per-boss roar sets - each an
// ARRAY so the subsystem can pick a random variation and they don't repeat. The
// UBLAudioSubsystem loads this and plays whatever is assigned; empty arrays are
// simply silent, so every VO hook works long before any line is recorded.
//
// Drop-in pipeline: put wavs under vo/<Character>/<event>_N.wav and run
// tools/unreal/import_vo.py to build DA_BLVOBank. Real recordings -> no code change.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BLVOBank.generated.h"

class USoundBase;

/** One contestant's barks. Multiple lines per event = variation. */
USTRUCT(BlueprintType)
struct FBLCharacterVO
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "VO") TArray<TObjectPtr<USoundBase>> Taunt;     // spawn / engaging
	UPROPERTY(EditAnywhere, Category = "VO") TArray<TObjectPtr<USoundBase>> Hurt;      // took a heavy hit
	UPROPERTY(EditAnywhere, Category = "VO") TArray<TObjectPtr<USoundBase>> Kill;      // scored a kill
	UPROPERTY(EditAnywhere, Category = "VO") TArray<TObjectPtr<USoundBase>> LowHealth; // near death
	UPROPERTY(EditAnywhere, Category = "VO") TArray<TObjectPtr<USoundBase>> Special;   // signature special
	UPROPERTY(EditAnywhere, Category = "VO") TArray<TObjectPtr<USoundBase>> Death;     // destroyed
};

/** A Collector boss's processed roars (Foundryman = furnace roar, etc.). */
USTRUCT(BlueprintType)
struct FBLBossVO
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "VO") TArray<TObjectPtr<USoundBase>> Rise;
	UPROPERTY(EditAnywhere, Category = "VO") TArray<TObjectPtr<USoundBase>> Phase2;
	UPROPERTY(EditAnywhere, Category = "VO") TArray<TObjectPtr<USoundBase>> Phase3;
	UPROPERTY(EditAnywhere, Category = "VO") TArray<TObjectPtr<USoundBase>> Attack;
	UPROPERTY(EditAnywhere, Category = "VO") TArray<TObjectPtr<USoundBase>> Death;
};

UCLASS(BlueprintType)
class BLACKLEDGER_API UBLVOBank : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Keyed by character key ("Surgeon", "Hollow", ...) - matches BLVehicleBios. */
	UPROPERTY(EditAnywhere, Category = "VO")
	TMap<FName, FBLCharacterVO> Characters;

	/** Keyed by boss key ("Foundryman", ...). */
	UPROPERTY(EditAnywhere, Category = "VO")
	TMap<FName, FBLBossVO> Bosses;

	/** The Auditor's between-and-during-round lines (the calm is the horror). */
	UPROPERTY(EditAnywhere, Category = "VO")
	TArray<TObjectPtr<USoundBase>> AuditorLines;
};
