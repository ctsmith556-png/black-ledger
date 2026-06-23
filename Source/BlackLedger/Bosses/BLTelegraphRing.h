// Black Ledger - generic attack telegraph: a blinking floor ring that quickens
// toward impact (Bible 4.6: every heavy attack telegraph -> window). Used by
// the Foundryman's mortar; reusable for any boss ground-target attack.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BLTelegraphRing.generated.h"

class UStaticMeshComponent;

UCLASS()
class BLACKLEDGER_API ABLTelegraphRing : public AActor
{
	GENERATED_BODY()

public:
	ABLTelegraphRing();

	virtual void Tick(float DeltaTime) override;

	void Configure(float Radius, float Seconds, const FLinearColor& Color);

private:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> Disc;

	float Life = 1.f;
	float Age = 0.f;
};
