// Black Ledger - The Surgeon's "Operating Table" (tuning sheet: diagnostic
// field, +100% damage vs marked enemies, self-heal). Spawns a field that
// follows the ambulance; enemies inside are "diagnosed."

#pragma once

#include "CoreMinimal.h"
#include "Specials/BLSpecialAbility.h"
#include "BLSpecial_DiagnosticField.generated.h"

UCLASS()
class BLACKLEDGER_API UBLSpecial_DiagnosticField : public UBLSpecialAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "BL|Special")
	float Radius = 1500.f;             // cm

	UPROPERTY(EditAnywhere, Category = "BL|Special")
	float Duration = 6.f;

	UPROPERTY(EditAnywhere, Category = "BL|Special")
	float DamageMarkMultiplier = 2.f;  // sheet: +100% vs marked

	UPROPERTY(EditAnywhere, Category = "BL|Special")
	float HealPerSecond = 8.f;         // while at least one enemy is diagnosed

	virtual void Activate(ABLCombatVehicle* OwnerVehicle) override;
};
