// Black Ledger - weapon component (TDD section 2/3, Phase 2)
// Phase 2 piece 1: machine-gun primary (unlimited ammo). The pickup slot
// (Homing Missile + ammo) is piece 2 and slots in beside this.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BLWeaponComponent.generated.h"

class ABLProjectile;
class UPointLightComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBLOnPickupChanged, FName, WeaponName, int32, Ammo);

UCLASS(ClassGroup = (BlackLedger), meta = (BlueprintSpawnableComponent))
class BLACKLEDGER_API UBLWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBLWeaponComponent();

	// ---- primary (machine gun) tuning ----
	/** Spawned per shot; defaults to ABLProjectile, override in BP for tracer/FX variants. */
	UPROPERTY(EditAnywhere, Category = "BL|Weapon")
	TSubclassOf<ABLProjectile> PrimaryProjectileClass;

	/** 6 dmg x 10 rds/s = 60 DPS, the BALANCE_SIM primary-only profile. */
	UPROPERTY(EditAnywhere, Category = "BL|Weapon")
	float PrimaryDamage = 6.f;

	UPROPERTY(EditAnywhere, Category = "BL|Weapon")
	float PrimaryFireRate = 10.f;      // rounds per second

	UPROPERTY(EditAnywhere, Category = "BL|Weapon")
	float PrimarySpreadDeg = 1.2f;     // random cone half-angle

	// ---- muzzle flash (placeholder point light; Niagara flash later) ----
	UPROPERTY(EditAnywhere, Category = "BL|Weapon")
	float MuzzleFlashIntensity = 9000.f;

	UPROPERTY(EditAnywhere, Category = "BL|Weapon")
	float MuzzleFlashSeconds = 0.05f;

	/** Muzzles in owner space; shots alternate between them (twin guns). */
	UPROPERTY(EditAnywhere, Category = "BL|Weapon")
	TArray<FVector> MuzzleOffsets = { FVector(380.f, 60.f, -15.f), FVector(380.f, -60.f, -15.f) };

	// ---- firing ----
	UFUNCTION(BlueprintCallable, Category = "BL|Weapon")
	void StartFirePrimary();

	UFUNCTION(BlueprintCallable, Category = "BL|Weapon")
	void StopFirePrimary();

	UFUNCTION(BlueprintPure, Category = "BL|Weapon")
	bool IsFiringPrimary() const { return bFiringPrimary; }

	// ---- pickup slot (one held weapon + ammo; UBLWeaponDataAsset later) ----
	/** Center muzzle for missiles/launched pickups, owner space. */
	UPROPERTY(EditAnywhere, Category = "BL|Weapon")
	FVector PickupMuzzleOffset = FVector(380.f, 0.f, 40.f);

	UPROPERTY(EditAnywhere, Category = "BL|Weapon")
	float PickupFireCooldown = 0.5f;   // s between pickup shots

	/** Homing lock: max range + half-angle of the front acquisition cone. */
	UPROPERTY(EditAnywhere, Category = "BL|Weapon")
	float HomingRange = 20000.f;       // cm

	UPROPERTY(EditAnywhere, Category = "BL|Weapon")
	float HomingConeHalfAngleDeg = 60.f;

	UPROPERTY(BlueprintAssignable, Category = "BL|Weapon")
	FBLOnPickupChanged OnPickupChanged;

	/** Called by ABLPickupActor. Same weapon stacks ammo; a new one replaces. */
	UFUNCTION(BlueprintCallable, Category = "BL|Weapon")
	void GrantPickup(TSubclassOf<ABLProjectile> ProjectileClass, int32 Ammo, FName WeaponName);

	UFUNCTION(BlueprintCallable, Category = "BL|Weapon")
	void FirePickup();

	UFUNCTION(BlueprintPure, Category = "BL|Weapon")
	int32 GetPickupAmmo() const { return PickupAmmo; }

	UFUNCTION(BlueprintPure, Category = "BL|Weapon")
	FName GetPickupName() const { return PickupName; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void FirePrimaryShot();
	ABLProjectile* SpawnProjectile(TSubclassOf<ABLProjectile> Class, const FVector& MuzzleLocal, const FVector& Dir);
	USceneComponent* FindHomingTarget() const;
	void FlashMuzzle(const FVector& MuzzleLocal);
	void EndMuzzleFlash();

	UPROPERTY()
	TObjectPtr<UPointLightComponent> MuzzleLight;
	FTimerHandle MuzzleFlashTimer;

	FTimerHandle PrimaryTimer;
	bool bFiringPrimary = false;
	double LastPrimaryShotTime = -1.0; // world seconds; keeps tap-fire inside the fire rate
	int32 MuzzleIndex = 0;

	UPROPERTY()
	TSubclassOf<ABLProjectile> PickupProjectileClass;
	int32 PickupAmmo = 0;
	FName PickupName = NAME_None;
	double LastPickupShotTime = -1.0;
};
