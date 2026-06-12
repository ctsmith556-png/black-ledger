// Black Ledger - Health Pack ("the most important pickup" - tuning sheet)
// Restores 33% of max HP on touch. Not consumed at full health.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BLPickupActor.h"
#include "BLPickup_Health.generated.h"

UCLASS()
class BLACKLEDGER_API ABLPickup_Health : public ABLPickupActor
{
	GENERATED_BODY()

public:
	ABLPickup_Health();

	UPROPERTY(EditAnywhere, Category = "BL|Pickup")
	float HealFraction = 0.33f;        // of max HP

protected:
	virtual bool GrantTo(AActor* OtherActor) override;
};
