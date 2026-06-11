// Black Ledger - projectile base

#include "BLProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Vehicles/BLHealthComponent.h"

ABLProjectile::ABLProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(8.f);
	Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Collision->SetNotifyRigidBodyCollision(true); // generate hit events on blocking sweep

	// placeholder tracer so rounds are visible in the gym; real FX later
	TracerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TracerMesh"));
	TracerMesh->SetupAttachment(Collision);
	TracerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 45x12 cm: from the chase cam you mostly see the round's tail cross-section,
	// so thickness drives readability (6 cm proved invisible; 16 read too chunky)
	TracerMesh->SetRelativeScale3D(FVector(0.45f, 0.12f, 0.12f));
	TracerMesh->SetCastShadow(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFinder(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereFinder.Succeeded())
	{
		TracerMesh->SetStaticMesh(SphereFinder.Object);
	}

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->UpdatedComponent = Collision;
	Movement->ProjectileGravityScale = 0.f;    // MG rounds fly flat
	Movement->bRotationFollowsVelocity = true;
	Movement->bSweepCollision = true;          // no tunnelling at 9000 cm/s
	Movement->InitialSpeed = MuzzleSpeed;
	Movement->MaxSpeed = 0.f;                  // uncapped; Launch() sets the real speed
}

void ABLProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSeconds);
	Collision->OnComponentHit.AddDynamic(this, &ABLProjectile::OnHit);

	// never collide with the vehicle that fired it
	if (AActor* MyInstigator = GetInstigator())
	{
		Collision->IgnoreActorWhenMoving(MyInstigator, true);
		if (UPrimitiveComponent* InstigatorRoot = Cast<UPrimitiveComponent>(MyInstigator->GetRootComponent()))
		{
			InstigatorRoot->IgnoreActorWhenMoving(this, true);
		}
	}
}

void ABLProjectile::Launch(const FVector& Dir, const FVector& ShooterVelocity)
{
	const FVector D = Dir.GetSafeNormal();
	// inherit the shooter's closing speed along the aim direction (never negative -
	// reversing must not produce slow rounds)
	const float Inherited = FMath::Max(0.f, FVector::DotProduct(ShooterVelocity, D));
	Movement->Velocity = D * (MuzzleSpeed + Inherited);
	SetActorRotation(D.Rotation());
}

void ABLProjectile::OnHit(UPrimitiveComponent* /*HitComp*/, AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/, FVector /*NormalImpulse*/, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherActor != GetInstigator())
	{
		ApplyImpactDamage(OtherActor, Hit);
	}
	if (UBLImpactFXSubsystem* FX = GetWorld()->GetSubsystem<UBLImpactFXSubsystem>())
	{
		FX->PlayImpact(Hit.bBlockingHit ? FVector(Hit.ImpactPoint) : GetActorLocation(), ImpactWeight);
	}
	Destroy();
}

void ABLProjectile::ApplyImpactDamage(AActor* OtherActor, const FHitResult& /*Hit*/)
{
	if (UBLHealthComponent* Victim = OtherActor->FindComponentByClass<UBLHealthComponent>())
	{
		Victim->ApplyDamage(Damage);
	}
	if (HitImpulse > 0.f)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent()))
		{
			if (Prim->IsSimulatingPhysics())
			{
				Prim->AddImpulse(GetVelocity().GetSafeNormal() * HitImpulse, NAME_None, /*bVelChange*/ true);
			}
		}
	}
}
