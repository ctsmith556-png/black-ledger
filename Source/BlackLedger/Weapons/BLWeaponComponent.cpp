// Black Ledger - weapon component, machine-gun primary

#include "BLWeaponComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "Vehicles/BLHealthComponent.h"
#include "Weapons/BLProjectile.h"
#include "Weapons/BLProjectile_Homing.h"

UBLWeaponComponent::UBLWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryProjectileClass = ABLProjectile::StaticClass();
}

void UBLWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// placeholder muzzle flash: one reusable point light parked at the active muzzle
	AActor* Owner = GetOwner();
	if (Owner && Owner->GetRootComponent())
	{
		MuzzleLight = NewObject<UPointLightComponent>(Owner, TEXT("BLMuzzleLight"));
		MuzzleLight->SetupAttachment(Owner->GetRootComponent());
		MuzzleLight->SetIntensity(0.f);
		MuzzleLight->SetLightColor(FColor(255, 186, 110)); // hot orange-white
		MuzzleLight->SetAttenuationRadius(700.f);
		MuzzleLight->SetCastShadows(false);
		MuzzleLight->RegisterComponent();
	}
}

void UBLWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopFirePrimary();
	Super::EndPlay(EndPlayReason);
}

void UBLWeaponComponent::StartFirePrimary()
{
	if (bFiringPrimary)
	{
		return;
	}
	bFiringPrimary = true;

	const float Interval = 1.f / FMath::Max(PrimaryFireRate, 0.1f);

	// fire now if the rate allows (so tapping can't beat holding), then loop
	const double Now = GetWorld()->GetTimeSeconds();
	if (Now - LastPrimaryShotTime >= Interval)
	{
		FirePrimaryShot();
	}
	GetWorld()->GetTimerManager().SetTimer(
		PrimaryTimer, this, &UBLWeaponComponent::FirePrimaryShot, Interval, true);
}

void UBLWeaponComponent::StopFirePrimary()
{
	bFiringPrimary = false;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PrimaryTimer);
	}
}

void UBLWeaponComponent::FirePrimaryShot()
{
	AActor* Owner = GetOwner();
	if (!Owner || !PrimaryProjectileClass || MuzzleOffsets.Num() == 0)
	{
		return;
	}

	// aim straight ahead with a touch of cone spread
	FVector Dir = Owner->GetActorForwardVector();
	if (PrimarySpreadDeg > 0.f)
	{
		Dir = FMath::VRandCone(Dir, FMath::DegreesToRadians(PrimarySpreadDeg));
	}

	const FVector MuzzleLocal = MuzzleOffsets[MuzzleIndex % MuzzleOffsets.Num()];
	ABLProjectile* Shot = SpawnProjectile(PrimaryProjectileClass, MuzzleLocal, Dir);
	MuzzleIndex++;
	if (Shot)
	{
		Shot->Damage = PrimaryDamage;
	}
	FlashMuzzle(MuzzleLocal);

	LastPrimaryShotTime = GetWorld()->GetTimeSeconds();
}

void UBLWeaponComponent::GrantPickup(TSubclassOf<ABLProjectile> ProjectileClass, int32 Ammo, FName WeaponName)
{
	if (!ProjectileClass || Ammo <= 0)
	{
		return;
	}
	if (ProjectileClass == PickupProjectileClass)
	{
		PickupAmmo += Ammo;        // same weapon: stack
	}
	else
	{
		PickupProjectileClass = ProjectileClass;  // new weapon: replace
		PickupAmmo = Ammo;
	}
	PickupName = WeaponName;
	OnPickupChanged.Broadcast(PickupName, PickupAmmo);
}

void UBLWeaponComponent::FirePickup()
{
	AActor* Owner = GetOwner();
	if (!Owner || !PickupProjectileClass || PickupAmmo <= 0)
	{
		return;
	}
	const double Now = GetWorld()->GetTimeSeconds();
	if (Now - LastPickupShotTime < PickupFireCooldown)
	{
		return;
	}

	ABLProjectile* Shot = SpawnProjectile(
		PickupProjectileClass, PickupMuzzleOffset, Owner->GetActorForwardVector());
	if (!Shot)
	{
		return;
	}

	if (ABLProjectile_Homing* Missile = Cast<ABLProjectile_Homing>(Shot))
	{
		Missile->SetHomingTarget(FindHomingTarget());
	}
	FlashMuzzle(PickupMuzzleOffset);

	LastPickupShotTime = Now;
	PickupAmmo--;
	if (PickupAmmo <= 0)
	{
		PickupProjectileClass = nullptr;
		PickupName = NAME_None;
	}
	OnPickupChanged.Broadcast(PickupName, PickupAmmo);
}

ABLProjectile* UBLWeaponComponent::SpawnProjectile(
	TSubclassOf<ABLProjectile> Class, const FVector& MuzzleLocal, const FVector& Dir)
{
	AActor* Owner = GetOwner();
	const FVector Muzzle = Owner->GetActorTransform().TransformPosition(MuzzleLocal);

	FActorSpawnParameters Params;
	Params.Owner = Owner;
	Params.Instigator = Cast<APawn>(Owner);
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABLProjectile* Shot = GetWorld()->SpawnActor<ABLProjectile>(Class, Muzzle, Dir.Rotation(), Params);
	if (Shot)
	{
		Shot->Launch(Dir, Owner->GetVelocity());
	}
	return Shot;
}

void UBLWeaponComponent::FlashMuzzle(const FVector& MuzzleLocal)
{
	if (!MuzzleLight)
	{
		return;
	}
	MuzzleLight->SetRelativeLocation(MuzzleLocal);
	MuzzleLight->SetIntensity(MuzzleFlashIntensity);
	GetWorld()->GetTimerManager().SetTimer(
		MuzzleFlashTimer, this, &UBLWeaponComponent::EndMuzzleFlash, MuzzleFlashSeconds, false);
}

void UBLWeaponComponent::EndMuzzleFlash()
{
	if (MuzzleLight)
	{
		MuzzleLight->SetIntensity(0.f);
	}
}

USceneComponent* UBLWeaponComponent::FindHomingTarget() const
{
	AActor* Owner = GetOwner();
	const FVector From = Owner->GetActorLocation();
	const FVector Fwd = Owner->GetActorForwardVector();
	const float CosCone = FMath::Cos(FMath::DegreesToRadians(HomingConeHalfAngleDeg));

	USceneComponent* Best = nullptr;
	float BestDistSq = FMath::Square(HomingRange);

	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* Pawn = *It;
		if (Pawn == Owner || !Pawn->FindComponentByClass<UBLHealthComponent>())
		{
			continue;
		}
		const FVector To = Pawn->GetActorLocation() - From;
		const float DistSq = To.SizeSquared();
		if (DistSq > BestDistSq)
		{
			continue;
		}
		if (FVector::DotProduct(To.GetSafeNormal(), Fwd) < CosCone)
		{
			continue;  // outside the front cone
		}
		Best = Pawn->GetRootComponent();
		BestDistSq = DistSq;
	}
	return Best;
}
