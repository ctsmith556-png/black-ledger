// Black Ledger - special ability component (TDD section 2)
// Cooldown timer + activates the character's UBLSpecialAbility; broadcasts
// charge state ("READY") for the HUD.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BLSpecialComponent.generated.h"

class UBLSpecialAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBLOnSpecialReady, bool, bReady);

UCLASS(ClassGroup = (BlackLedger), meta = (BlueprintSpawnableComponent))
class BLACKLEDGER_API UBLSpecialComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBLSpecialComponent();

	/** The character's signature ability (data assets pick this later). */
	UPROPERTY(EditAnywhere, Category = "BL|Special")
	TSubclassOf<UBLSpecialAbility> AbilityClass;

	UPROPERTY(EditAnywhere, Category = "BL|Special")
	float CooldownSeconds = 18.f;

	UPROPERTY(BlueprintAssignable, Category = "BL|Special")
	FBLOnSpecialReady OnReadyChanged;

	UFUNCTION(BlueprintCallable, Category = "BL|Special")
	bool TryActivate();

	UFUNCTION(BlueprintPure, Category = "BL|Special")
	bool IsReady() const { return bReady; }

private:
	void OnCooldownDone();

	FTimerHandle CooldownTimer;
	bool bReady = true;
};
