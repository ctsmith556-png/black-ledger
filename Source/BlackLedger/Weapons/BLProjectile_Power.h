// Black Ledger - Power Missile (tuning sheet: 200 direct / 60 splash)
// Straight-line, unguided, devastating when pinned. No tracking - landing it
// is the player's problem, which is why it hits like a freight train.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BLProjectile.h"
#include "BLProjectile_Power.generated.h"

UCLASS()
class BLACKLEDGER_API ABLProjectile_Power : public ABLProjectile
{
	GENERATED_BODY()

public:
	ABLProjectile_Power();
};
