// Black Ledger - impact camera shake (TDD section 5)
// One C++ perlin shake; the subsystem scales it small (MG) -> massive (special).

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "BLCameraShake_Impact.generated.h"

UCLASS()
class BLACKLEDGER_API UBLCameraShake_Impact : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	UBLCameraShake_Impact(const FObjectInitializer& ObjectInitializer);
};
