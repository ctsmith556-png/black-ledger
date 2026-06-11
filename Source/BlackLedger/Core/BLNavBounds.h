// Black Ledger - navigation bounds without a brush volume.
// NavMeshBoundsVolume is a brush actor, which editor python can't author - this
// registers an AABB with the navigation system directly, so scripted greybox maps
// (make_mill_map.py) get a runtime-generated navmesh. TDD section 6: runtime
// navigation is the gating tech for destructible map transformations.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BLNavBounds.generated.h"

UCLASS()
class BLACKLEDGER_API ABLNavBounds : public AActor
{
	GENERATED_BODY()

public:
	ABLNavBounds();

	/** Half-size of the navigable box, centered on this actor. */
	UPROPERTY(EditAnywhere, Category = "BL|Nav")
	FVector BoundsExtent = FVector(10000.f, 8000.f, 2000.f);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void RegisterBounds(bool bAdd);
};
