// Black Ledger - health + armor

#include "BLHealthComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"

UBLHealthComponent::UBLHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBLHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	MaxHealth = BaseHP * (0.7f + (ArmorStat - 1.f) * 0.0889f);
	Health = MaxHealth;
}

void UBLHealthComponent::ApplyDamage(float Amount)
{
	if (Amount <= 0.f || IsDead())
	{
		return;
	}
	Amount *= FMath::Max(IncomingDamageMultiplier, 0.f);
	Health = FMath::Max(Health - Amount, 0.f);
	OnDamaged.Broadcast(Amount, Health);

#if !(UE_BUILD_SHIPPING)
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red,
			FString::Printf(TEXT("%s took %.0f dmg -> %.0f/%.0f"),
				*GetOwner()->GetName(), Amount, Health, MaxHealth));
	}
#endif

	if (IsDead())
	{
		OnDeath.Broadcast();
	}
}

void UBLHealthComponent::ScaleMaxHealth(float Scale)
{
	if (Scale <= 0.f || IsDead())
	{
		return;
	}
	const float Fraction = (MaxHealth > 0.f) ? Health / MaxHealth : 1.f;
	MaxHealth *= Scale;
	Health = MaxHealth * Fraction;
}

void UBLHealthComponent::Heal(float Amount)
{
	if (Amount <= 0.f || IsDead())
	{
		return;
	}
	Health = FMath::Min(Health + Amount, MaxHealth);
}
