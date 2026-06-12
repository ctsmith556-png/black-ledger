// Black Ledger - timed environmental hazard base

#include "BLHazardActor.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Vehicles/BLCombatVehicle.h"
#include "Vehicles/BLHealthComponent.h"

ABLHazardActor::ABLHazardActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// placeholder telegraph/active FX: one orange light. Steam + molten Niagara later.
	WarnLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("WarnLight"));
	WarnLight->SetupAttachment(RootComponent);
	WarnLight->SetRelativeLocation(FVector(0.f, 0.f, 600.f));
	WarnLight->SetIntensity(0.f);
	WarnLight->SetLightColor(FColor(255, 110, 20));
	WarnLight->SetAttenuationRadius(ZoneRadius * 2.f);
	WarnLight->SetCastShadows(false);

	PourPool = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PourPool"));
	PourPool->SetupAttachment(RootComponent);
	PourPool->SetRelativeLocation(FVector(0.f, 0.f, 18.f));
	PourPool->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PourPool->SetCastShadow(false);
	PourPool->SetCanEverAffectNavigation(false);
	PourPool->SetVisibility(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylFinder(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylFinder.Succeeded())
	{
		PourPool->SetStaticMesh(CylFinder.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (MatFinder.Succeeded())
	{
		PourPool->SetMaterial(0, MatFinder.Object);
	}
}

void ABLHazardActor::BeginPlay()
{
	Super::BeginPlay();
	WarnLight->SetAttenuationRadius(ZoneRadius * 2.5f);
	// pool disc sized to the damage zone (engine cylinder = 50 cm radius)
	PourPool->SetRelativeScale3D(FVector(ZoneRadius / 50.f, ZoneRadius / 50.f, 0.06f));
	if (UMaterialInstanceDynamic* MID = PourPool->CreateDynamicMaterialInstance(0))
	{
		MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.f, 0.32f, 0.05f));
	}
	PhaseTime = FMath::Clamp(StartOffsetSeconds, 0.f, CooldownSeconds);
}

void ABLHazardActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PhaseTime += DeltaTime;

	switch (Phase)
	{
	case EBLHazardPhase::Idle:
		if (PhaseTime >= CooldownSeconds)
		{
			SetPhase(EBLHazardPhase::Telegraph);
		}
		break;
	case EBLHazardPhase::Telegraph:
		if (PhaseTime >= TelegraphSeconds)
		{
			SetPhase(EBLHazardPhase::Active);
		}
		break;
	case EBLHazardPhase::Active:
		ApplyZoneDamage(DeltaTime);
		if (PhaseTime >= ActiveSeconds)
		{
			SetPhase(EBLHazardPhase::Idle);
		}
		break;
	}

	UpdateFX();
}

void ABLHazardActor::SetPhase(EBLHazardPhase NewPhase)
{
	Phase = NewPhase;
	PhaseTime = 0.f;
	OnPhaseChanged(NewPhase);
}

void ABLHazardActor::ApplyZoneDamage(float DeltaTime)
{
	const FVector Center = GetActorLocation();
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* Pawn = *It;
		const FVector ToPawn = Pawn->GetActorLocation() - Center;
		if (ToPawn.SizeSquared2D() > FMath::Square(ZoneRadius))
		{
			continue;
		}
		if (const ABLCombatVehicle* V = Cast<ABLCombatVehicle>(Pawn))
		{
			if (V->bHazardImmune)
			{
				continue; // the Foundryman lives here
			}
		}
		if (UBLHealthComponent* Victim = Pawn->FindComponentByClass<UBLHealthComponent>())
		{
			Victim->ApplyDamage(DamagePerSecond * DeltaTime);
		}
	}
	// TODO: burn status effect (UBLStatusComponent) + molten FX/audio through the subsystem
}

void ABLHazardActor::UpdateFX()
{
	float Intensity = 0.f;
	bool bPoolVisible = false;
	switch (Phase)
	{
	case EBLHazardPhase::Telegraph:
	{
		// urgent blink that quickens toward the pour; the pool flashes with it
		const float Urgency = PhaseTime / FMath::Max(TelegraphSeconds, 0.1f);
		const float Pulse = 0.5f + 0.5f * FMath::Sin(PhaseTime * (8.f + 18.f * Urgency));
		Intensity = FMath::Lerp(40000.f, 120000.f, Urgency) * Pulse;
		bPoolVisible = Pulse > 0.5f;
		break;
	}
	case EBLHazardPhase::Active:
		// molten glare with flicker; pool burns solid
		Intensity = 300000.f * (0.9f + 0.1f * FMath::PerlinNoise1D(GetWorld()->GetTimeSeconds() * 13.f));
		bPoolVisible = true;
		break;
	default:
		break;
	}
	WarnLight->SetIntensity(Intensity);
	if (PourPool->IsVisible() != bPoolVisible)
	{
		PourPool->SetVisibility(bPoolVisible);
	}
}
