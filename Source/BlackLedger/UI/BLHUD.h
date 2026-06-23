// Black Ledger - in-match HUD (slice scope: canvas-drawn; UMG dossier-styled
// version comes with the art pass). Layout per ui/HUD_Wireframe roles:
// player vitals bottom-left, special bottom-center, pickup bottom-right,
// boss bar top-center, victory/defeat overlays in ledger tone.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BLHUD.generated.h"

class ABLBossPawn;

UCLASS()
class BLACKLEDGER_API ABLHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	void DrawBar(float X, float Y, float W, float H, float Fraction, const FLinearColor& Fill);
	void DrawCenteredText(UFont* Font, const FString& Text, float CenterX, float Y,
		const FLinearColor& Color, float Scale = 1.f);

	ABLBossPawn* FindBoss() const;
};
