// Black Ledger - special ability component

#include "BLSpecialComponent.h"
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

	UBLSpecialAbility* Ability = NewObject<UBLSpecialAbility>(this, AbilityClass);
	Ability->Activate(Vehicle);

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
