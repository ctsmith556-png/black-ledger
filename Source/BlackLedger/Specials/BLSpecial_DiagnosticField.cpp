// Black Ledger - The Surgeon's Operating Table

#include "BLSpecial_DiagnosticField.h"
#include "Engine/World.h"
#include "Specials/BLDiagnosticField.h"
#include "Vehicles/BLCombatVehicle.h"

void UBLSpecial_DiagnosticField::Activate(ABLCombatVehicle* OwnerVehicle)
{
	UWorld* World = GetWorld();
	if (!World || !OwnerVehicle)
	{
		return;
	}
	FActorSpawnParameters Params;
	Params.Owner = OwnerVehicle;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ABLDiagnosticField* Field = World->SpawnActor<ABLDiagnosticField>(
		OwnerVehicle->GetActorLocation(), FRotator::ZeroRotator, Params);
	if (Field)
	{
		Field->AttachToActor(OwnerVehicle, FAttachmentTransformRules::KeepWorldTransform);
		Field->Configure(OwnerVehicle, Radius, Duration, DamageMarkMultiplier, HealPerSecond);
	}
}
