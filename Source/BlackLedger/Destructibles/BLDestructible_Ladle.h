// Black Ledger - "Tap the Ladle" (Mill Build Plan section 8 / 26).
// Shoot the hanging slag ladle until its release pins give; it dumps molten slag into
// the centre (a recurring circular hazard = weaponised centre) and drops to become a
// low cover blob for the rest of the match.

#pragma once

#include "CoreMinimal.h"
#include "Destructibles/BLDestructibleFeature.h"
#include "BLDestructible_Ladle.generated.h"

UCLASS()
class BLACKLEDGER_API ABLDestructible_Ladle : public ABLDestructibleFeature
{
	GENERATED_BODY()

public:
	ABLDestructible_Ladle();

protected:
	virtual void OnCollapsed() override;

	/** Central slag pool radius after the dump (cm). */
	UPROPERTY(EditAnywhere, Category = "BL|Ladle")
	float DumpHazardRadius = 2200.f;

	/** How often the dumped slag re-pours (cm); keeps the centre risky. */
	UPROPERTY(EditAnywhere, Category = "BL|Ladle")
	float DumpHazardCooldown = 22.f;
};
