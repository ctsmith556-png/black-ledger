// Black Ledger - the Diagnostic Field actor

#include "BLDiagnosticField.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Vehicles/BLCombatVehicle.h"
#include "Vehicles/BLHealthComponent.h"

ABLDiagnosticField::ABLDiagnosticField()
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

	Glow = CreateDefaultSubobject<UPointLightComponent>(TEXT("Glow"));
	Glow->SetupAttachment(Disc);
	Glow->SetRelativeLocation(FVector(0.f, 0.f, 400.f));
	Glow->SetLightColor(FColor(120, 235, 220)); // surgical teal
	Glow->SetIntensity(30000.f);
	Glow->SetCastShadows(false);
}

void ABLDiagnosticField::Configure(ABLCombatVehicle* InOwner, float InRadius,
	float InDuration, float InMarkMultiplier, float InHealPerSecond)
{
	OwnerVehicle = InOwner;
	Radius = InRadius;
	MarkMultiplier = InMarkMultiplier;
	HealPerSecond = InHealPerSecond;

	Disc->SetRelativeScale3D(FVector(Radius / 50.f, Radius / 50.f, 0.05f));
	Disc->SetRelativeLocation(FVector(0.f, 0.f, 25.f));
	if (UMaterialInstanceDynamic* MID = Disc->CreateDynamicMaterialInstance(0))
	{
		MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.35f, 0.95f, 0.88f));
	}
	Glow->SetAttenuationRadius(Radius * 1.6f);
	SetLifeSpan(InDuration);
}

void ABLDiagnosticField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ABLCombatVehicle* Surgeon = OwnerVehicle.Get();
	if (!Surgeon)
	{
		Destroy();
		return;
	}

	// diagnose enemies inside the field; release the ones that escape
	const FVector Center = Surgeon->GetActorLocation();
	for (TActorIterator<ABLCombatVehicle> It(GetWorld()); It; ++It)
	{
		ABLCombatVehicle* Other = *It;
		if (Other == Surgeon || !Other->Health)
		{
			continue;
		}
		const bool bInside = FVector::DistSquared2D(Other->GetActorLocation(), Center)
			< FMath::Square(Radius) && !Other->Health->IsDead();
		const bool bMarked = Marked.Contains(Other);
		if (bInside && !bMarked)
		{
			Other->Health->IncomingDamageMultiplier = MarkMultiplier;
			Marked.Add(Other);
		}
		else if (!bInside && bMarked)
		{
			Other->Health->IncomingDamageMultiplier = 1.f;
			Marked.Remove(Other);
		}
	}

	// the Surgeon heals while a patient is on the table
	int32 LiveMarks = 0;
	for (const TWeakObjectPtr<ABLCombatVehicle>& M : Marked)
	{
		if (M.IsValid())
		{
			LiveMarks++;
		}
	}
	if (LiveMarks > 0 && Surgeon->Health)
	{
		Surgeon->Health->Heal(HealPerSecond * DeltaTime);
	}
}

void ABLDiagnosticField::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RestoreAllMarks();
	Super::EndPlay(EndPlayReason);
}

void ABLDiagnosticField::RestoreAllMarks()
{
	for (const TWeakObjectPtr<ABLCombatVehicle>& M : Marked)
	{
		if (ABLCombatVehicle* V = M.Get())
		{
			if (V->Health)
			{
				V->Health->IncomingDamageMultiplier = 1.f;
			}
		}
	}
	Marked.Empty();
}
