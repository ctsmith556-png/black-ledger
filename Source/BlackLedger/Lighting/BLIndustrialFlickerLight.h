// Black Ledger - reusable flickering industrial light (Mill Build Plan section 24).
// Furnace glow, catwalk bulbs, loading-bay sodium lamps, broken fluorescents, warning
// lamps. Drop it in and tune per instance - no hand-keyframed lights except hero moments.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BLIndustrialFlickerLight.generated.h"

class UPointLightComponent;

UCLASS()
class BLACKLEDGER_API ABLIndustrialFlickerLight : public AActor
{
	GENERATED_BODY()

public:
	ABLIndustrialFlickerLight();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Light")
	TObjectPtr<UPointLightComponent> Light;

	UPROPERTY(EditAnywhere, Category = "BL|Light")
	float BaseIntensity = 4000.f;

	UPROPERTY(EditAnywhere, Category = "BL|Light")
	float FlickerAmount = 1800.f;

	UPROPERTY(EditAnywhere, Category = "BL|Light")
	float FlickerSpeed = 12.f;

	/** Per-tick chance of a brief brown-out cut (0..1). */
	UPROPERTY(EditAnywhere, Category = "BL|Light")
	float RandomCutChance = 0.015f;

	UPROPERTY(EditAnywhere, Category = "BL|Light")
	FColor LightColor = FColor(255, 170, 90, 255);

	UPROPERTY(EditAnywhere, Category = "BL|Light")
	float AttenuationRadius = 2600.f;

private:
	float TimeOffset = 0.f;
};
