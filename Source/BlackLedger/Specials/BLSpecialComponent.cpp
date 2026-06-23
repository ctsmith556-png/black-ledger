// Black Ledger - special ability component

#include "BLSpecialComponent.h"
#include "Audio/BLAudioSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Specials/BLSpecialAbility.h"
#include "Specials/BLSpecial_DiagnosticField.h"
#include "TimerManager.h"
#include "Vehicles/BLCombatVehicle.h"

UBLSpecialComponent::UBLSpecialComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	AbilityClass = UBLSpecial_DiagnosticField::StaticClass(); // Surgeon default; data assets later
}

bool UBLSpecialComponent::TryActivate()
{
	ABLCombatVehicle* Vehicle = Cast<ABLCombatVehicle>(GetOwner());
	if (!bReady || !AbilityClass || !Vehicle)
	{
		return false;
	}

	// TEMP: every vehicle currently defaults to the Surgeon's Diagnostic Field, so AI
	// popping it spawns teal fields all over the arena ("green circles over the AI").
	// Until specials are data-driven per character, only the player fires its special.
	if (!Vehicle->IsPlayerControlled())
	{
		return false;
	}

	UBLSpecialAbility* Ability = NewObject<UBLSpecialAbility>(this, AbilityClass);
	Ability->Activate(Vehicle);

	if (UBLAudioSubsystem* Audio = GetWorld()->GetSubsystem<UBLAudioSubsystem>())
	{
		Audio->PostSpecial(Vehicle->GetActorLocation());
		if (Vehicle->IsPlayerControlled())
		{
			Audio->PostBark(Vehicle->CharacterKey, EBLBark::Special, Vehicle->GetActorLocation());
		}
	}

	bReady = false;
	OnReadyChanged.Broadcast(false);
	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimer, this, &UBLSpecialComponent::OnCooldownDone, CooldownSeconds, false);
	return true;
}

void UBLSpecialComponent::OnCooldownDone()
{
	bReady = true;
	OnReadyChanged.Broadcast(true);

#if !(UE_BUILD_SHIPPING)
	// placeholder HUD: only shout for the local player
	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (GEngine && Pawn && Pawn->IsPlayerControlled())
	{
		GEngine->AddOnScreenDebugMessage(11, 2.5f, FColor::Cyan, TEXT("SPECIAL READY"));
	}
#endif
}
