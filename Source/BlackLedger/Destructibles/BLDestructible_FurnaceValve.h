// Black Ledger - "Blow the Furnace" (Mill Build Plan section 8 / 26).
// Glowing pressure valves on a furnace. Destroy them and the furnace detonates once -
// killing anything too close - and the blast opens the sealed loading bay (any actor
// tagged SealedDoorTag is removed), adding a new room/route + the rare reward inside.

#pragma once

#include "CoreMinimal.h"
#include "Destructibles/BLDestructibleFeature.h"
#include "BLDestructible_FurnaceValve.generated.h"

UCLASS()
class BLACKLEDGER_API ABLDestructible_FurnaceValve : public ABLDestructibleFeature
{
	GENERATED_BODY()

public:
	ABLDestructible_FurnaceValve();

protected:
	virtual void OnCollapsed() override;

	/** Level geometry with this tag is opened (hidden + collision off) on breach. */
	UPROPERTY(EditAnywhere, Category = "BL|Furnace")
	FName SealedDoorTag = TEXT("BLSealedDoor");

	UPROPERTY(EditAnywhere, Category = "BL|Furnace")
	float BlastRadius = 1400.f;

	/** One-shot detonation damage - lethal to anything caught point-blank. */
	UPROPERTY(EditAnywhere, Category = "BL|Furnace")
	float BlastDamage = 9000.f;
};
