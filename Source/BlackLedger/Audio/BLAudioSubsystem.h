// Black Ledger - audio director (the code side of AUDIO_EVENTS.md).
//
// Centralizes every gameplay sound the way UBLImpactFXSubsystem centralizes feel:
// one place posts the one-shots, drives the engine/heartbeat loops from RTPC-style
// params, and cross-fades the music state. It plays whatever the UBLAudioBank has
// assigned and no-ops on null, so call sites are safe before any wav exists.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FX/BLImpactFXSubsystem.h"   // EBLImpactWeight
#include "BLAudioSubsystem.generated.h"

class UBLAudioBank;
class UBLVOBank;
class UAudioComponent;
class USoundBase;
class USoundAttenuation;
class APawn;

UENUM(BlueprintType)
enum class EBLMusicState : uint8
{
	Silent,
	Menu,
	Combat,
	BossP1,
	BossP2,
	BossP3,
	Victory,
	Death
};

/** Character bark events (VO casting brief). */
UENUM(BlueprintType)
enum class EBLBark : uint8
{
	Taunt,
	Hurt,
	Kill,
	LowHealth,
	Special,
	Death
};

/** Collector boss roar events. */
UENUM(BlueprintType)
enum class EBLBossVO : uint8
{
	Rise,
	Phase2,
	Phase3,
	Attack,
	Death
};

UCLASS()
class BLACKLEDGER_API UBLAudioSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// ---- one-shots ----
	void PostImpact(const FVector& Location, EBLImpactWeight Weight);
	void PostMGFire(const FVector& Location);
	void PostMissileLaunch(const FVector& Location);
	void PostVehicleDeath(const FVector& Location);
	void PostSpecial(const FVector& Location);
	void PostPickup(const FVector& Location);
	void PostUISelect();

	// ---- voice-over (UBLVOBank; random variation + a shared VO cooldown) ----
	void PostBark(FName Character, EBLBark Event, const FVector& Location);
	void PostBossVO(FName Boss, EBLBossVO Event, const FVector& Location);
	void PostAuditorLine();

	// ---- adaptive ----
	UFUNCTION(BlueprintCallable, Category = "BL|Audio")
	void SetMusicState(EBLMusicState NewState);

	/** Player-only, called each frame from the pawn: drives the engine loop pitch
	 *  (from speed) and the low-HP heartbeat (rises as health drops). */
	void NotifyPlayerSpeed(APawn* Player, float SpeedKph);
	void NotifyLowHealth(float HealthFraction);

private:
	void Play2D(USoundBase* Sound, float Volume = 1.f);
	void PlayAt(USoundBase* Sound, const FVector& Location, float Volume = 1.f);
	USoundBase* MusicFor(EBLMusicState State) const;

	/** Pick a random line from the set and play it (3D unless b2D); honors the VO
	 *  cooldown so lines don't stack. Dev-prints the label when the set is empty. */
	void PlayVOSet(const TArray<TObjectPtr<USoundBase>>& Lines, const FVector& Location,
		bool b2D, const TCHAR* DebugLabel);

	UPROPERTY()
	TObjectPtr<UBLAudioBank> Bank;

	UPROPERTY()
	TObjectPtr<UBLVOBank> VOBank;

	/** Runtime distance falloff for all positional one-shots, so far-off combat
	 *  fades out instead of playing at full volume. */
	UPROPERTY()
	TObjectPtr<USoundAttenuation> WorldAttenuation;

	UPROPERTY()
	TObjectPtr<UAudioComponent> MusicComp;

	UPROPERTY()
	TObjectPtr<UAudioComponent> EngineComp;       // idle layer

	UPROPERTY()
	TObjectPtr<UAudioComponent> EngineRevComp;    // high-RPM layer (crossfaded by speed)

	UPROPERTY()
	TObjectPtr<UAudioComponent> HeartbeatComp;

	EBLMusicState MusicState = EBLMusicState::Silent;
	bool bHeartbeatOn = false;
	double LastVOTime = -100.0;     // world seconds; VO cooldown gate
	float VOCooldown = 2.2f;        // min seconds between VO lines
};
