// Black Ledger - centralized combat feel

#include "BLImpactFXSubsystem.h"
#include "Audio/BLAudioSubsystem.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "FX/BLCameraShake_Impact.h"
#include "FX/BLImpactBurst.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

namespace
{
	struct FBLImpactProfile
	{
		float ShakeScale;
		float Dilation;     // 1 = no hit-stop
		float RealSeconds;  // hit-stop length in real time
	};
	// Light, Medium, Heavy, Massive (TDD 5: 50-100 ms, scaled by weapon weight)
	constexpr FBLImpactProfile Profiles[4] = {
		{ 0.35f, 1.f,   0.f   },
		{ 0.8f,  0.45f, 0.05f },
		{ 1.5f,  0.25f, 0.09f },
		{ 2.4f,  0.15f, 0.12f },
	};
}

void UBLImpactFXSubsystem::PlayImpact(const FVector& Location, EBLImpactWeight Weight)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	const FBLImpactProfile& P = Profiles[static_cast<int32>(Weight)];

	if (P.Dilation < 1.f)
	{
		ApplyTimeDip(P.Dilation, P.RealSeconds);
	}

	if (APlayerController* PC = World->GetFirstPlayerController())
	{
		if (PC->PlayerCameraManager)
		{
			const float Dist = FVector::Dist(PC->PlayerCameraManager->GetCameraLocation(), Location);
			const float Falloff = 3000.f / (3000.f + Dist); // full strength close, fades with range
			PC->PlayerCameraManager->StartCameraShake(
				UBLCameraShake_Impact::StaticClass(), P.ShakeScale * Falloff);
		}
	}

	// placeholder burst (flash + fireball); authored Niagara impacts replace this
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ABLImpactBurst* Burst = World->SpawnActor<ABLImpactBurst>(Location, FRotator::ZeroRotator, Params))
	{
		Burst->Configure(Weight);
	}

	// 3-layer impact audio (AUDIO_EVENTS.md) via the audio director; the matched
	// micro audio-gap is implicit in the hit-stop above. Real particles/decals = TODO.
	if (UBLAudioSubsystem* Audio = World->GetSubsystem<UBLAudioSubsystem>())
	{
		Audio->PostImpact(Location, Weight);
	}
}

void UBLImpactFXSubsystem::PlayDeathMoment()
{
	ApplyTimeDip(0.3f, 1.0f);
	if (APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
	{
		if (PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->StartCameraShake(UBLCameraShake_Impact::StaticClass(), 2.f);
		}
	}
}

void UBLImpactFXSubsystem::ApplyTimeDip(float Dilation, float RealSeconds)
{
	// never let a weak dip soften or cut short a strong one already running
	if (Dilation >= ActiveDilation && ActiveDilation < 1.f)
	{
		return;
	}
	ActiveDilation = Dilation;
	UGameplayStatics::SetGlobalTimeDilation(this, Dilation);
	// timers tick on dilated time: scale the duration so the dip lasts RealSeconds of wall time
	GetWorld()->GetTimerManager().SetTimer(
		TimeDipTimer, this, &UBLImpactFXSubsystem::RestoreTime,
		FMath::Max(RealSeconds * Dilation, 0.001f), false);
}

void UBLImpactFXSubsystem::RestoreTime()
{
	ActiveDilation = 1.f;
	UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
}
