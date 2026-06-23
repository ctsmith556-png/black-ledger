// Black Ledger - reusable flickering industrial light

#include "BLIndustrialFlickerLight.h"
#include "Components/PointLightComponent.h"

ABLIndustrialFlickerLight::ABLIndustrialFlickerLight()
{
	PrimaryActorTick.bCanEverTick = true;

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	RootComponent = Light;
	Light->SetCastShadows(false);          // cheap; we have many of these
	Light->SetIntensity(BaseIntensity);
}

void ABLIndustrialFlickerLight::BeginPlay()
{
	Super::BeginPlay();
	// desync each lamp so they don't pulse in unison
	TimeOffset = FMath::FRandRange(0.f, 1000.f);
	Light->SetLightColor(LightColor);
	Light->SetAttenuationRadius(AttenuationRadius);
	Light->SetIntensity(BaseIntensity);
}

void ABLIndustrialFlickerLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float T = GetWorld()->GetTimeSeconds() + TimeOffset;
	const float Noise =
		FMath::Sin(T * FlickerSpeed) * 0.5f +
		FMath::Sin(T * FlickerSpeed * 2.31f) * 0.3f +
		FMath::Sin(T * FlickerSpeed * 5.77f) * 0.2f;

	float FinalIntensity = BaseIntensity + Noise * FlickerAmount;
	if (FMath::FRand() < RandomCutChance)
	{
		FinalIntensity *= FMath::RandRange(0.05f, 0.35f);   // brief brown-out
	}
	Light->SetIntensity(FMath::Max(0.f, FinalIntensity));
}
