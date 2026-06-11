// Black Ledger - timed environmental hazard base (TDD section 6, Bible section 5)
// Cycle: idle (cooldown) -> TELEGRAPH (warning FX, hard-but-fair) -> ACTIVE
// (damage window) -> idle. One subclass per hazard; bosses can command these.
// Difficulty never shortens the telegraph (Bible 4.6).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BLHazardActor.generated.h"

class UPointLightComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;

UENUM(BlueprintType)
enum class EBLHazardPhase : uint8
{
	Idle,
	Telegraph,
	Active
};

UCLASS()
class BLACKLEDGER_API ABLHazardActor : public AActor
{
	GENERATED_BODY()

public:
	ABLHazardActor();

	virtual void Tick(float DeltaTime) override;

	// ---- cycle tuning ----
	UPROPERTY(EditAnywhere, Category = "BL|Hazard")
	float CooldownSeconds = 90.f;     // idle time between pours (Bible: ~90s)

	UPROPERTY(EditAnywhere, Category = "BL|Hazard")
	float TelegraphSeconds = 5.f;     // warning window - NEVER scaled by difficulty

	UPROPERTY(EditAnywhere, Category = "BL|Hazard")
	float ActiveSeconds = 4.f;        // damage window

	UPROPERTY(EditAnywhere, Category = "BL|Hazard")
	float ZoneRadius = 1500.f;        // cm, centered on the actor

	UPROPERTY(EditAnywhere, Category = "BL|Hazard")
	float DamagePerSecond = 55.f;

	/** Seconds already elapsed in the first idle phase - staggers multiple hazards. */
	UPROPERTY(EditAnywhere, Category = "BL|Hazard")
	float StartOffsetSeconds = 0.f;

	UFUNCTION(BlueprintPure, Category = "BL|Hazard")
	EBLHazardPhase GetPhase() const { return Phase; }

protected:
	virtual void BeginPlay() override;

	/** Subclass hook (steam VFX, audio, boss-command reactions...). */
	virtual void OnPhaseChanged(EBLHazardPhase NewPhase) {}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Hazard")
	TObjectPtr<UPointLightComponent> WarnLight;

	/** Molten pool covering the zone: blinks during telegraph, solid while active. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Hazard")
	TObjectPtr<UStaticMeshComponent> PourPool;

private:
	void SetPhase(EBLHazardPhase NewPhase);
	void ApplyZoneDamage(float DeltaTime);
	void UpdateFX();

	EBLHazardPhase Phase = EBLHazardPhase::Idle;
	float PhaseTime = 0.f;
};
