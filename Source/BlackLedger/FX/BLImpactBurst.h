// Black Ledger - procedural impact burst: emissive fireball + radial sparks +
// a lingering smoke puff + a flash light, all weight-scaled. Spawned by
// UBLImpactFXSubsystem at every impact. Pure C++/primitives (no Niagara) - it
// loads the scripted M_BL_Emissive / M_BL_Smoke materials if present and falls
// back to the engine basic-shape material otherwise.

#pragma once

#include "CoreMinimal.h"
#include "FX/BLImpactFXSubsystem.h"
#include "GameFramework/Actor.h"
#include "BLImpactBurst.generated.h"

class UPointLightComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;

UCLASS()
class BLACKLEDGER_API ABLImpactBurst : public AActor
{
	GENERATED_BODY()

public:
	ABLImpactBurst();

	virtual void Tick(float DeltaTime) override;

	/** Call right after spawning. */
	void Configure(EBLImpactWeight Weight);

private:
	static constexpr int32 MaxSparks = 8;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> Fireball;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> Smoke;

	UPROPERTY()
	TObjectPtr<UPointLightComponent> Flash;

	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> Sparks;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> FireballMID;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> SmokeMID;

	// per-spark velocity (cm/s), parallel to the active prefix of Sparks
	TArray<FVector> SparkVels;
	int32 ActiveSparks = 0;

	float Age = 0.f;
	float Life = 0.3f;
	float StartRadius = 20.f;        // cm, fireball
	float EndRadius = 120.f;
	float FlashIntensity = 30000.f;
	float SmokeEndRadius = 160.f;    // cm, smoke puff at end of life
	FLinearColor BurstColor = FLinearColor(1.f, 0.45f, 0.1f);
};
