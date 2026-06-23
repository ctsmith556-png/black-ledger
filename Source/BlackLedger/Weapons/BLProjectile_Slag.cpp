// Black Ledger - Foundryman slag mortar shell

#include "BLProjectile_Slag.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABLProjectile_Slag::ABLProjectile_Slag()
{
	Damage = 70.f;             // direct hits are rare - the splash is the threat
	SplashDamage = 45.f;
	SplashRadius = 800.f;
	SplashImpulse = 350.f;
	HitImpulse = 400.f;
	LifeSeconds = 6.f;
	ImpactWeight = EBLImpactWeight::Heavy;

	Movement->ProjectileGravityScale = 1.f;   // it's a mortar
	Movement->bRotationFollowsVelocity = true;

	// chunky molten glob placeholder
	TracerMesh->SetRelativeScale3D(FVector(0.8f, 0.6f, 0.6f));
}

void ABLProjectile_Slag::LaunchBallistic(const FVector& Target, float FlightTime)
{
	const float T = FMath::Max(FlightTime, 0.3f);
	const FVector Delta = Target - GetActorLocation();
	const float Gravity = 980.f * Movement->ProjectileGravityScale;
	FVector Vel = Delta / T;
	Vel.Z = Delta.Z / T + 0.5f * Gravity * T;  // counter gravity to land on time
	Movement->Velocity = Vel;
	SetActorRotation(Vel.Rotation());
}
