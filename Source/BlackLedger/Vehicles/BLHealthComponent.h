// Black Ledger - health + armor (TDD section 2)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BLHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBLOnDamaged, float, Amount, float, HealthRemaining);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBLOnDeath);

UCLASS(ClassGroup = (BlackLedger), meta = (BlueprintSpawnableComponent))
class BLACKLEDGER_API UBLHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBLHealthComponent();

	/** Tuning-sheet formula: HP = BaseHP * (0.7 + (Armor-1) * 0.0889). Sheet base = 1000. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BL|Health")
	float BaseHP = 1000.f;

	/** 1-10 stat from the character sheet (Surgeon = 7 -> 1233 eff HP, per BALANCE_SIM). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BL|Health", meta = (ClampMin = 1, ClampMax = 10))
	float ArmorStat = 7.f;

	UPROPERTY(BlueprintAssignable, Category = "BL|Health")
	FBLOnDamaged OnDamaged;

	UPROPERTY(BlueprintAssignable, Category = "BL|Health")
	FBLOnDeath OnDeath;

	UFUNCTION(BlueprintCallable, Category = "BL|Health")
	void ApplyDamage(float Amount);

	UFUNCTION(BlueprintCallable, Category = "BL|Health")
	void Heal(float Amount);

	/** Difficulty HP scaling (Bible 4.6: scale HP, never remove tells). Keeps current HP fraction. */
	UFUNCTION(BlueprintCallable, Category = "BL|Health")
	void ScaleMaxHealth(float Scale);

	UFUNCTION(BlueprintPure, Category = "BL|Health")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "BL|Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "BL|Health")
	bool IsDead() const { return Health <= 0.f; }

protected:
	virtual void BeginPlay() override;

private:
	float MaxHealth = 100.f;
	float Health = 100.f;
};
