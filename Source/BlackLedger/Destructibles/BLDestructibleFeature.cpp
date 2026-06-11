// Black Ledger - destructible map feature base

#include "BLDestructibleFeature.h"
#include "Components/StaticMeshComponent.h"
#include "FX/BLImpactFXSubsystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Vehicles/BLHealthComponent.h"

ABLDestructibleFeature::ABLDestructibleFeature()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->SetMobility(EComponentMobility::Static); // pieces are static level geometry

	Health = CreateDefaultSubobject<UBLHealthComponent>(TEXT("Health"));
	Health->BaseHP = 350.f;   // ~245 effective at armor 1: ~3 missiles or sustained MG
	Health->ArmorStat = 1.f;
}

void ABLDestructibleFeature::BeginPlay()
{
	Super::BeginPlay();
	Health->OnDeath.AddDynamic(this, &ABLDestructibleFeature::HandleBroken);

	// belt-and-braces: after-state geometry stays inert until the collapse
	for (UStaticMeshComponent* Piece : AfterPieces)
	{
		SetPieceActive(Piece, false);
	}
}

UStaticMeshComponent* ABLDestructibleFeature::MakePiece(const TCHAR* Name,
	const FVector& RelLoc, const FVector& Scale, const FRotator& RelRot,
	bool bAfterState, bool bPermanent)
{
	UStaticMeshComponent* Piece = CreateDefaultSubobject<UStaticMeshComponent>(Name);
	Piece->SetupAttachment(RootComponent);
	Piece->SetRelativeLocation(RelLoc);
	Piece->SetRelativeRotation(RelRot);
	Piece->SetRelativeScale3D(Scale);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		Piece->SetStaticMesh(CubeFinder.Object);
	}

	if (bAfterState)
	{
		Piece->SetVisibility(false);
		Piece->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AfterPieces.Add(Piece);
	}
	else if (!bPermanent)
	{
		BeforePieces.Add(Piece);
	}
	return Piece;
}

void ABLDestructibleFeature::HandleBroken()
{
	if (bBroken)
	{
		return;
	}
	bBroken = true;

	// (1) spectacle
	if (UBLImpactFXSubsystem* FX = GetWorld()->GetSubsystem<UBLImpactFXSubsystem>())
	{
		FX->PlayImpact(GetActorLocation(), EBLImpactWeight::Massive);
	}

	// (2) swap to the authored, navigable after-state
	for (UStaticMeshComponent* Piece : BeforePieces)
	{
		SetPieceActive(Piece, false);
	}
	for (UStaticMeshComponent* Piece : AfterPieces)
	{
		SetPieceActive(Piece, true);
	}

	// (3) navigation: the collision changes above dirty the affected navmesh
	// tiles; with RuntimeGeneration=Dynamic they rebuild on their own

	OnCollapsed();
}

void ABLDestructibleFeature::SetPieceActive(UStaticMeshComponent* Piece, bool bActive)
{
	if (Piece)
	{
		Piece->SetVisibility(bActive);
		Piece->SetCollisionEnabled(bActive
			? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	}
}
