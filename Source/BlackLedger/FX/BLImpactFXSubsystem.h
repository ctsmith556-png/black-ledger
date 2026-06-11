// Black Ledger - centralized combat feel (TDD section 5)
// Every impact routes through here so all weapons feel consistent:
// hit-stop (global time dip, weight-scaled), camera shake (distance falloff),
// the death moment (30% slow-mo), and later the 3-layer audio + particles/decals.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BLImpactFXSubsystem.generated.h"

UENUM(BlueprintType)
enum class EBLImpactWeight : uint8
{
	Light,    // machine-gun round: shake only, no hit-stop (10/s would be constant slow-mo)
	Medium,   // light pickups
	Heavy,    // missiles
	Massive   // specials / Final Notice
};

UCLASS()
class BLACKLEDGER_API UBLImpactFXSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** The single entry point for every projectile/melee/hazard impact. */
	UFUNCTION(BlueprintCallable, Category = "BL|FX")
	void PlayImpact(const FVector& Location, EBLImpactWeight Weight);

	/** Bible 4.4: ~30% time-slow for ~1 s on a kill (kill-cam linger comes later). */
	UFUNCTION(BlueprintCallable, Category = "BL|FX")
	void PlayDeathMoment();

private:
	void ApplyTimeDip(float Dilation, float RealSeconds);
	void RestoreTime();

	FTimerHandle TimeDipTimer;
	float ActiveDilation = 1.f;
};
