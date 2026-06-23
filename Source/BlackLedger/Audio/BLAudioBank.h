// Black Ledger - audio bank (the data side of AUDIO_EVENTS.md).
//
// A single DataAsset holding the USoundBase for every game event the code posts.
// The UBLAudioSubsystem loads it and plays whatever is assigned; any slot left
// null is simply silent, so the wiring works long before real sound design lands.
// tools/unreal/import_audio.py builds DA_BLAudioBank from placeholder tones.
//
// When middleware (Wwise/FMOD) arrives this asset is the swap point: repoint the
// slots (or replace the subsystem's Play helpers) without touching call sites.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BLAudioBank.generated.h"

class USoundBase;

UCLASS(BlueprintType)
class BLACKLEDGER_API UBLAudioBank : public UDataAsset
{
	GENERATED_BODY()

public:
	// ---- one-shots (3-layer blend containers later; one wav each for now) ----
	UPROPERTY(EditAnywhere, Category = "Weapon")   TObjectPtr<USoundBase> MGFire;
	UPROPERTY(EditAnywhere, Category = "Weapon")   TObjectPtr<USoundBase> MGHit;
	UPROPERTY(EditAnywhere, Category = "Weapon")   TObjectPtr<USoundBase> MissileLaunch;
	UPROPERTY(EditAnywhere, Category = "Weapon")   TObjectPtr<USoundBase> MissileImpact;
	UPROPERTY(EditAnywhere, Category = "Weapon")   TObjectPtr<USoundBase> PowerImpact;
	UPROPERTY(EditAnywhere, Category = "Weapon")   TObjectPtr<USoundBase> Explosion;
	UPROPERTY(EditAnywhere, Category = "Vehicle")  TObjectPtr<USoundBase> VehicleDeath;
	UPROPERTY(EditAnywhere, Category = "Special")  TObjectPtr<USoundBase> SpecialActivate;
	UPROPERTY(EditAnywhere, Category = "Pickup")   TObjectPtr<USoundBase> Pickup;
	UPROPERTY(EditAnywhere, Category = "UI")       TObjectPtr<USoundBase> UISelect;

	// ---- loops (driven by RTPC-style params on the subsystem) ----
	UPROPERTY(EditAnywhere, Category = "Vehicle")  TObjectPtr<USoundBase> EngineLoop;   // idle bed
	UPROPERTY(EditAnywhere, Category = "Vehicle")  TObjectPtr<USoundBase> EngineRev;    // high-RPM layer (crossfaded in by speed; optional)
	UPROPERTY(EditAnywhere, Category = "Vehicle")  TObjectPtr<USoundBase> Heartbeat;

	// ---- music states (AUDIO_EVENTS.md MusicState) ----
	UPROPERTY(EditAnywhere, Category = "Music")    TObjectPtr<USoundBase> MusicMenu;
	UPROPERTY(EditAnywhere, Category = "Music")    TObjectPtr<USoundBase> MusicCombat;
	UPROPERTY(EditAnywhere, Category = "Music")    TObjectPtr<USoundBase> MusicBossP1;
	UPROPERTY(EditAnywhere, Category = "Music")    TObjectPtr<USoundBase> MusicBossP2;
	UPROPERTY(EditAnywhere, Category = "Music")    TObjectPtr<USoundBase> MusicBossP3;
	UPROPERTY(EditAnywhere, Category = "Music")    TObjectPtr<USoundBase> MusicVictory;
	UPROPERTY(EditAnywhere, Category = "Music")    TObjectPtr<USoundBase> MusicDeath;
};
