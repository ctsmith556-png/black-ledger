// Black Ledger - special ability base (TDD section 3)
// One subclass per character. Designers set radius/duration via properties.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BLSpecialAbility.generated.h"

class ABLCombatVehicle;

UCLASS(Abstract, Blueprintable)
class BLACKLEDGER_API UBLSpecialAbility : public UObject
{
	GENERATED_BODY()

public:
	virtual void Activate(ABLCombatVehicle* OwnerVehicle) {}

	/** Outer-chain world so abilities can spawn actors. */
	virtual UWorld* GetWorld() const override
	{
		return GetOuter() ? GetOuter()->GetWorld() : nullptr;
	}
};
