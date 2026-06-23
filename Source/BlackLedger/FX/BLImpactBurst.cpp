// Black Ledger - procedural impact burst

#include "BLImpactBurst.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	// loaded once: the scripted emissive material (tools/unreal/make_fx_materials.py),
	// or null if it hasn't been generated yet - the caller falls back to the engine mat
	UMaterialInterface* LoadFXMaterial(const TCHAR* Path)
	{
		return LoadObject<UMaterialInterface>(nullptr, Path);
	}
}

ABLImpactBurst::ABLImpactBurst()
{
	PrimaryActorTick.bCanEverTick = true;

	UStaticMesh* Sphere = nullptr;
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFinder(
			TEXT("/Engine/BasicShapes/Sphere.Sphere"));
		Sphere = SphereFinder.Succeeded() ? SphereFinder.Object : nullptr;
	}
	UMaterialInterface* BasicMat = nullptr;
	{
		static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
		BasicMat = MatFinder.Succeeded() ? MatFinder.Object : nullptr;
	}

	Fireball = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fireball"));
	SetRootComponent(Fireball);
	Fireball->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Fireball->SetCastShadow(false);
	Fireball->SetCanEverAffectNavigation(false);
	if (Sphere) { Fireball->SetStaticMesh(Sphere); }
	if (BasicMat) { Fireball->SetMaterial(0, BasicMat); }

	Smoke = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Smoke"));
	Smoke->SetupAttachment(Fireball);
	Smoke->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Smoke->SetCastShadow(false);
	Smoke->SetCanEverAffectNavigation(false);
	if (Sphere) { Smoke->SetStaticMesh(Sphere); }
	if (BasicMat) { Smoke->SetMaterial(0, BasicMat); }

	Flash = CreateDefaultSubobject<UPointLightComponent>(TEXT("Flash"));
	Flash->SetupAttachment(Fireball);
	Flash->SetCastShadows(false);
	Flash->SetAttenuationRadius(1400.f);

	Sparks.Reserve(MaxSparks);
	for (int32 i = 0; i < MaxSparks; ++i)
	{
		UStaticMeshComponent* Spark = CreateDefaultSubobject<UStaticMeshComponent>(
			*FString::Printf(TEXT("Spark_%d"), i));
		Spark->SetupAttachment(Fireball);
		Spark->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Spark->SetCastShadow(false);
		Spark->SetCanEverAffectNavigation(false);
		Spark->SetVisibility(false);
		if (Sphere) { Spark->SetStaticMesh(Sphere); }
		if (BasicMat) { Spark->SetMaterial(0, BasicMat); }
		Sparks.Add(Spark);
	}
}

void ABLImpactBurst::Configure(EBLImpactWeight Weight)
{
	int32 NumSparks = 6;
	float SparkSpeed = 700.f;
	BurstColor = FLinearColor(1.f, 0.45f, 0.1f);

	switch (Weight)
	{
	case EBLImpactWeight::Light:   // MG spark: tiny + cheap (fires ~10/s/car) - no sparks
		Life = 0.16f; StartRadius = 7.f;  EndRadius = 20.f;  FlashIntensity = 6000.f;
		SmokeEndRadius = 24.f; NumSparks = 0; SparkSpeed = 650.f;
		BurstColor = FLinearColor(1.f, 0.85f, 0.55f);
		break;
	case EBLImpactWeight::Medium:
		Life = 0.28f; StartRadius = 16.f; EndRadius = 80.f;  FlashIntensity = 22000.f;
		SmokeEndRadius = 130.f; NumSparks = 5; SparkSpeed = 950.f;
		break;
	case EBLImpactWeight::Heavy:   // missile: real fireball + sparks
		Life = 0.4f;  StartRadius = 32.f; EndRadius = 210.f; FlashIntensity = 60000.f;
		SmokeEndRadius = 320.f; NumSparks = 8; SparkSpeed = 1500.f;
		break;
	case EBLImpactWeight::Massive:
		Life = 0.55f; StartRadius = 48.f; EndRadius = 330.f; FlashIntensity = 95000.f;
		SmokeEndRadius = 480.f; NumSparks = 8; SparkSpeed = 1900.f;
		break;
	}
	ActiveSparks = FMath::Min(NumSparks, MaxSparks);

	// emissive fireball (scripted material if present, else lit basic-shape fallback)
	UMaterialInterface* Emissive = LoadFXMaterial(TEXT("/Game/BlackLedger/FX/M_BL_Emissive.M_BL_Emissive"));
	FireballMID = Fireball->CreateDynamicMaterialInstance(0, Emissive);
	if (FireballMID)
	{
		FireballMID->SetVectorParameterValue(TEXT("Color"), BurstColor);
		FireballMID->SetScalarParameterValue(TEXT("Strength"), 7.f);
	}
	Fireball->SetWorldScale3D(FVector(StartRadius / 50.f)); // engine sphere = 50 cm radius

	// translucent smoke puff (scripted smoke material if present)
	UMaterialInterface* SmokeMat = LoadFXMaterial(TEXT("/Game/BlackLedger/FX/M_BL_Smoke.M_BL_Smoke"));
	SmokeMID = Smoke->CreateDynamicMaterialInstance(0, SmokeMat);
	if (SmokeMID)
	{
		SmokeMID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.05f, 0.05f, 0.06f));
		SmokeMID->SetScalarParameterValue(TEXT("Opacity"), 0.5f);
	}
	Smoke->SetWorldScale3D(FVector(StartRadius / 50.f));

	Flash->SetLightColor(BurstColor);
	Flash->SetIntensity(FlashIntensity);

	// fling the sparks out on random radial vectors (biased upward/outward)
	SparkVels.SetNum(ActiveSparks);
	for (int32 i = 0; i < Sparks.Num(); ++i)
	{
		UStaticMeshComponent* Spark = Sparks[i];
		if (i >= ActiveSparks)
		{
			Spark->SetVisibility(false);
			continue;
		}
		FVector Dir = FMath::VRand();
		Dir.Z = FMath::Abs(Dir.Z) * 0.6f + 0.25f; // bias up so they arc, not sink
		Dir = Dir.GetSafeNormal();
		SparkVels[i] = Dir * SparkSpeed * FMath::FRandRange(0.6f, 1.2f);
		Spark->SetRelativeLocation(FVector::ZeroVector);
		Spark->SetRelativeScale3D(FVector(0.18f, 0.06f, 0.06f)); // little streaks
		Spark->SetVisibility(true);
		if (UMaterialInstanceDynamic* SparkMID = Spark->CreateDynamicMaterialInstance(0, Emissive))
		{
			SparkMID->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.f, 0.8f, 0.4f));
			SparkMID->SetScalarParameterValue(TEXT("Strength"), 9.f);
		}
	}

	SetLifeSpan(Life + 0.15f); // let the smoke/sparks finish after the fireball
}

void ABLImpactBurst::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Age += DeltaTime;
	const float T = FMath::Clamp(Age / Life, 0.f, 1.f);

	// fireball: fast initial expansion that eases out; emissive + light die off
	const float Radius = FMath::Lerp(StartRadius, EndRadius, FMath::Pow(T, 0.4f));
	Fireball->SetWorldScale3D(FVector(Radius / 50.f));
	Flash->SetIntensity(FlashIntensity * (1.f - T));
	if (FireballMID)
	{
		FireballMID->SetScalarParameterValue(TEXT("Strength"), FMath::Lerp(7.f, 0.f, T));
	}
	Fireball->SetVisibility(T < 1.f);

	// smoke: keeps growing and fades a touch slower than the fireball, lingering
	const float ST = FMath::Clamp(Age / (Life + 0.15f), 0.f, 1.f);
	const float SmokeR = FMath::Lerp(StartRadius, SmokeEndRadius, FMath::Pow(ST, 0.6f));
	Smoke->SetWorldScale3D(FVector(SmokeR / 50.f));
	if (SmokeMID)
	{
		SmokeMID->SetScalarParameterValue(TEXT("Opacity"), FMath::Lerp(0.5f, 0.f, ST));
	}

	// sparks: ballistic, shrinking, fading out by mid-life
	const float G = 1400.f;
	for (int32 i = 0; i < ActiveSparks && i < Sparks.Num(); ++i)
	{
		UStaticMeshComponent* Spark = Sparks[i];
		FVector V = SparkVels[i];
		V.Z -= G * DeltaTime;
		SparkVels[i] = V;
		Spark->AddRelativeLocation(V * DeltaTime);
		const float SparkT = FMath::Clamp(Age / (Life * 0.9f), 0.f, 1.f);
		const float S = FMath::Lerp(1.f, 0.f, SparkT);
		Spark->SetRelativeScale3D(FVector(0.18f * S, 0.06f * S, 0.06f * S));
		if (SparkT >= 1.f)
		{
			Spark->SetVisibility(false);
		}
	}
}
