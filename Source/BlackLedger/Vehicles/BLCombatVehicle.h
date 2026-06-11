// Black Ledger - the combat vehicle pawn (TDD section 2, Phase 1: arcade movement)
// Architecture: physics box root + spring-raycast suspension; body and wheels are
// visual meshes driven by the traces. Wheels spin/steer cosmetically.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BLCombatVehicle.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UBLHealthComponent;
class UBLWeaponComponent;

UCLASS()
class BLACKLEDGER_API ABLCombatVehicle : public APawn
{
	GENERATED_BODY()

public:
	ABLCombatVehicle();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// ---- components ----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Vehicle")
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Vehicle")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Vehicle")
	TObjectPtr<UStaticMeshComponent> WheelFL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Vehicle")
	TObjectPtr<UStaticMeshComponent> WheelFR;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Vehicle")
	TObjectPtr<UStaticMeshComponent> WheelRL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Vehicle")
	TObjectPtr<UStaticMeshComponent> WheelRR;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Vehicle")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Vehicle")
	TObjectPtr<UCameraComponent> ChaseCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Vehicle")
	TObjectPtr<UBLHealthComponent> Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Vehicle")
	TObjectPtr<UBLWeaponComponent> Weapon;

	// ---- chassis geometry (Surgeon, June 10 re-prep report; data assets later) ----
	UPROPERTY(EditAnywhere, Category = "BL|Chassis")
	float AxleFrontX = 177.78f;        // pivot is wheelbase CENTER (from Surgeon_UE.fbx nodes)

	UPROPERTY(EditAnywhere, Category = "BL|Chassis")
	float AxleRearX = -177.78f;        // symmetric around the wheelbase-center pivot

	UPROPERTY(EditAnywhere, Category = "BL|Chassis")
	float TrackHalfY = 100.78f;        // center -> wheel lateral (cm)

	UPROPERTY(EditAnywhere, Category = "BL|Chassis")
	float FrontWheelRadius = 39.2f;    // cm

	UPROPERTY(EditAnywhere, Category = "BL|Chassis")
	float RearWheelRadius = 39.2f;     // cm

	UPROPERTY(EditAnywhere, Category = "BL|Chassis")
	float AnchorZ = -55.f;             // suspension anchor height in box space (cm)

	UPROPERTY(EditAnywhere, Category = "BL|Chassis")
	float BodyMeshZOffset = -112.f;    // body ground-origin relative to box center (cm)

	UPROPERTY(EditAnywhere, Category = "BL|Chassis")
	float MassKg = 2200.f;

	// ---- suspension ----
	UPROPERTY(EditAnywhere, Category = "BL|Suspension")
	float SuspensionTravel = 26.f;     // cm of usable travel below the anchor

	UPROPERTY(EditAnywhere, Category = "BL|Suspension")
	float SpringStrength = 52000.f;    // force per cm of compression (kg cm/s^2)

	UPROPERTY(EditAnywhere, Category = "BL|Suspension")
	float SpringDamping = 7200.f;      // force per cm/s of vertical point velocity

	UPROPERTY(EditAnywhere, Category = "BL|Suspension")
	float MaxSpringForce = 2300000.f;  // per-corner force cap - stops landing pogo-launch

	// ---- airborne handling ----
	UPROPERTY(EditAnywhere, Category = "BL|Air")
	float ExtraFallGravity = 1500.f;   // cm/s^2 extra downward when airborne (weighty jumps)

	UPROPERTY(EditAnywhere, Category = "BL|Air")
	float AirAngularDamping = 3.f;     // 1/s tumble damping while airborne

	UPROPERTY(EditAnywhere, Category = "BL|Air")
	float AirLevelStrength = 140.f;    // deg/s auto-level toward upright while airborne

	// ---- drive ----
	UPROPERTY(EditAnywhere, Category = "BL|Drive")
	float EngineForce = 2200000.f;     // kg cm/s^2 (~1g for 2.2t)

	UPROPERTY(EditAnywhere, Category = "BL|Drive")
	float MaxSpeedKph = 95.f;

	UPROPERTY(EditAnywhere, Category = "BL|Drive")
	float MaxReverseKph = 32.f;

	UPROPERTY(EditAnywhere, Category = "BL|Drive")
	float MaxYawRateDeg = 70.f;        // deg/s yaw at full lock + full speed

	UPROPERTY(EditAnywhere, Category = "BL|Drive")
	float SteerResponse = 7.f;         // how fast yaw rate eases to target (1/s)

	UPROPERTY(EditAnywhere, Category = "BL|Drive")
	float GripCoefficient = 7.f;       // 1/s lateral velocity cancel rate

	UPROPERTY(EditAnywhere, Category = "BL|Drive")
	float HandbrakeGripScale = 0.15f;  // rear grip while drifting (lower = looser)

	UPROPERTY(EditAnywhere, Category = "BL|Drive")
	float HandbrakeYawBoost = 1.7f;    // extra yaw authority while sliding

	UPROPERTY(EditAnywhere, Category = "BL|Drive")
	float HandbrakeBrakeForce = 1700000.f; // longitudinal braking on handbrake

	UPROPERTY(EditAnywhere, Category = "BL|Drive")
	float CoastDrag = 0.8f;            // 1/s when off-throttle

	UPROPERTY(EditAnywhere, Category = "BL|Drive")
	float RollingDrag = 0.12f;         // 1/s always

	UPROPERTY(EditAnywhere, Category = "BL|Drive")
	float SelfRightDelay = 2.f;        // s upside-down before auto-recover

	UPROPERTY(EditAnywhere, Category = "BL|Debug")
	bool bDrawSuspensionDebug = true;   // Phase-1 default ON; flip off when tuned

protected:
	virtual void BeginPlay() override;

	// input
	void InputThrottle(float Value) { ThrottleInput = Value; }
	void InputSteer(float Value) { SteerInput = Value; }
	void InputHandbrakePressed() { bHandbrake = true; }
	void InputHandbrakeReleased() { bHandbrake = false; }
	void InputFirePressed();
	void InputFireReleased();
	void InputFirePickup();

private:
	struct FBLWheel
	{
		UStaticMeshComponent* Mesh = nullptr;
		FVector AnchorLocal = FVector::ZeroVector;
		float Radius = 40.f;
		bool bFront = false;
		bool bGrounded = false;
		float SpinDeg = 0.f;
	};

	void StepSuspensionAndDrive(float Dt);
	void StepWheelVisuals(float Dt);
	void StepSelfRight(float Dt);

	TArray<FBLWheel, TInlineAllocator<4>> Wheels;
	float ThrottleInput = 0.f;
	float SteerInput = 0.f;
	bool bHandbrake = false;
	float UpsideDownTime = 0.f;
	int32 GroundedCount = 0;
};
