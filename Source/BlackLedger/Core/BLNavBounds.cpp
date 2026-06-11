// Black Ledger - navigation bounds registration

#include "BLNavBounds.h"
#include "NavigationSystem.h"

namespace
{
	// UE 5.7 made AddNavigationBoundsUpdateRequest protected (NavMeshBoundsVolume
	// calls it internally, but brush volumes can't be authored from python).
	// Standard protected-access pattern: a never-instantiated derived type may form
	// a member pointer to the protected method and invoke it on any base instance.
	struct FBLNavSysAccess : public UNavigationSystemV1
	{
		static void AddBoundsRequest(UNavigationSystemV1& Sys, const FNavigationBoundsUpdateRequest& Request)
		{
			(Sys.*(&FBLNavSysAccess::AddNavigationBoundsUpdateRequest))(Request);
		}
	};
}

ABLNavBounds::ABLNavBounds()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ABLNavBounds::BeginPlay()
{
	Super::BeginPlay();
	RegisterBounds(true);
}

void ABLNavBounds::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RegisterBounds(false);
	Super::EndPlay(EndPlayReason);
}

void ABLNavBounds::RegisterBounds(bool bAdd)
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
	{
		return;
	}
	FNavigationBoundsUpdateRequest Request;
	Request.NavBounds.UniqueID = GetUniqueID();
	Request.NavBounds.AreaBox = FBox::BuildAABB(GetActorLocation(), BoundsExtent);
	Request.NavBounds.Level = GetLevel();
	Request.UpdateRequest = bAdd
		? FNavigationBoundsUpdateRequest::Added
		: FNavigationBoundsUpdateRequest::Removed;
	FBLNavSysAccess::AddBoundsRequest(*NavSys, Request);
}
