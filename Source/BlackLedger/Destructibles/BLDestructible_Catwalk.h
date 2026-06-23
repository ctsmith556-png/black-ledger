// Black Ledger - the Mill catwalk collapse (vertical-slice destructible #2)
// Before: elevated deck spanning the mill floor, entry ramps at both ends,
// two shootable support pillars. After: the mid-span drops out, becoming
// wreck-ramps from the floor up to the surviving end gantries (blockout:
// "shoot supports -> COLLAPSE -> wreck becomes ramp to surviving gantry").

#pragma once

#include "CoreMinimal.h"
#include "Destructibles/BLDestructibleFeature.h"
#include "BLDestructible_Catwalk.generated.h"

class UStaticMeshComponent;

UCLASS()
class BLACKLEDGER_API ABLDestructible_Catwalk : public ABLDestructibleFeature
{
	GENERATED_BODY()

public:
	ABLDestructible_Catwalk();

	/** When false, the catwalk drops its own floor entry ramps - used when it's a flush
	 *  section of the square loop (you reach it by driving the loop, not its own ramps). */
	UPROPERTY(EditAnywhere, Category = "BL|Catwalk")
	bool bEntryRamps = true;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> EntryRampW;
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> EntryRampE;
};
