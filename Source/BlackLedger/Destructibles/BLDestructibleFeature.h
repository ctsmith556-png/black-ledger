// Black Ledger - destructible map feature base (TDD section 6)
// Has health (so every weapon damages it with zero special-casing); on death it
// (1) plays the spectacle via the FX subsystem, (2) swaps pre-authored "after"
// geometry that is NAVIGABLE, (3) lets the dynamic navmesh re-knit (collision
// toggles dirty the tiles automatically). Chaos break spectacle replaces the
// greybox swap later - the navigable result is always the authored swap.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BLDestructibleFeature.generated.h"

class UBLHealthComponent;
class UStaticMeshComponent;

UCLASS(Abstract)
class BLACKLEDGER_API ABLDestructibleFeature : public AActor
{
	GENERATED_BODY()

public:
	ABLDestructibleFeature();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BL|Destructible")
	TObjectPtr<UBLHealthComponent> Health;

	UFUNCTION(BlueprintPure, Category = "BL|Destructible")
	bool IsBroken() const { return bBroken; }

protected:
	virtual void BeginPlay() override;

	/** Subclass hook for extra collapse behavior (hazard triggers, audio, etc.). */
	virtual void OnCollapsed() {}

	/** Constructor helper: greybox cube piece. bAfterState pieces start hidden. */
	UStaticMeshComponent* MakePiece(const TCHAR* Name, const FVector& RelLoc,
		const FVector& Scale, const FRotator& RelRot, bool bAfterState, bool bPermanent = false);

	UFUNCTION()
	void HandleBroken();

	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> BeforePieces;

	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> AfterPieces;

private:
	static void SetPieceActive(UStaticMeshComponent* Piece, bool bActive);

	bool bBroken = false;
};
