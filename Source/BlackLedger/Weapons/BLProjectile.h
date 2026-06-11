// Black Ledger - projectile base (TDD section 3, Phase 2)
// Straight machine-gun round for now; Homing/Lobbed/etc subclass later.
// On hit: applies damage to the victim's UBLHealthComponent. FX routing moves
// into UBLImpactFXSubsystem when that lands (Phase 2 piece 3).

#pragma once

#include "CoreMinimal.h"
#include "FX/BLImpactFXSubsystem.h"
#include "GameFramework/Actor.h"
#include "BLProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class BLACKLEDGER_API ABLProjectile : public AActor
{
	GENERATED_BODY()

public:
	ABLProjectile();

	// ---- components ----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Projectile")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Projectile")
	TObjectPtr<UStaticMeshComponent> TracerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Projectile")
	TObjectPtr<UProjectileMovementComponent> Movement;

	// ---- tuning (weapon component overrides on spawn; data assets later) ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BL|Projectile")
	float Damage = 6.f;

	UPROPERTY(EditAnywhere, Category = "BL|Projectile")
	float MuzzleSpeed = 9000.f;        // cm/s

	UPROPERTY(EditAnywhere, Category = "BL|Projectile")
	float LifeSeconds = 1.0f;          // ~90 m max range at MuzzleSpeed; trims tracer linger

	UPROPERTY(EditAnywhere, Category = "BL|Projectile")
	EBLImpactWeight ImpactWeight = EBLImpactWeight::Light;

	/** Knockback as velocity change (cm/s) along the flight direction - mass independent. */
	UPROPERTY(EditAnywhere, Category = "BL|Projectile")
	float HitImpulse = 25.f;

	/** Fire: aims along Dir and adds the shooter's forward speed so the
	 *  shooter can't outrun its own rounds. Call right after spawning. */
	void Launch(const FVector& Dir, const FVector& ShooterVelocity);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Damage application on impact; subclasses extend (e.g. splash). */
	virtual void ApplyImpactDamage(AActor* OtherActor, const FHitResult& Hit);
};
