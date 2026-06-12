// Black Ledger - Homing Missile projectile

#include "BLProjectile_Homing.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABLProjectile_Homing::ABLProjectile_Homing()
{
	// tuning sheet: 80 direct / 40 splash
	Damage = 80.f;
	SplashDamage = 40.f;
	SplashRadius = 600.f;
	SplashImpulse = 280.f;
	HitImpulse = 350.f;     // direct hits visibly shove the victim
	MuzzleSpeed = 5000.f;   // slower than MG; the tracking does the work
	LifeSeconds = 4.f;
	ImpactWeight = EBLImpactWeight::Heavy;

	Movement->HomingAccelerationMagnitude = HomingAcceleration;
	Movement->MaxSpeed = 5500.f; // cap inherited launch speed so the turn radius stays tight

	// placeholder missile body: long dark dart; real mesh/FX + smoke trail later
	TracerMesh->SetRelativeScale3D(FVector(1.6f, 0.28f, 0.28f));
}

void ABLProjectile_Homing::SetHomingTarget(USceneComponent* TargetComponent)
{
	Movement->bIsHomingProjectile = (TargetComponent != nullptr);
	Movement->HomingTargetComponent = TargetComponent;
	Movement->HomingAccelerationMagnitude = HomingAcceleration;
}
