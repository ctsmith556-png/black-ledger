// Black Ledger - weapon pickup

#include "BLPickupActor.h"
#include "Audio/BLAudioSubsystem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Weapons/BLProjectile_Homing.h"
#include "Weapons/BLWeaponComponent.h"

ABLPickupActor::ABLPickupActor()
{
	PrimaryActorTick.bCanEverTick = true;

	TouchSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TouchSphere"));
	SetRootComponent(TouchSphere);
	TouchSphere->InitSphereRadius(150.f);
	TouchSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	TouchSphere->SetGenerateOverlapEvents(true);
	TouchSphere->SetCanEverAffectNavigation(false);

	// placeholder: dark floating crate; dossier-styled mesh later
	Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
	Visual->SetupAttachment(TouchSphere);
	Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Visual->SetRelativeScale3D(FVector(0.7f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		Visual->SetStaticMesh(CubeFinder.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (MatFinder.Succeeded())
	{
		Visual->SetMaterial(0, MatFinder.Object);
	}

	ProjectileClass = ABLProjectile_Homing::StaticClass();
}

void ABLPickupActor::BeginPlay()
{
	Super::BeginPlay();
	TouchSphere->OnComponentBeginOverlap.AddDynamic(this, &ABLPickupActor::OnTouch);
	if (UMaterialInstanceDynamic* MID = Visual->CreateDynamicMaterialInstance(0))
	{
		MID->SetVectorParameterValue(TEXT("Color"), CrateColor);
	}
}

void ABLPickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Visual->AddLocalRotation(FRotator(0.f, SpinDegPerSec * DeltaTime, 0.f));
}

void ABLPickupActor::OnTouch(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/,
	bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
	if (!OtherActor || !GrantTo(OtherActor))
	{
		return;
	}
	if (UWorld* World = GetWorld())
	{
		if (UBLAudioSubsystem* Audio = World->GetSubsystem<UBLAudioSubsystem>())
		{
			Audio->PostPickup(GetActorLocation());
		}
	}
	SetPickupActive(false);
	GetWorldTimerManager().SetTimer(RespawnTimer, this, &ABLPickupActor::Respawn, RespawnSeconds, false);
}

bool ABLPickupActor::GrantTo(AActor* OtherActor)
{
	UBLWeaponComponent* Weapon = OtherActor->FindComponentByClass<UBLWeaponComponent>();
	if (!Weapon)
	{
		return false;  // not a combat vehicle (e.g. a stray projectile)
	}
	Weapon->GrantPickup(ProjectileClass, Ammo, WeaponName);
	return true;
}

void ABLPickupActor::Respawn()
{
	SetPickupActive(true);
}

void ABLPickupActor::SetPickupActive(bool bActive)
{
	SetActorHiddenInGame(!bActive);
	SetActorEnableCollision(bActive);
	SetActorTickEnabled(bActive);
}
