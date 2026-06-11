// Black Ledger - Homing Missile projectile

#include "BLProjectile_Homing.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Vehicles/BLHealthComponent.h"

ABLProjectile_Homing::ABLProjectile_Homing()
{
	Damage = 80.f;          // direct hit (tuning sheet)
	MuzzleSpeed = 5000.f;   // slower than MG; the tracking does the work
	LifeSeconds = 4.f;
	ImpactWeight = EBLImpactWeight::Heavy;
	HitImpulse = 350.f; // direct hits visibly shove the victim

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

void ABLProjectile_Homing::ApplyImpactDamage(AActor* OtherActor, const FHitResult& Hit)
{
	// direct hit
	Super::ApplyImpactDamage(OtherActor, Hit);

	// splash to everyone else nearby (not the direct victim, not the shooter)
	const FVector Center = Hit.bBlockingHit ? FVector(Hit.ImpactPoint) : GetActorLocation();
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* Pawn = *It;
		if (Pawn == OtherActor || Pawn == GetInstigator())
		{
			continue;
		}
		const float DistSq = FVector::DistSquared(Pawn->GetActorLocation(), Center);
		if (DistSq > FMath::Square(SplashRadius))
		{
			continue;
		}
		if (UBLHealthComponent* Victim = Pawn->FindComponentByClass<UBLHealthComponent>())
		{
			Victim->ApplyDamage(SplashDamage);
		}
		// explosion knockback: radial shove + a little lift, fading with distance
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Pawn->GetRootComponent()))
		{
			if (Prim->IsSimulatingPhysics())
			{
				const float Falloff = 1.f - 0.7f * (FMath::Sqrt(DistSq) / SplashRadius);
				FVector Dir = (Pawn->GetActorLocation() - Center).GetSafeNormal();
				Dir.Z = FMath::Max(Dir.Z, 0.25f); // always some lift - reads as blast
				Prim->AddImpulse(Dir.GetSafeNormal() * SplashImpulse * Falloff, NAME_None, /*bVelChange*/ true);
			}
		}
	}
}
