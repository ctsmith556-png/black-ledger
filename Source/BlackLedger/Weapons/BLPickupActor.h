// Black Ledger - weapon pickup (TDD section 3, Phase 2 piece 2)
// Sits at a spawn point; on touch grants ammo to the vehicle's WeaponComponent,
// hides, and respawns after RespawnSeconds. Health Pack variant comes later.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BLPickupActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class ABLProjectile;

UCLASS()
class BLACKLEDGER_API ABLPickupActor : public AActor
{
	GENERATED_BODY()

public:
	ABLPickupActor();

	virtual void Tick(float DeltaTime) override;

	// ---- components ----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Pickup")
	TObjectPtr<USphereComponent> TouchSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Pickup")
	TObjectPtr<UStaticMeshComponent> Visual;

	// ---- payload (Homing Missile defaults; data assets later) ----
	UPROPERTY(EditAnywhere, Category = "BL|Pickup")
	TSubclassOf<ABLProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "BL|Pickup")
	int32 Ammo = 3;

	UPROPERTY(EditAnywhere, Category = "BL|Pickup")
	FName WeaponName = TEXT("Homing Missile");

	UPROPERTY(EditAnywhere, Category = "BL|Pickup")
	float RespawnSeconds = 10.f;

	UPROPERTY(EditAnywhere, Category = "BL|Pickup")
	float SpinDegPerSec = 90.f;        // idle spin so it reads as a pickup

	/** Crate tint so pickup types read at a glance (weapons grey, health green). */
	UPROPERTY(EditAnywhere, Category = "BL|Pickup")
	FLinearColor CrateColor = FLinearColor(0.35f, 0.33f, 0.3f);

protected:
	virtual void BeginPlay() override;

	/** What touching this grants. Base: weapon ammo. Returns false to stay un-consumed. */
	virtual bool GrantTo(AActor* OtherActor);

	UFUNCTION()
	void OnTouch(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

private:
	void Respawn();
	void SetPickupActive(bool bActive);

	FTimerHandle RespawnTimer;
};
