// Black Ledger - Homing Missile projectile

#include "BLProjectile_Homing.h"
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
		if (FVector::DistSquared(Pawn->GetActorLocation(), Center) > FMath::Square(SplashRadius))
		{
			continue;
		}
		if (UBLHealthComponent* Victim = Pawn->FindComponentByClass<UBLHealthComponent>())
		{
			Victim->ApplyDamage(SplashDamage);
		}
	}
}
