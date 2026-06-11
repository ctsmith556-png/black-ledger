// Black Ledger - placeholder impact burst

#include "BLImpactBurst.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

ABLImpactBurst::ABLImpactBurst()
{
	PrimaryActorTick.bCanEverTick = true;

	Fireball = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fireball"));
	SetRootComponent(Fireball);
	Fireball->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Fireball->SetCastShadow(false);
	Fireball->SetCanEverAffectNavigation(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFinder(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereFinder.Succeeded())
	{
		Fireball->SetStaticMesh(SphereFinder.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (MatFinder.Succeeded())
	{
		Fireball->SetMaterial(0, MatFinder.Object);
	}

	Flash = CreateDefaultSubobject<UPointLightComponent>(TEXT("Flash"));
	Flash->SetupAttachment(Fireball);
	Flash->SetCastShadows(false);
	Flash->SetAttenuationRadius(1400.f);
}

void ABLImpactBurst::Configure(EBLImpactWeight Weight)
{
	FLinearColor Color(1.f, 0.45f, 0.1f); // explosion orange
	switch (Weight)
	{
	case EBLImpactWeight::Light:   // MG spark: small, pale, fast
		Life = 0.1f;  StartRadius = 7.f;   EndRadius = 22.f;   FlashIntensity = 6000.f;
		Color = FLinearColor(1.f, 0.85f, 0.55f);
		break;
	case EBLImpactWeight::Medium:
		Life = 0.22f; StartRadius = 18.f;  EndRadius = 80.f;   FlashIntensity = 22000.f;
		break;
	case EBLImpactWeight::Heavy:   // missile: real fireball
		Life = 0.35f; StartRadius = 35.f;  EndRadius = 210.f;  FlashIntensity = 60000.f;
		break;
	case EBLImpactWeight::Massive:
		Life = 0.5f;  StartRadius = 50.f;  EndRadius = 330.f;  FlashIntensity = 95000.f;
		break;
	}

	FireballMID = Fireball->CreateDynamicMaterialInstance(0);
	if (FireballMID)
	{
		FireballMID->SetVectorParameterValue(TEXT("Color"), Color); // BasicShapeMaterial param
	}
	Flash->SetLightColor(Color);
	Flash->SetIntensity(FlashIntensity);
	Fireball->SetWorldScale3D(FVector(StartRadius / 50.f)); // engine sphere = 50 cm radius

	SetLifeSpan(Life);
}

void ABLImpactBurst::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Age += DeltaTime;
	const float T = FMath::Clamp(Age / Life, 0.f, 1.f);
	// fast initial expansion that eases out; light dies off linearly
	const float Radius = FMath::Lerp(StartRadius, EndRadius, FMath::Pow(T, 0.4f));
	Fireball->SetWorldScale3D(FVector(Radius / 50.f));
	Flash->SetIntensity(FlashIntensity * (1.f - T));
}
