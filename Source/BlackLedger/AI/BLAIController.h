// Black Ledger - AI combat driver (TDD section 8, Phase 2 piece 4)
// Compact C++ state machine covering the Week-2 task set: chase, strafe-orbit,
// fire primary, grab pickups, unstuck. Drives the same ABLCombatVehicle as the
// player via SetDriveInput. Behavior Trees take over with the bosses (Phase 3) -
// each block here maps 1:1 onto a BT task (DriveTo / StrafeTarget / FirePrimary /
// GrabPickup).

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BLAIController.generated.h"

class ABLCombatVehicle;
class ABLPickupActor;

/** Per-difficulty AI scaling (Bible 4.6: HP + attack frequency, never removing tells). */
USTRUCT()
struct FBLAIDifficultyParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float ThrottleScale = 0.9f;       // how hard it drives

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float FireBurstSeconds = 1.2f;    // MG duty cycle: trigger held...

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float FireRestSeconds = 0.6f;     // ...then released

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float FireConeDeg = 9.f;          // wider = sprays before it's lined up = misses more

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float MissileIntervalSeconds = 5.f;

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float HealthScale = 1.f;          // applied to the AI vehicle on possess

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float EngagementScale = 1.f;      // multiplies EngagementRange (hard = more aware)
};

UCLASS()
class BLACKLEDGER_API ABLAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABLAIController();

	virtual void Tick(float DeltaTime) override;

	// ---- combat driving tuning ----
	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float PursueRange = 2400.f;        // beyond this: drive straight at the target

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float OrbitAngleDeg = 62.f;        // inside PursueRange: aim this far off-target (strafe)

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float MinFightRange = 900.f;       // closer than this: peel away, never grind into the target

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float PeelAngleDeg = 115.f;        // peel-away aim offset (past tangent = opens distance)

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float FireRange = 6500.f;          // MG range

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float MissileConeDeg = 22.f;       // homing forgives more

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float MissileMinRange = 900.f;     // don't splash yourself

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float PickupSeekRange = 9000.f;    // how far it will detour for ammo

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float SteerResponseDeg = 35.f;     // full lock at this much heading error

	// ---- free-roam / dispersion (2x arena scale doctrine) ----
	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float EngagementRange = 16000.f;   // cm; vehicles beyond this aren't noticed

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float RoamThrottle = 0.85f;        // cruising speed while wandering

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float DisengageCheckSeconds = 15.f; // how often the AI considers breaking off

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	float DisengageChance = 0.3f;      // chance per check to detour for 8-14s

	// ---- difficulty profiles (defaults set in the constructor) ----
	UPROPERTY(EditAnywhere, Category = "BL|AI")
	FBLAIDifficultyParams EasyParams;

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	FBLAIDifficultyParams MediumParams;

	UPROPERTY(EditAnywhere, Category = "BL|AI")
	FBLAIDifficultyParams HardParams;

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	ABLCombatVehicle* GetVehicle() const;
	APawn* AcquireTarget(float RangeScale);
	ABLPickupActor* FindPickup() const;
	FVector PickRoamPoint() const;
	const FBLAIDifficultyParams& GetParams() const;

	float StuckTime = 0.f;
	float ReverseTime = 0.f;           // > 0 while running the unstuck maneuver
	float OrbitSign = 1.f;             // strafe direction; flips on unstuck
	float LastSteer = 0.f;
	float FireCycleTime = 0.f;         // clock for the burst/rest duty cycle
	double LastMissileTime = -100.0;

	TWeakObjectPtr<APawn> CurrentTarget; // sticky target (hysteresis, no flip-flop)
	FVector RoamTarget = FVector::ZeroVector;
	bool bHasRoamTarget = false;
	float RoamTime = 0.f;
	float WanderClock = 0.f;
	float DisengageTime = 0.f;         // > 0 = voluntarily detouring from combat
};
