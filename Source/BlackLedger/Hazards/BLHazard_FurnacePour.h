// Black Ledger - the Mill furnace pour (Bible 5.1)
// "Furnace pour every ~90s, 5s warning (steam + orange light)" - molten wave
// floods the pour zone around the furnace pit. F1/F2 run staggered phases.

#pragma once

#include "CoreMinimal.h"
#include "Hazards/BLHazardActor.h"
#include "BLHazard_FurnacePour.generated.h"

UCLASS()
class BLACKLEDGER_API ABLHazard_FurnacePour : public ABLHazardActor
{
	GENERATED_BODY()

public:
	ABLHazard_FurnacePour();
};
