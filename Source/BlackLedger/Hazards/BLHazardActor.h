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

/** Pour footprint (Bible 5.1 / Mill plan section 8: distinct shapes teach the map).
 *  Circular = overflow ring around the pit; Fan = directed spray; River = straight
 *  molten line across the floor. Fan/River aim along PourYawDeg. */
UENUM(BlueprintType)
enum class EBLPourShape : uint8
{
	Circular,
	Fan,
	River
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

	// ---- pour footprint (Mill plan section 8) ----
	UPROPERTY(EditAnywhere, Category = "BL|Hazard|Shape")
	EBLPourShape PourShape = EBLPourShape::Circular;

	/** Aim for Fan/River, degrees (world yaw). Ignored by Circular. */
	UPROPERTY(EditAnywhere, Category = "BL|Hazard|Shape")
	float PourYawDeg = 0.f;

	/** Fan half-angle (degrees) - total spread is twice this. */
	UPROPERTY(EditAnywhere, Category = "BL|Hazard|Shape")
	float FanHalfAngleDeg = 42.f;

	/** River lateral half-width (cm). */
	UPROPERTY(EditAnywhere, Category = "BL|Hazard|Shape")
	float RiverHalfWidth = 700.f;

	/** River reach each way from the pit along PourYaw (cm); 0 = use ZoneRadius. */
	UPROPERTY(EditAnywhere, Category = "BL|Hazard|Shape")
	float RiverLength = 0.f;

	UFUNCTION(BlueprintPure, Category = "BL|Hazard")
	EBLHazardPhase GetPhase() const { return Phase; }

	/** Boss commandeering (TDD section 7): new cycle speed + force the next
	 *  telegraph after FirstDelaySeconds (if currently idle). */
	UFUNCTION(BlueprintCallable, Category = "BL|Hazard")
	void CommandeerCycle(float NewCooldownSeconds, float FirstDelaySeconds);

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
	/** True if a point offset (cm, world) from the pit lies in the active pour footprint. */
	bool IsInPourZone(const FVector& ToPoint) const;
	void ShapePourPool();

	EBLHazardPhase Phase = EBLHazardPhase::Idle;
	float PhaseTime = 0.f;
};
