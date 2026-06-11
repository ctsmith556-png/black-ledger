// Black Ledger - placeholder impact burst (flash light + expanding fireball)
// Spawned by UBLImpactFXSubsystem at every impact, sized by weight.
// Stands in until authored Niagara impacts/explosions exist; the routing stays.

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
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> Fireball;

	UPROPERTY()
	TObjectPtr<UPointLightComponent> Flash;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> FireballMID;

	float Age = 0.f;
	float Life = 0.3f;
	float StartRadius = 20.f;   // cm
	float EndRadius = 120.f;    // cm
	float FlashIntensity = 30000.f;
};
