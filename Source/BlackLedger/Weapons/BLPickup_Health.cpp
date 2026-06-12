// Black Ledger - Health Pack

#include "BLPickup_Health.h"
#include "Vehicles/BLHealthComponent.h"

ABLPickup_Health::ABLPickup_Health()
{
	WeaponName = TEXT("Health Pack");
	RespawnSeconds = 20.f;             // more precious than ammo
	CrateColor = FLinearColor(0.08f, 0.65f, 0.18f);
}

bool ABLPickup_Health::GrantTo(AActor* OtherActor)
{
	UBLHealthComponent* Health = OtherActor->FindComponentByClass<UBLHealthComponent>();
	if (!Health || Health->IsDead())
	{
		return false;
	}
	if (Health->GetHealth() >= Health->GetMaxHealth() - 1.f)
	{
		return false;                  // full HP: leave it for when it matters
	}
	Health->Heal(Health->GetMaxHealth() * HealFraction);
	return true;
}
