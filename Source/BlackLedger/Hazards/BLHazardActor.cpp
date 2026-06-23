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
	// shape the pour pool to match the damage footprint (Circular/Fan/River)
	ShapePourPool();
	// flowing molten slag (scripted M_BL_MoltenSlag); falls back to the flat emissive, then
	// the engine basic-shape material, so the pour always renders.
	UMaterialInterface* Molten = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/Game/BlackLedger/FX/M_BL_MoltenSlag.M_BL_MoltenSlag"));
	if (!Molten)
	{
		Molten = LoadObject<UMaterialInterface>(
			nullptr, TEXT("/Game/BlackLedger/FX/M_BL_Emissive.M_BL_Emissive"));
	}
	if (UMaterialInstanceDynamic* MID = PourPool->CreateDynamicMaterialInstance(0, Molten))
	{
		MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.f, 0.32f, 0.05f));
		MID->SetScalarParameterValue(TEXT("Strength"), 4.f);
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

void ABLHazardActor::CommandeerCycle(float NewCooldownSeconds, float FirstDelaySeconds)
{
	CooldownSeconds = FMath::Max(NewCooldownSeconds, 5.f);
	if (Phase == EBLHazardPhase::Idle)
	{
		PhaseTime = CooldownSeconds - FMath::Max(FirstDelaySeconds, 0.f);
	}
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
		if (!IsInPourZone(ToPawn))
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

bool ABLHazardActor::IsInPourZone(const FVector& ToPoint) const
{
	const float DistSq = ToPoint.SizeSquared2D();
	if (DistSq > FMath::Square(ZoneRadius))
	{
		return false; // ZoneRadius bounds every shape
	}
	if (PourShape == EBLPourShape::Circular)
	{
		return true;
	}

	const FVector2D P(ToPoint.X, ToPoint.Y);
	const float Dist = FMath::Sqrt(DistSq);
	if (Dist < 1.f)
	{
		return true; // dead centre is always in
	}
	const FVector Fwd3 = FRotator(0.f, PourYawDeg, 0.f).Vector();
	const FVector2D Fwd(Fwd3.X, Fwd3.Y);
	const float Along = FVector2D::DotProduct(P, Fwd); // signed forward distance

	if (PourShape == EBLPourShape::Fan)
	{
		if (Along <= 0.f)
		{
			return false; // spray only goes forward
		}
		const float CosToFwd = Along / Dist;
		return CosToFwd >= FMath::Cos(FMath::DegreesToRadians(FanHalfAngleDeg));
	}

	// River: a straight strip crossing the pit along PourYaw
	const float Lateral = FMath::Abs(FVector2D::CrossProduct(Fwd, P)); // Fwd is unit -> perp dist
	const float Len = (RiverLength > 0.f) ? RiverLength : ZoneRadius;
	return FMath::Abs(Along) <= Len && Lateral <= RiverHalfWidth;
}

void ABLHazardActor::ShapePourPool()
{
	// engine Cylinder/Cube basic shapes are 100 cm; cylinder radius = 50 cm.
	const FVector Up(0.f, 0.f, 18.f);
	switch (PourShape)
	{
	case EBLPourShape::Circular:
	default:
		PourPool->SetRelativeRotation(FRotator::ZeroRotator);
		PourPool->SetRelativeLocation(Up);
		PourPool->SetRelativeScale3D(FVector(ZoneRadius / 50.f, ZoneRadius / 50.f, 0.06f));
		break;

	case EBLPourShape::Fan:
	{
		// an ellipse tongue ahead of the pit: long on the aim axis, narrower across.
		PourPool->SetRelativeRotation(FRotator(0.f, PourYawDeg, 0.f));
		const FVector Fwd = FRotator(0.f, PourYawDeg, 0.f).Vector();
		PourPool->SetRelativeLocation(Fwd * (ZoneRadius * 0.45f) + Up);
		PourPool->SetRelativeScale3D(FVector(ZoneRadius * 1.05f / 50.f, ZoneRadius * 0.62f / 50.f, 0.06f));
		break;
	}

	case EBLPourShape::River:
	{
		// straight molten line: swap to a cube strip along the aim axis.
		if (UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")))
		{
			PourPool->SetStaticMesh(Cube);
		}
		const float Len = (RiverLength > 0.f) ? RiverLength : ZoneRadius;
		PourPool->SetRelativeRotation(FRotator(0.f, PourYawDeg, 0.f));
		PourPool->SetRelativeLocation(Up);
		PourPool->SetRelativeScale3D(FVector(Len * 2.f / 100.f, RiverHalfWidth * 2.f / 100.f, 0.12f));
		break;
	}
	}
}
