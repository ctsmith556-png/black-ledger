// Black Ledger - Collector boss pawn base (TDD section 7, Bible 4.6)
// Shares the full ABLCombatVehicle stack (suspension, health, weapon, special)
// and adds the phase state machine: Phase 1/2/3 at the tuning sheet's HP
// thresholds, each phase enabling a different attack set, every heavy attack
// telegraph -> window so it's hard-but-fair. Defaults = The Foundryman "Tap"
// (UBLBossDataAsset takes over when bosses go data-driven).

#pragma once

#include "CoreMinimal.h"
#include "Vehicles/BLCombatVehicle.h"
#include "BLBossPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBLOnBossPhaseChanged, int32, NewPhase);

UCLASS()
class BLACKLEDGER_API ABLBossPawn : public ABLCombatVehicle
{
	GENERATED_BODY()

public:
	ABLBossPawn();

	/** Phase 2/3 trigger fractions of max HP (sheet: 66% / 33%). */
	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float Phase2Fraction = 0.66f;

	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float Phase3Fraction = 0.33f;

	UPROPERTY(BlueprintAssignable, Category = "BL|Boss")
	FBLOnBossPhaseChanged OnPhaseChanged;

	UFUNCTION(BlueprintPure, Category = "BL|Boss")
	int32 GetPhase() const { return Phase; }

	/** The arrival: teleport to the rise point and announce (slice: Furnace 3).
	 *  Cinematic rise + furnace commandeering hooks live here. */
	UFUNCTION(BlueprintCallable, Category = "BL|Boss")
	void RiseAt(const FVector& Location);

protected:
	virtual void BeginPlay() override;

	/** Phase hooks - subclasses/Foundryman attacks land here next. */
	virtual void EnterPhase(int32 NewPhase);

	UFUNCTION()
	void OnBossDamaged(float Amount, float HealthRemaining);

	UFUNCTION()
	void OnBossDeath();

private:
	int32 Phase = 1;
};
