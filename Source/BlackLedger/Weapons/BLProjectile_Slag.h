// Black Ledger - Foundryman slag mortar shell: lobbed ballistic arc from the
// vessel, heavy splash on landing. Fired at a telegraphed target ring.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BLProjectile.h"
#include "BLProjectile_Slag.generated.h"

UCLASS()
class BLACKLEDGER_API ABLProjectile_Slag : public ABLProjectile
{
	GENERATED_BODY()

public:
	ABLProjectile_Slag();

	/** Ballistic launch: arrive at Target in FlightTime seconds under gravity. */
	void LaunchBallistic(const FVector& Target, float FlightTime);
};
