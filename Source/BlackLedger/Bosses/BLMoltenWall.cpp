// Black Ledger - the Foundryman's Molten Wall

#include "BLMoltenWall.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Vehicles/BLCombatVehicle.h"
#include "Vehicles/BLHealthComponent.h"

ABLMoltenWall::ABLMoltenWall()
{
	PrimaryActorTick.bCanEverTick = true;

	Strip = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Strip"));
	SetRootComponent(Strip);
	Strip->SetCollisionEnabled(ECollisionEnabled::NoCollision); // damage zone, not a barrier
	Strip->SetCastShadow(false);
	Strip->SetCanEverAffectNavigation(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		Strip->SetStaticMesh(CubeFinder.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (MatFinder.Succeeded())
	{
		Strip->SetMaterial(0, MatFinder.Object);
	}

	Glow = CreateDefaultSubobject<UPointLightComponent>(TEXT("Glow"));
	Glow->SetupAttachment(Strip);
	Glow->SetRelativeLocation(FVector(0.f, 0.f, 300.f));
	Glow->SetLightColor(FColor(255, 90, 12));
	Glow->SetIntensity(0.f);
	Glow->SetCastShadows(false);
}

void ABLMoltenWall::Configure(float InLength, float InWidth, float InTelegraphSeconds,
	float InActiveSeconds, float InDamagePerSecond, AActor* InInstigatorActor)
{
	HalfLength = InLength * 0.5f;
	HalfWidth = InWidth * 0.5f;
	TelegraphSeconds = InTelegraphSeconds;
	ActiveSeconds = InActiveSeconds;
	DamagePerSecond = InDamagePerSecond;
	InstigatorActor = InInstigatorActor;

	Strip->SetRelativeScale3D(FVector(InLength / 100.f, InWidth / 100.f, 0.06f));
	// scripted emissive material for a true molten glow (falls back to the lit
	// basic-shape material if M_BL_Emissive hasn't been generated)
	UMaterialInterface* Emissive = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/Game/BlackLedger/FX/M_BL_Emissive.M_BL_Emissive"));
	if (UMaterialInstanceDynamic* MID = Strip->CreateDynamicMaterialInstance(0, Emissive))
	{
		MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.f, 0.28f, 0.04f));
		MID->SetScalarParameterValue(TEXT("Strength"), 5.f);
	}
	Glow->SetAttenuationRadius(InLength * 0.7f);
	SetLifeSpan(TelegraphSeconds + ActiveSeconds);
}

void ABLMoltenWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Age += DeltaTime;

	if (Age < TelegraphSeconds)
	{
		// warning: blinking strip, light swelling
		const float Urgency = Age / TelegraphSeconds;
		const float Pulse = FMath::Sin(Age * (9.f + 22.f * Urgency));
		Strip->SetVisibility(Pulse > -0.2f);
		Glow->SetIntensity(FMath::Lerp(15000.f, 60000.f, Urgency));
		return;
	}

	// eruption window: solid burn, heavy light
	Strip->SetVisibility(true);
	Glow->SetIntensity(180000.f);

	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* Pawn = *It;
		if (Pawn == InstigatorActor.Get())
		{
			continue;
		}
		if (const ABLCombatVehicle* V = Cast<ABLCombatVehicle>(Pawn))
		{
			if (V->bHazardImmune)
			{
				continue;
			}
		}
		// rotation/translation only - the root carries the visual scale
		const FVector Local = GetActorRotation().UnrotateVector(
			Pawn->GetActorLocation() - GetActorLocation());
		if (FMath::Abs(Local.X) > HalfLength + 120.f || FMath::Abs(Local.Y) > HalfWidth + 120.f)
		{
			continue;
		}
		if (UBLHealthComponent* Victim = Pawn->FindComponentByClass<UBLHealthComponent>())
		{
			Victim->ApplyDamage(DamagePerSecond * DeltaTime);
		}
	}
}
