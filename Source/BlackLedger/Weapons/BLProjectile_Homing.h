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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BL|Projectile")
	float SplashDamage = 40.f;

	UPROPERTY(EditAnywhere, Category = "BL|Projectile")
	float SplashRadius = 600.f;        // cm

	UPROPERTY(EditAnywhere, Category = "BL|Projectile")
	float HomingAcceleration = 25000.f; // cm/s^2; turn radius = speed^2 / this (~10 m at 5000)

	/** Explosion knockback as velocity change (cm/s) at the blast center, falling off with range. */
	UPROPERTY(EditAnywhere, Category = "BL|Projectile")
	float SplashImpulse = 280.f;

	/** Lock onto a target's root; pass null for a dumb-fire missile. */
	UFUNCTION(BlueprintCallable, Category = "BL|Projectile")
	void SetHomingTarget(USceneComponent* TargetComponent);

protected:
	virtual void ApplyImpactDamage(AActor* OtherActor, const FHitResult& Hit) override;
};
