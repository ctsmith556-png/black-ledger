// Black Ledger - the Mill catwalk collapse (vertical-slice destructible #2)
// Before: elevated deck spanning the mill floor, entry ramps at both ends,
// two shootable support pillars. After: the mid-span drops out, becoming
// wreck-ramps from the floor up to the surviving end gantries (blockout:
// "shoot supports -> COLLAPSE -> wreck becomes ramp to surviving gantry").

#pragma once

#include "CoreMinimal.h"
#include "Destructibles/BLDestructibleFeature.h"
#include "BLDestructible_Catwalk.generated.h"

UCLASS()
class BLACKLEDGER_API ABLDestructible_Catwalk : public ABLDestructibleFeature
{
	GENERATED_BODY()

public:
	ABLDestructible_Catwalk();
};
