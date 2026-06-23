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

	virtual void Tick(float DeltaTime) override;

	// ---- attack tuning (telegraph lengths NEVER scale with difficulty) ----
	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float MortarInterval = 6.5f;       // s, phase 1-2 (phase 3: x0.7); difficulty scales this

	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float MortarTelegraph = 1.5f;      // ring warning before the shell lands - hard but FAIR

	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float MortarLeadFactor = 0.55f;    // under-lead: holding speed beats the shell; turning beats it too

	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float MortarScatter = 250.f;       // cm of aim slop - he's pouring slag, not sniping

	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float WallInterval = 11.f;         // s, phase 2+ (phase 3: x0.7)

	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float WallLength = 3000.f;         // cm

	/** Phase 2/3 trigger fractions of max HP (sheet: 66% / 33%). */
	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float Phase2Fraction = 0.66f;

	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float Phase3Fraction = 0.33f;

	// ---- Phase 3 "Tap the Heat": the slam + weak-point window (Bible 4.6) ----
	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float SlamInterval = 7.0f;         // s between slams (difficulty scales this)

	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float SlamTelegraph = 1.3f;        // cage glows white this long before the slam - the TELL

	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float SlamRange = 3500.f;          // cm: only slams when the player is close enough to bait

	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float SlamRadius = 1400.f;         // cm shockwave

	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float SlamDamage = 240.f;          // center damage (falls off with distance)

	/** After a slam the core is exposed for this long - bait the slam, hit the core. */
	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float ExposeSeconds = 3.0f;

	/** Damage taken to the exposed core is multiplied by this (the window pays off). */
	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float ExposedDamageMultiplier = 3.0f;

	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float OverheatSpeedKph = 78.f;     // phase 3 speeds up from the base 55

	/** Tread cleats scroll at forward-speed * this (sign flips direction). */
	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float TreadScrollScale = 0.004f;

	/** Procedural cube tread strips OFF - a scrolling box always reads as a comb, never a
	 *  real track. Realistic moving treads need the modeled tread faces split into their own
	 *  material slot (Blender) so a UV-pan material can scroll the actual geometry. */
	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	bool bShowTreadOverlays = false;

	/** Tread overlays auto-fit to the body bounds at BeginPlay; these nudge the seat.
	 *  InsetY pulls the strips inward from the widest point; DropZ raises/lowers them. */
	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float TreadInsetY = 35.f;

	UPROPERTY(EditAnywhere, Category = "BL|Boss")
	float TreadDropZ = 0.f;

	UPROPERTY(BlueprintAssignable, Category = "BL|Boss")
	FBLOnBossPhaseChanged OnPhaseChanged;

	UFUNCTION(BlueprintPure, Category = "BL|Boss")
	int32 GetPhase() const { return Phase; }

	/** HUD reads this to call out the strike window. */
	UFUNCTION(BlueprintPure, Category = "BL|Boss")
	bool IsCoreExposed() const { return bCoreExposed; }

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
	void UpdateAttacks(float DeltaTime);
	APawn* FindPlayerTarget() const;
	void BeginMortar(APawn* Target);
	void LaunchMortar();
	void SpawnMoltenWall(APawn* Target);
	float DifficultyIntervalScale() const;

	// phase 3 slam + weak-point window
	void BeginSlam();
	void DoSlam();
	void ExposeCore();
	void HideCore();
	void DropFireTrail();
	void UpdateCoreGlow(float DeltaTime);
	void UpdateTreads(float DeltaTime);   // scroll the tread cleats with speed

	// the glowing core inside the cage - the weak point + the white-hot tell
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> Core;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CoreMID;

	// tread belts down each side (the wheels are hidden); cleats scroll with speed
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> TreadL;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> TreadR;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> TreadLMID;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> TreadRMID;

	float TreadScroll = 0.f;

	int32 Phase = 1;
	float MortarClock = 0.f;
	float WallClock = 0.f;
	float SlamClock = 0.f;
	bool bOverheated = false;
	bool bSlamTelegraph = false;   // cage glowing white, slam incoming
	float SlamTelegraphT = 0.f;    // 0..1 ramp during the tell
	bool bCoreExposed = false;
	FVector PendingMortarTarget = FVector::ZeroVector;
	FTimerHandle MortarLaunchTimer;
	FTimerHandle SlamHitTimer;
	FTimerHandle ExposeTimer;
	FTimerHandle FireTrailTimer;
};
