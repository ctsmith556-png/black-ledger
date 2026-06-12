// Black Ledger - Power Missile

#include "BLProjectile_Power.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABLProjectile_Power::ABLProjectile_Power()
{
	// tuning sheet: 200 direct / 60 splash - kills most roster cars in 2 directs
	Damage = 200.f;
	SplashDamage = 60.f;
	SplashRadius = 750.f;
	SplashImpulse = 450.f;
	HitImpulse = 600.f;        // freight-train shove
	MuzzleSpeed = 7500.f;      // fast and flat; you aim it, it goes
	LifeSeconds = 2.5f;
	ImpactWeight = EBLImpactWeight::Massive; // the screen should feel this one

	// placeholder: fat heavy dart, visibly bigger than the homing missile
	TracerMesh->SetRelativeScale3D(FVector(2.2f, 0.42f, 0.42f));
}
