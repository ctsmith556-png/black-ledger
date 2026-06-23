// Black Ledger - in-match HUD

#include "BLHUD.h"
#include "Bosses/BLBossPawn.h"
#include "Core/BLGameMode.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "EngineUtils.h"
#include "Specials/BLSpecialComponent.h"
#include "Vehicles/BLCombatVehicle.h"
#include "Vehicles/BLHealthComponent.h"
#include "Weapons/BLWeaponComponent.h"

namespace
{
	const FLinearColor InkDark(0.02f, 0.02f, 0.022f, 0.78f);   // ledger paper-black
	const FLinearColor BloodRed(0.62f, 0.08f, 0.06f);
	const FLinearColor EmberOrange(1.f, 0.45f, 0.1f);
	const FLinearColor SurgicalTeal(0.35f, 0.95f, 0.88f);
}

void ABLHUD::DrawHUD()
{
	Super::DrawHUD();
	if (!Canvas)
	{
		return;
	}
	const float W = Canvas->SizeX;
	const float H = Canvas->SizeY;
	UFont* Big = GEngine->GetLargeFont();
	UFont* Mid = GEngine->GetMediumFont();

	const ABLCombatVehicle* V = Cast<ABLCombatVehicle>(GetOwningPawn());
	const ABLGameMode* GM = GetWorld()->GetAuthGameMode<ABLGameMode>();

	// ---- TM:B-style cluster (bottom-right): weapon rack + vertical health ----
	if (V && V->Health)
	{
		const float Frac = V->Health->GetHealth() / FMath::Max(V->Health->GetMaxHealth(), 1.f);

		// vertical health bar (classic green, draining top-down to amber/red)
		const float BarW = 30.f, BarH = 230.f;
		const float BarX = W - 76.f, BarY = H - 60.f - BarH;
		const FLinearColor HealthFill = Frac > 0.5f
			? FLinearColor(0.18f, 0.78f, 0.22f)
			: (Frac > 0.25f ? EmberOrange : BloodRed);
		DrawRect(InkDark, BarX - 3.f, BarY - 3.f, BarW + 6.f, BarH + 6.f);
		DrawRect(FLinearColor(0.10f, 0.10f, 0.09f), BarX, BarY, BarW, BarH);
		const float FillH = BarH * FMath::Clamp(Frac, 0.f, 1.f);
		DrawRect(HealthFill, BarX, BarY + (BarH - FillH), BarW, FillH);

		// weapon rack, right-aligned beside the bar
		if (V->Weapon)
		{
			const TArray<FBLWeaponSlot>& Rack = V->Weapon->GetInventory();
			const int32 Sel = V->Weapon->GetSelectedIndex();
			const float RackRight = BarX - 18.f;

			if (Rack.IsValidIndex(Sel))
			{
				// selected weapon: big, ember, with ammo count
				const FString Line = FString::Printf(TEXT("%s  x%02d"),
					*Rack[Sel].Name.ToString().ToUpper(), Rack[Sel].Ammo);
				DrawText(Line, EmberOrange,
					RackRight - Big->GetStringSize(*Line), H - 132.f, Big);

				// the rest of the rack, dimmed, stacked above (cycle: wheel / d-pad)
				float RowY = H - 162.f;
				for (int32 Offset = 1; Offset < Rack.Num() && Offset < 4; ++Offset)
				{
					const int32 i = (Sel + Offset) % Rack.Num();
					const FString Row = FString::Printf(TEXT("%s x%d"),
						*Rack[i].Name.ToString(), Rack[i].Ammo);
					DrawText(Row, FLinearColor(0.45f, 0.43f, 0.40f),
						RackRight - Mid->GetStringSize(*Row), RowY, Mid);
					RowY -= 22.f;
				}
			}
			else
			{
				const FString None = TEXT("- no pickups -");
				DrawText(None, FLinearColor(0.4f, 0.4f, 0.4f),
					RackRight - Mid->GetStringSize(*None), H - 128.f, Mid);
			}
		}

		// special lamp at the top of the cluster
		if (V->Special)
		{
			const bool bReady = V->Special->IsReady();
			const FString Lamp = bReady ? TEXT("OPERATING TABLE - READY [Q]")
				: TEXT("operating table - preparing");
			DrawText(Lamp, bReady ? SurgicalTeal : FLinearColor(0.4f, 0.4f, 0.4f),
				BarX - 18.f - Mid->GetStringSize(*Lamp), BarY - 26.f, Mid);
		}

		// low-health pulse: the screen itself starts to bleed
		if (Frac < 0.25f && Frac > 0.f)
		{
			const float Pulse = 0.10f + 0.07f * FMath::Sin(GetWorld()->GetTimeSeconds() * 6.f);
			DrawRect(FLinearColor(0.45f, 0.f, 0.f, Pulse), 0.f, 0.f, W, H);
		}
	}

	// ---- boss bar (top-center) ----
	if (const ABLBossPawn* Boss = FindBoss())
	{
		const float Frac = Boss->Health->GetHealth() / FMath::Max(Boss->Health->GetMaxHealth(), 1.f);
		const float BarW = FMath::Min(700.f, W * 0.5f);
		DrawCenteredText(Big, TEXT("THE FOUNDRYMAN  \"TAP\""), W * 0.5f, 36.f, BloodRed);
		DrawBar(W * 0.5f - BarW * 0.5f, 64.f, BarW, 16.f, Frac, BloodRed);
		FString Pips;
		for (int32 i = 1; i <= 3; ++i)
		{
			Pips += (Boss->GetPhase() >= i) ? TEXT("X ") : TEXT("- ");
		}
		DrawCenteredText(Mid, FString::Printf(TEXT("PHASE  %s"), *Pips), W * 0.5f, 86.f, EmberOrange);

		// weak-point window: tell the player to punish the exposed core
		if (Boss->IsCoreExposed())
		{
			const float Pulse = 0.55f + 0.45f * FMath::Sin(GetWorld()->GetTimeSeconds() * 10.f);
			DrawCenteredText(Big, TEXT("STRIKE THE CORE"), W * 0.5f, 110.f,
				FLinearColor(1.f, 0.85f, 0.3f, Pulse), 1.3f);
		}
	}

	// match-state overlays (Victory / Defeat / Unlock) now live in the Slate
	// result screens raised by UBLUISubsystem - the HUD only draws live combat.
	(void)GM;
}

void ABLHUD::DrawBar(float X, float Y, float BarW, float BarH, float Fraction, const FLinearColor& Fill)
{
	DrawRect(InkDark, X - 2.f, Y - 2.f, BarW + 4.f, BarH + 4.f);
	DrawRect(FLinearColor(0.12f, 0.11f, 0.10f), X, Y, BarW, BarH);
	DrawRect(Fill, X, Y, BarW * FMath::Clamp(Fraction, 0.f, 1.f), BarH);
}

void ABLHUD::DrawCenteredText(UFont* Font, const FString& Text, float CenterX, float Y,
	const FLinearColor& Color, float Scale)
{
	const float TextW = Font->GetStringSize(*Text) * Scale;
	DrawText(Text, Color, CenterX - TextW * 0.5f, Y, Font, Scale);
}

ABLBossPawn* ABLHUD::FindBoss() const
{
	for (TActorIterator<ABLBossPawn> It(GetWorld()); It; ++It)
	{
		if (It->Health && !It->Health->IsDead())
		{
			return *It;
		}
	}
	return nullptr;
}
