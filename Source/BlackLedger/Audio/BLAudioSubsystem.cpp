// Black Ledger - audio director

#include "BLAudioSubsystem.h"
#include "Audio/BLAudioBank.h"
#include "Audio/BLVOBank.h"
#include "Components/AudioComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundBase.h"

void UBLAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// banks are optional - everything below no-ops gracefully when they're absent
	Bank = LoadObject<UBLAudioBank>(nullptr,
		TEXT("/Game/BlackLedger/Audio/DA_BLAudioBank.DA_BLAudioBank"));
	VOBank = LoadObject<UBLVOBank>(nullptr,
		TEXT("/Game/BlackLedger/Audio/DA_BLVOBank.DA_BLVOBank"));

	// distance falloff for positional one-shots (built in code - no asset needed).
	// Full volume within ~18 m, fading to silence over the next ~90 m, so a distant
	// AI brawl is faint instead of full-blast in your ears.
	WorldAttenuation = NewObject<USoundAttenuation>(this);
	FSoundAttenuationSettings& A = WorldAttenuation->Attenuation;
	A.bAttenuate = true;
	A.bSpatialize = true;
	A.AttenuationShape = EAttenuationShape::Sphere;
	A.AttenuationShapeExtents = FVector(1800.f, 0.f, 0.f);   // inner radius (full volume)
	A.FalloffDistance = 9000.f;                              // fade distance beyond that
	A.DistanceAlgorithm = EAttenuationDistanceModel::NaturalSound;
}

// ----------------------------------------------------------------------------
// play helpers
// ----------------------------------------------------------------------------
void UBLAudioSubsystem::Play2D(USoundBase* Sound, float Volume)
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(this, Sound, Volume);
	}
}

void UBLAudioSubsystem::PlayAt(USoundBase* Sound, const FVector& Location, float Volume)
{
	if (Sound && GetWorld())
	{
		// distance falloff + a little per-shot pitch variation so rapid repeats
		// (machine-gun fire/hits) don't sound mechanically identical
		const float Pitch = FMath::FRandRange(0.92f, 1.08f);
		UGameplayStatics::PlaySoundAtLocation(this, Sound, Location, FRotator::ZeroRotator,
			Volume, Pitch, 0.f, WorldAttenuation, nullptr, nullptr);
	}
}

// ----------------------------------------------------------------------------
// one-shots
// ----------------------------------------------------------------------------
void UBLAudioSubsystem::PostImpact(const FVector& Location, EBLImpactWeight Weight)
{
	if (!Bank)
	{
		return;
	}
	switch (Weight)
	{
	case EBLImpactWeight::Light:   PlayAt(Bank->MGHit, Location, 0.7f); break;
	case EBLImpactWeight::Medium:  PlayAt(Bank->MissileImpact, Location); break;
	case EBLImpactWeight::Heavy:   PlayAt(Bank->MissileImpact, Location); break;
	case EBLImpactWeight::Massive: PlayAt(Bank->PowerImpact ? Bank->PowerImpact : Bank->Explosion, Location); break;
	}
}

void UBLAudioSubsystem::PostMGFire(const FVector& Location)      { if (Bank) PlayAt(Bank->MGFire, Location, 0.6f); }
void UBLAudioSubsystem::PostMissileLaunch(const FVector& Location){ if (Bank) PlayAt(Bank->MissileLaunch, Location); }
void UBLAudioSubsystem::PostSpecial(const FVector& Location)     { if (Bank) PlayAt(Bank->SpecialActivate, Location); }
void UBLAudioSubsystem::PostPickup(const FVector& Location)      { if (Bank) PlayAt(Bank->Pickup, Location); }
void UBLAudioSubsystem::PostUISelect()                          { if (Bank) Play2D(Bank->UISelect, 0.8f); }

void UBLAudioSubsystem::PostVehicleDeath(const FVector& Location)
{
	if (!Bank)
	{
		return;
	}
	PlayAt(Bank->VehicleDeath ? Bank->VehicleDeath : Bank->Explosion, Location, 1.f);
}

// ----------------------------------------------------------------------------
// music
// ----------------------------------------------------------------------------
USoundBase* UBLAudioSubsystem::MusicFor(EBLMusicState State) const
{
	if (!Bank)
	{
		return nullptr;
	}
	switch (State)
	{
	case EBLMusicState::Menu:    return Bank->MusicMenu;
	case EBLMusicState::Combat:  return Bank->MusicCombat;
	case EBLMusicState::BossP1:  return Bank->MusicBossP1;
	case EBLMusicState::BossP2:  return Bank->MusicBossP2;
	case EBLMusicState::BossP3:  return Bank->MusicBossP3;
	case EBLMusicState::Victory: return Bank->MusicVictory;
	case EBLMusicState::Death:   return Bank->MusicDeath;
	default:                     return nullptr;
	}
}

void UBLAudioSubsystem::SetMusicState(EBLMusicState NewState)
{
	if (NewState == MusicState)
	{
		return;
	}
	MusicState = NewState;

	// fade the outgoing bed; bAutoDestroy means it tears down once silent
	if (MusicComp)
	{
		MusicComp->FadeOut(0.8f, 0.f);
		MusicComp = nullptr;
	}
	if (USoundBase* M = MusicFor(NewState))
	{
		MusicComp = UGameplayStatics::SpawnSound2D(this, M, 1.f, 1.f, 0.f, nullptr, false, /*bAutoDestroy*/ true);
		if (MusicComp)
		{
			MusicComp->FadeIn(1.0f, 1.f);
		}
	}
}

// ----------------------------------------------------------------------------
// adaptive loops (player only)
// ----------------------------------------------------------------------------
void UBLAudioSubsystem::NotifyPlayerSpeed(APawn* Player, float SpeedKph)
{
	if (!Bank || !Player || (!Bank->EngineLoop && !Bank->EngineRev))
	{
		return;
	}
	USceneComponent* Root = Player->GetRootComponent();

	// lazily spawn the two looping layers attached to the player car
	if (Bank->EngineLoop && !EngineComp)
	{
		EngineComp = UGameplayStatics::SpawnSoundAttached(
			Bank->EngineLoop, Root, NAME_None,
			FVector::ZeroVector, EAttachLocation::KeepRelativeOffset,
			/*bStopWhenAttachedToDestroyed*/ true, 1.f, 1.f, 0.f,
			nullptr, nullptr, /*bAutoDestroy*/ false);
	}
	if (Bank->EngineRev && !EngineRevComp)
	{
		EngineRevComp = UGameplayStatics::SpawnSoundAttached(
			Bank->EngineRev, Root, NAME_None,
			FVector::ZeroVector, EAttachLocation::KeepRelativeOffset,
			/*bStopWhenAttachedToDestroyed*/ true, 0.f, 1.f, 0.f,   // starts silent
			nullptr, nullptr, /*bAutoDestroy*/ false);
	}

	// rev across the chassis's real speed band (Surgeon tops ~95 kph)
	const float Frac = FMath::Clamp(SpeedKph / 110.f, 0.f, 1.f);

	if (EngineRevComp)
	{
		// two-layer engine: idle dominant at rest, the rev layer rises and takes
		// over under throttle. Each layer also pitches up a little within its band.
		if (EngineComp)
		{
			EngineComp->SetVolumeMultiplier(1.f - 0.85f * Frac);
			EngineComp->SetPitchMultiplier(0.92f + 0.30f * Frac);
		}
		EngineRevComp->SetVolumeMultiplier(FMath::Clamp(1.15f * Frac, 0.f, 1.f));
		EngineRevComp->SetPitchMultiplier(0.90f + 0.55f * Frac);
	}
	else if (EngineComp)
	{
		// no rev clip yet: pitch-shift the single idle for a basic rev (today's behavior)
		EngineComp->SetVolumeMultiplier(0.6f + 0.4f * Frac);
		EngineComp->SetPitchMultiplier(0.9f + 0.5f * Frac);
	}
}

void UBLAudioSubsystem::NotifyLowHealth(float HealthFraction)
{
	if (!Bank || !Bank->Heartbeat)
	{
		return;
	}
	const bool bShould = (HealthFraction > 0.f && HealthFraction < 0.30f);
	if (bShould && !bHeartbeatOn)
	{
		HeartbeatComp = UGameplayStatics::SpawnSound2D(
			this, Bank->Heartbeat, 1.f, 1.f, 0.f, nullptr, false, /*bAutoDestroy*/ false);
		bHeartbeatOn = (HeartbeatComp != nullptr);
	}
	else if (!bShould && bHeartbeatOn)
	{
		if (HeartbeatComp) { HeartbeatComp->FadeOut(0.4f, 0.f); HeartbeatComp = nullptr; }
		bHeartbeatOn = false;
	}
	if (bHeartbeatOn && HeartbeatComp)
	{
		// faster + louder the closer to death (0.30 -> 0.0 maps 1.0x -> 1.8x pitch)
		const float T = FMath::Clamp(1.f - HealthFraction / 0.30f, 0.f, 1.f);
		HeartbeatComp->SetPitchMultiplier(1.0f + 0.8f * T);
		HeartbeatComp->SetVolumeMultiplier(0.6f + 0.4f * T);
	}
}

// ----------------------------------------------------------------------------
// voice-over
// ----------------------------------------------------------------------------
void UBLAudioSubsystem::PlayVOSet(const TArray<TObjectPtr<USoundBase>>& Lines,
	const FVector& Location, bool b2D, const TCHAR* DebugLabel)
{
	const double Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	if (Now - LastVOTime < VOCooldown)
	{
		return;   // don't let VO lines stack on top of each other
	}
	if (Lines.Num() == 0)
	{
#if !(UE_BUILD_SHIPPING)
		// the hook is wired - the line just hasn't been recorded/assigned yet
		if (GEngine && DebugLabel)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Silver,
				FString::Printf(TEXT("[VO] %s (no line assigned)"), DebugLabel));
		}
#endif
		return;
	}
	USoundBase* Line = Lines[FMath::RandRange(0, Lines.Num() - 1)];
	if (!Line)
	{
		return;
	}
	LastVOTime = Now;
	if (b2D) { Play2D(Line, 1.f); }
	else     { PlayAt(Line, Location, 1.f); }
}

void UBLAudioSubsystem::PostBark(FName Character, EBLBark Event, const FVector& Location)
{
	static const TArray<TObjectPtr<USoundBase>> Empty;
	const FBLCharacterVO* VO = VOBank ? VOBank->Characters.Find(Character) : nullptr;
	const TArray<TObjectPtr<USoundBase>>* Set = &Empty;
	const TCHAR* Label = TEXT("Bark");
	if (VO)
	{
		switch (Event)
		{
		case EBLBark::Taunt:     Set = &VO->Taunt;     Label = TEXT("Taunt"); break;
		case EBLBark::Hurt:      Set = &VO->Hurt;      Label = TEXT("Hurt"); break;
		case EBLBark::Kill:      Set = &VO->Kill;      Label = TEXT("Kill"); break;
		case EBLBark::LowHealth: Set = &VO->LowHealth; Label = TEXT("LowHealth"); break;
		case EBLBark::Special:   Set = &VO->Special;   Label = TEXT("Special"); break;
		case EBLBark::Death:     Set = &VO->Death;     Label = TEXT("Death"); break;
		}
	}
	PlayVOSet(*Set, Location, /*b2D*/ false, Label);
}

void UBLAudioSubsystem::PostBossVO(FName Boss, EBLBossVO Event, const FVector& Location)
{
	static const TArray<TObjectPtr<USoundBase>> Empty;
	const FBLBossVO* VO = VOBank ? VOBank->Bosses.Find(Boss) : nullptr;
	const TArray<TObjectPtr<USoundBase>>* Set = &Empty;
	const TCHAR* Label = TEXT("BossVO");
	if (VO)
	{
		switch (Event)
		{
		case EBLBossVO::Rise:   Set = &VO->Rise;   Label = TEXT("Boss/Rise"); break;
		case EBLBossVO::Phase2: Set = &VO->Phase2; Label = TEXT("Boss/Phase2"); break;
		case EBLBossVO::Phase3: Set = &VO->Phase3; Label = TEXT("Boss/Phase3"); break;
		case EBLBossVO::Attack: Set = &VO->Attack; Label = TEXT("Boss/Attack"); break;
		case EBLBossVO::Death:  Set = &VO->Death;  Label = TEXT("Boss/Death"); break;
		}
	}
	PlayVOSet(*Set, Location, /*b2D*/ false, Label);
}

void UBLAudioSubsystem::PostAuditorLine()
{
	static const TArray<TObjectPtr<USoundBase>> Empty;
	const TArray<TObjectPtr<USoundBase>>* Set = VOBank ? &VOBank->AuditorLines : &Empty;
	PlayVOSet(*Set, FVector::ZeroVector, /*b2D*/ true, TEXT("Auditor"));   // omnipresent voice
}
