// Black Ledger - the Foundryman's Molten Wall (sheet: phase 2+ signature).
// A line of slag telegraphs on the floor, then erupts: a linear burn zone that
// cuts off an escape lane for a few seconds. Telegraph -> window, always.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BLMoltenWall.generated.h"

class UPointLightComponent;
class UStaticMeshComponent;

UCLASS()
class BLACKLEDGER_API ABLMoltenWall : public AActor
{
	GENERATED_BODY()

public:
	ABLMoltenWall();

	virtual void Tick(float DeltaTime) override;

	/** Center/rotation come from the spawn transform; lengths in cm. */
	void Configure(float InLength, float InWidth, float InTelegraphSeconds,
		float InActiveSeconds, float InDamagePerSecond, AActor* InInstigatorActor);

private:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> Strip;

	UPROPERTY()
	TObjectPtr<UPointLightComponent> Glow;

	TWeakObjectPtr<AActor> InstigatorActor;
	float HalfLength = 1500.f;
	float HalfWidth = 175.f;
	float TelegraphSeconds = 1.2f;
	float ActiveSeconds = 4.f;
	float DamagePerSecond = 70.f;
	float Age = 0.f;
};
