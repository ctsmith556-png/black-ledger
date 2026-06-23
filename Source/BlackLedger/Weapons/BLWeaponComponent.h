// Black Ledger - weapon component (TDD section 2/3, Phase 2)
// Phase 2 piece 1: machine-gun primary (unlimited ammo). The pickup slot
// (Homing Missile + ammo) is piece 2 and slots in beside this.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BLWeaponComponent.generated.h"

class ABLProjectile;
class UPointLightComponent;

/** One held pickup weapon. The full 14-weapon pool stacks through these. */
USTRUCT(BlueprintType)
struct FBLWeaponSlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "BL|Weapon")
	TSubclassOf<ABLProjectile> ProjectileClass;

	UPROPERTY(BlueprintReadOnly, Category = "BL|Weapon")
	int32 Ammo = 0;

	UPROPERTY(BlueprintReadOnly, Category = "BL|Weapon")
	FName Name;
};

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
	float MuzzleFlashIntensity = 20000.f;   // reads as a strobe in the dark foundry

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

	/** Called by ABLPickupActor. Same weapon stacks ammo; new weapons add a slot. */
	UFUNCTION(BlueprintCallable, Category = "BL|Weapon")
	void GrantPickup(TSubclassOf<ABLProjectile> ProjectileClass, int32 Ammo, FName WeaponName);

	/** Fires the SELECTED slot. */
	UFUNCTION(BlueprintCallable, Category = "BL|Weapon")
	void FirePickup();

	/** TM-style weapon cycling: +1 next / -1 previous. */
	UFUNCTION(BlueprintCallable, Category = "BL|Weapon")
	void CycleWeapon(int32 Direction);

	UFUNCTION(BlueprintPure, Category = "BL|Weapon")
	int32 GetPickupAmmo() const
	{
		return Inventory.IsValidIndex(SelectedIndex) ? Inventory[SelectedIndex].Ammo : 0;
	}

	UFUNCTION(BlueprintPure, Category = "BL|Weapon")
	FName GetPickupName() const
	{
		return Inventory.IsValidIndex(SelectedIndex) ? Inventory[SelectedIndex].Name : NAME_None;
	}

	const TArray<FBLWeaponSlot>& GetInventory() const { return Inventory; }
	int32 GetSelectedIndex() const { return SelectedIndex; }

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

	void BroadcastSelected();

	UPROPERTY()
	TArray<FBLWeaponSlot> Inventory;
	int32 SelectedIndex = INDEX_NONE;
	double LastPickupShotTime = -1.0;
};
