// Black Ledger - the Diagnostic Field actor: follows the Surgeon, marks
// enemies inside (x2 incoming damage), heals the Surgeon while anyone is
// diagnosed. Greybox visual: pale surgical-teal disc + light. Real VFX later.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BLDiagnosticField.generated.h"

class ABLCombatVehicle;
class UPointLightComponent;
class UStaticMeshComponent;

UCLASS()
class BLACKLEDGER_API ABLDiagnosticField : public AActor
{
	GENERATED_BODY()

public:
	ABLDiagnosticField();

	virtual void Tick(float DeltaTime) override;

	void Configure(ABLCombatVehicle* InOwner, float InRadius, float InDuration,
		float InMarkMultiplier, float InHealPerSecond);

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void RestoreAllMarks();

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> Disc;

	UPROPERTY()
	TObjectPtr<UPointLightComponent> Glow;

	TWeakObjectPtr<ABLCombatVehicle> OwnerVehicle;
	TSet<TWeakObjectPtr<ABLCombatVehicle>> Marked;
	float Radius = 1500.f;
	float MarkMultiplier = 2.f;
	float HealPerSecond = 8.f;
};
