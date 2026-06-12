// Black Ledger - Homing Missile projectile (Phase 2 piece 2)
// Tuning sheet: 80 direct / 40 splash, tracks nearest enemy in front.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BLProjectile.h"
#include "BLProjectile_Homing.generated.h"

UCLASS()
class BLACKLEDGER_API ABLProjectile_Homing : public ABLProjectile
{
	GENERATED_BODY()

public:
	ABLProjectile_Homing();

	UPROPERTY(EditAnywhere, Category = "BL|Projectile")
	float HomingAcceleration = 25000.f; // cm/s^2; turn radius = speed^2 / this (~10 m at 5000)

	/** Lock onto a target's root; pass null for a dumb-fire missile. */
	UFUNCTION(BlueprintCallable, Category = "BL|Projectile")
	void SetHomingTarget(USceneComponent* TargetComponent);
};
