// Black Ledger - projectile base

#include "BLProjectile.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Vehicles/BLHealthComponent.h"

ABLProjectile::ABLProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(8.f);
	Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	// QUERY-ONLY: rounds hit via their own movement sweep, but never exist as
	// physics bodies - so no vehicle (any speed) can ram its own or anyone
	// else's projectiles and get physically stopped by them
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetNotifyRigidBodyCollision(true); // generate hit events on blocking sweep
	Collision->SetCanEverAffectNavigation(false); // projectiles must not dirty navmesh tiles

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

	// in-flight glow; off by default (MG rounds), enabled bright for heavy ordnance in BeginPlay
	GlowLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("GlowLight"));
	GlowLight->SetupAttachment(Collision);
	GlowLight->SetIntensity(0.f);
	GlowLight->SetAttenuationRadius(1400.f);
	GlowLight->SetLightColor(FColor(255, 120, 40));   // ember
	GlowLight->SetCastShadows(false);

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

	// glowing tracer: swap to the scripted emissive material if it exists (the
	// ctor leaves the lit basic-shape material as a fallback)
	if (TracerMesh)
	{
		if (UMaterialInterface* Emissive = LoadObject<UMaterialInterface>(
			nullptr, TEXT("/Game/BlackLedger/FX/M_BL_Emissive.M_BL_Emissive")))
		{
			if (UMaterialInstanceDynamic* MID = TracerMesh->CreateDynamicMaterialInstance(0, Emissive))
			{
				const FLinearColor C = (ImpactWeight == EBLImpactWeight::Light)
					? FLinearColor(1.f, 0.85f, 0.45f)   // MG: hot tracer yellow
					: FLinearColor(1.f, 0.5f, 0.15f);   // heavier ordnance: ember
				MID->SetVectorParameterValue(TEXT("Color"), C);
				MID->SetScalarParameterValue(TEXT("Strength"), 6.f);
			}
		}
	}

	// heavy ordnance carries a real moving light so missiles streak through the dark
	// and splash the walls with ember light; MG rounds stay dark (cost) but bright-emissive.
	if (GlowLight && ImpactWeight != EBLImpactWeight::Light)
	{
		const bool bMassive = (ImpactWeight == EBLImpactWeight::Massive);
		GlowLight->SetIntensity(bMassive ? 22000.f : 14000.f);
		GlowLight->SetAttenuationRadius(bMassive ? 1800.f : 1400.f);
		GlowLight->SetLightColor((ImpactWeight == EBLImpactWeight::Massive)
			? FColor(255, 150, 70) : FColor(255, 110, 35));
	}

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

void ABLProjectile::ApplyImpactDamage(AActor* OtherActor, const FHitResult& Hit)
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

	// splash to everyone else nearby (not the direct victim, not the shooter)
	if (SplashDamage <= 0.f || SplashRadius <= 0.f)
	{
		return;
	}
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
		if (SplashImpulse > 0.f)
		{
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Pawn->GetRootComponent()))
			{
				if (Prim->IsSimulatingPhysics())
				{
					const float Falloff = 1.f - 0.7f * (FMath::Sqrt(DistSq) / SplashRadius);
					FVector Dir = (Pawn->GetActorLocation() - Center).GetSafeNormal();
					Dir.Z = FMath::Max(Dir.Z, 0.25f); // always some lift - reads as blast
					Prim->AddImpulse(Dir.GetSafeNormal() * SplashImpulse * Falloff, NAME_None, true);
				}
			}
		}
	}
}
