// Black Ledger - attack telegraph ring

#include "BLTelegraphRing.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

ABLTelegraphRing::ABLTelegraphRing()
{
	PrimaryActorTick.bCanEverTick = true;

	Disc = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Disc"));
	SetRootComponent(Disc);
	Disc->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Disc->SetCastShadow(false);
	Disc->SetCanEverAffectNavigation(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylFinder(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylFinder.Succeeded())
	{
		Disc->SetStaticMesh(CylFinder.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (MatFinder.Succeeded())
	{
		Disc->SetMaterial(0, MatFinder.Object);
	}
}

void ABLTelegraphRing::Configure(float Radius, float Seconds, const FLinearColor& Color)
{
	Life = FMath::Max(Seconds, 0.1f);
	Disc->SetRelativeScale3D(FVector(Radius / 50.f, Radius / 50.f, 0.05f));
	if (UMaterialInstanceDynamic* MID = Disc->CreateDynamicMaterialInstance(0))
	{
		MID->SetVectorParameterValue(TEXT("Color"), Color);
	}
	SetLifeSpan(Life);
}

void ABLTelegraphRing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Age += DeltaTime;
	// blink, quickening toward impact
	const float Urgency = Age / Life;
	const float Pulse = FMath::Sin(Age * (10.f + 24.f * Urgency));
	Disc->SetVisibility(Pulse > -0.2f);
}
