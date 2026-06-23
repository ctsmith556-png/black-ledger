// Black Ledger - boss combat driver: the standard AI driver minus the
// civilian habits. A Collector never peels, never wanders off mid-fight,
// never detours for pickups - it hunts. Bespoke Behavior Trees replace this
// when bosses go data-driven.

#pragma once

#include "CoreMinimal.h"
#include "AI/BLAIController.h"
#include "BLBossAIController.generated.h"

UCLASS()
class BLACKLEDGER_API ABLBossAIController : public ABLAIController
{
	GENERATED_BODY()

public:
	ABLBossAIController();
};
