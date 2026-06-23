// Black Ledger - front-end UI manager (see header).

#include "BLUISubsystem.h"

#include "BLVehicleBios.h"
#include "Core/BLGameInstance.h"
#include "Core/BLSaveGame.h"

#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

#include "Brushes/SlateColorBrush.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateTypes.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"

// ----------------------------------------------------------------------------
// Dossier look - the same ledger palette the canvas HUD uses, expressed as
// persistent Slate brushes/fonts. Kept file-local; the UMG art pass will own the
// "real" styling later, but the flow above never has to change.
// ----------------------------------------------------------------------------
namespace BLMenu
{
	const FLinearColor Ink(0.020f, 0.020f, 0.025f, 0.97f);   // full-screen paper-black
	const FLinearColor Bone(0.86f, 0.84f, 0.78f);
	const FLinearColor Blood(0.62f, 0.08f, 0.06f);
	const FLinearColor Ember(1.00f, 0.45f, 0.10f);
	const FLinearColor Teal(0.35f, 0.95f, 0.88f);
	const FLinearColor Dim(0.42f, 0.40f, 0.37f);

	static const FSlateColorBrush InkBrush(Ink);
	static const FSlateColorBrush ClearBrush(FLinearColor(0.f, 0.f, 0.f, 0.f));   // gap spacer
	static const FSlateColorBrush BtnNormal(FLinearColor(0.10f, 0.095f, 0.090f, 1.f));
	static const FSlateColorBrush BtnHover(FLinearColor(0.22f, 0.075f, 0.055f, 1.f));
	static const FSlateColorBrush BtnPress(FLinearColor(0.32f, 0.105f, 0.065f, 1.f));

	static FSlateFontInfo Font(int32 Size, bool bBold = true)
	{
		return FCoreStyle::GetDefaultFontStyle(bBold ? "Bold" : "Regular", Size);
	}

	static const FButtonStyle& ButtonStyle()
	{
		static const FButtonStyle Style = FButtonStyle()
			.SetNormal(BtnNormal)
			.SetHovered(BtnHover)
			.SetPressed(BtnPress)
			.SetNormalPadding(FMargin(0.f))
			.SetPressedPadding(FMargin(0.f));
		return Style;
	}

	// a fixed-width ledger button wired to a plain TFunction action
	static TSharedRef<SWidget> Button(const FText& Label, TFunction<void()> OnClick,
		bool bEnabled = true, const FLinearColor& TextColor = Bone)
	{
		// SBorder carries the inter-button gap via .Padding (SBox has no Padding arg);
		// the transparent brush makes the border itself invisible.
		return SNew(SBorder)
			.BorderImage(&ClearBrush)
			.Padding(FMargin(0.f, 5.f))
			.HAlign(HAlign_Center)
			[
				SNew(SBox).WidthOverride(520.f)
				[
					SNew(SButton)
					.ButtonStyle(&ButtonStyle())
					.IsEnabled(bEnabled)
					.HAlign(HAlign_Center).VAlign(VAlign_Center)
					.ContentPadding(FMargin(28.f, 13.f))
					.OnClicked_Lambda([OnClick]()
					{
						if (OnClick) { OnClick(); }
						return FReply::Handled();
					})
					[
						SNew(STextBlock)
						.Text(Label)
						.Font(Font(22))
						.ColorAndOpacity(FSlateColor(bEnabled ? TextColor : Dim))
					]
				]
			];
	}

	static TSharedRef<SWidget> Label(const FText& Text, int32 Size,
		const FLinearColor& Color, bool bBold = true)
	{
		return SNew(STextBlock).Text(Text).Font(Font(Size, bBold))
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FSlateColor(Color));
	}

	// left-justified, auto-wrapping body copy (for dossier bios / storyboards)
	static TSharedRef<SWidget> Paragraph(const FText& Text, int32 Size,
		const FLinearColor& Color, float WrapAt)
	{
		return SNew(STextBlock).Text(Text).Font(Font(Size, false))
			.ColorAndOpacity(FSlateColor(Color))
			.AutoWrapText(true).WrapTextAt(WrapAt)
			.Justification(ETextJustify::Left);
	}

	// full-screen ink panel: title + optional subtitle, then a centered body
	static TSharedRef<SWidget> Frame(const FText& Title, const FLinearColor& TitleColor,
		const FText& Subtitle, TSharedRef<SWidget> Body)
	{
		TSharedRef<SVerticalBox> Col = SNew(SVerticalBox);
		if (!Title.IsEmpty())
		{
			Col->AddSlot().AutoHeight().HAlign(HAlign_Center).Padding(FMargin(0, 0, 0, 6))
				[ Label(Title, 46, TitleColor) ];
		}
		if (!Subtitle.IsEmpty())
		{
			Col->AddSlot().AutoHeight().HAlign(HAlign_Center).Padding(FMargin(0, 0, 0, 4))
				[ Label(Subtitle, 16, Dim, false) ];
		}
		Col->AddSlot().AutoHeight().HAlign(HAlign_Center).Padding(FMargin(0, 26, 0, 0))
			[ Body ];

		return SNew(SBorder)
			.BorderImage(&InkBrush)
			.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
			[
				SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
				[ Col ]
			];
	}
}

// ----------------------------------------------------------------------------
// Screens
// ----------------------------------------------------------------------------
TSharedRef<SWidget> UBLUISubsystem::BuildMainMenu()
{
	const bool bCanContinue = HasSaveProgress();
	return BLMenu::Frame(
		FText::FromString(TEXT("BLACK LEDGER")), BLMenu::Bone,
		FText::FromString(TEXT("THE AUDITOR IS NOW REVIEWING YOUR FILE")),
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[ BLMenu::Button(FText::FromString(TEXT("NEW GAME")),
				[this]() { ShowRoster(); }) ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[ BLMenu::Button(FText::FromString(TEXT("CONTINUE")),
				[this]() { ShowRoster(); }, bCanContinue) ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[ BLMenu::Button(FText::FromString(TEXT("ROSTER")),
				[this]() { ShowRoster(); }) ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[ BLMenu::Button(FText::FromString(TEXT("OPTIONS")),
				[this]() { ShowOptions(); }) ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[ BLMenu::Button(FText::FromString(TEXT("QUIT")),
				[this]() { QuitGame(); }) ]
	);
}

TSharedRef<SWidget> UBLUISubsystem::BuildRoster()
{
	// the full launch roster, every file selectable to open its dossier.
	// Only the Surgeon is actually drivable in this build (vehicle-select deferred).
	const int32 N = UE_ARRAY_COUNT(GBLLaunchBios);
	TSharedRef<SVerticalBox> ColA = SNew(SVerticalBox);
	TSharedRef<SVerticalBox> ColB = SNew(SVerticalBox);
	for (int32 i = 0; i < N; ++i)
	{
		const FBLVehicleBio* Bio = &GBLLaunchBios[i];
		const bool bPlayable = (FString(Bio->Key) == TEXT("Surgeon"));
		const FString Lbl = FString::Printf(TEXT("%s  %s%s"),
			Bio->Number, Bio->DisplayName, bPlayable ? TEXT("   [OPEN]") : TEXT(""));
		const TSharedRef<SVerticalBox>& Col = (i < (N + 1) / 2) ? ColA : ColB;
		Col->AddSlot().AutoHeight().HAlign(HAlign_Center)
			[ BLMenu::Button(FText::FromString(Lbl),
				[this, Bio]() { ShowBiography(Bio); }, true,
				bPlayable ? BLMenu::Teal : BLMenu::Bone) ];
	}

	// Collector bosses appear here only once the player has claimed them
	TSharedRef<SVerticalBox> BossBox = SNew(SVerticalBox);
	int32 BossCount = 0;
	for (const FBLVehicleBio& B : GBLBossBios)
	{
		if (!IsCollectorUnlocked(FName(B.Key)))
		{
			continue;
		}
		const FBLVehicleBio* Bio = &B;
		++BossCount;
		const FString Lbl = FString::Printf(TEXT("%s  %s   [CLAIMED]"), Bio->Number, Bio->DisplayName);
		BossBox->AddSlot().AutoHeight().HAlign(HAlign_Center)
			[ BLMenu::Button(FText::FromString(Lbl),
				[this, Bio]() { ShowBiography(Bio); }, true, BLMenu::Ember) ];
	}

	TSharedRef<SVerticalBox> Body = SNew(SVerticalBox);
	Body->AddSlot().AutoHeight().HAlign(HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().Padding(FMargin(8, 0))[ ColA ]
			+ SHorizontalBox::Slot().AutoWidth().Padding(FMargin(8, 0))[ ColB ]
		];
	if (BossCount > 0)
	{
		Body->AddSlot().AutoHeight().HAlign(HAlign_Center).Padding(FMargin(0, 14, 0, 4))
			[ BLMenu::Label(FText::FromString(TEXT("CLAIMED COLLECTORS")), 16, BLMenu::Ember) ];
		Body->AddSlot().AutoHeight().HAlign(HAlign_Center)[ BossBox ];
	}
	Body->AddSlot().AutoHeight().HAlign(HAlign_Center).Padding(FMargin(0, 18, 0, 0))
		[ BLMenu::Button(FText::FromString(TEXT("BACK")), [this]() { ShowMainMenu(); }) ];

	return BLMenu::Frame(FText::FromString(TEXT("THE FILING CABINET")), BLMenu::Bone,
		FText::FromString(TEXT("SELECT A CONTESTANT TO OPEN THEIR FILE")), Body);
}

TSharedRef<SWidget> UBLUISubsystem::BuildBiography(const FBLVehicleBio* Bio)
{
	const bool bPlayable = (FString(Bio->Key) == TEXT("Surgeon"));

	TSharedRef<SVerticalBox> Actions = SNew(SVerticalBox);
	Actions->AddSlot().AutoHeight().HAlign(HAlign_Center)
		[ BLMenu::Button(FText::FromString(TEXT("WATCH INTRO CINEMATIC")),
			[this, Bio]() { ShowIntro(Bio); }) ];
	if (bPlayable)
	{
		Actions->AddSlot().AutoHeight().HAlign(HAlign_Center)
			[ BLMenu::Button(FText::FromString(TEXT("DEPLOY  -  OPEN FILE")),
				[this, Bio]() { DeployContestant(Bio); }, true, BLMenu::Teal) ];
	}
	else
	{
		Actions->AddSlot().AutoHeight().HAlign(HAlign_Center).Padding(FMargin(0, 2, 0, 2))
			[ BLMenu::Label(FText::FromString(Bio->bBoss
				? TEXT("CLAIMED - drivable once vehicle-select is data-driven")
				: TEXT("SEALED - not yet drivable in this build")),
				14, Bio->bBoss ? BLMenu::Ember : BLMenu::Dim) ];
	}
	Actions->AddSlot().AutoHeight().HAlign(HAlign_Center)
		[ BLMenu::Button(FText::FromString(TEXT("BACK")), [this]() { ShowRoster(); }) ];

	TSharedRef<SVerticalBox> Body = SNew(SVerticalBox);
	Body->AddSlot().AutoHeight().HAlign(HAlign_Center).Padding(FMargin(0, 0, 0, 14))
		[ BLMenu::Label(FText::FromString(FString::Printf(TEXT("\"%s\""), Bio->Tagline)),
			17, BLMenu::Ember) ];
	Body->AddSlot().AutoHeight().HAlign(HAlign_Center)
		[
			SNew(SBox).WidthOverride(960.f).HeightOverride(420.f)
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
					[ BLMenu::Paragraph(FText::FromString(Bio->Bio), 16, BLMenu::Bone, 920.f) ]
			]
		];
	Body->AddSlot().AutoHeight().HAlign(HAlign_Center).Padding(FMargin(0, 18, 0, 0))[ Actions ];

	return BLMenu::Frame(FText::FromString(Bio->DisplayName),
		bPlayable ? BLMenu::Teal : (Bio->bBoss ? BLMenu::Ember : BLMenu::Bone),
		FText::FromString(TEXT("CASE FILE")), Body);
}

// Split a storyboard paragraph into motion-comic beats (one sentence each), so
// the cinematic can pace them like comic panels instead of a wall of text.
static TArray<FString> BLSplitBeats(const FString& Text)
{
	TArray<FString> Raw;
	Text.ParseIntoArray(Raw, TEXT(". "), true);
	TArray<FString> Beats;
	for (FString S : Raw)
	{
		S.TrimStartAndEndInline();
		if (S.IsEmpty())
		{
			continue;
		}
		// ParseIntoArray drops the delimiter period - put it back unless the beat
		// already ends in its own punctuation (quote, ?, !, ...).
		const TCHAR Last = S[S.Len() - 1];
		if (Last != TEXT('.') && Last != TEXT('!') && Last != TEXT('?')
			&& Last != TEXT('\'') && Last != TEXT('"'))
		{
			S += TEXT(".");
		}
		Beats.Add(S);
	}
	if (Beats.Num() == 0)
	{
		Beats.Add(Text);
	}
	return Beats;
}

TSharedRef<SWidget> UBLUISubsystem::BuildCinematic()
{
	const FString Beat = CineBeats.IsValidIndex(CineIndex) ? CineBeats[CineIndex] : FString();
	const bool bLast = (CineIndex + 1 >= CineBeats.Num());

	TSharedRef<SVerticalBox> Body = SNew(SVerticalBox);
	// the panel narration - large, centered, wrapped
	Body->AddSlot().AutoHeight().HAlign(HAlign_Center)
		[
			SNew(SBox).WidthOverride(1040.f)
			[
				SNew(STextBlock).Text(FText::FromString(Beat))
				.Font(BLMenu::Font(24, false))
				.ColorAndOpacity(FSlateColor(BLMenu::Bone))
				.AutoWrapText(true).WrapTextAt(1000.f)
				.Justification(ETextJustify::Center)
			]
		];
	// progress dots
	Body->AddSlot().AutoHeight().HAlign(HAlign_Center).Padding(FMargin(0, 22, 0, 0))
		[ BLMenu::Label(FText::FromString(FString::Printf(TEXT("%d / %d"),
			CineIndex + 1, CineBeats.Num())), 13, BLMenu::Dim, false) ];
	// controls
	Body->AddSlot().AutoHeight().HAlign(HAlign_Center).Padding(FMargin(0, 10, 0, 0))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()
				[ BLMenu::Button(FText::FromString(bLast ? TEXT("FINISH") : TEXT("NEXT >")),
					[this]() { CineNext(); }) ]
			+ SHorizontalBox::Slot().AutoWidth()
				[ BLMenu::Button(FText::FromString(TEXT("SKIP")),
					[this]() { CineFinish(); }, true, BLMenu::Dim) ]
		];

	return BLMenu::Frame(FText::GetEmpty(), BLMenu::Bone, FText::GetEmpty(), Body);
}

TSharedRef<SWidget> UBLUISubsystem::BuildOptions()
{
	const EBLDifficulty Cur = GetDifficulty();
	auto DiffBtn = [this, Cur](const FText& Label, EBLDifficulty D)
	{
		const bool bSel = (D == Cur);
		return BLMenu::Button(
			bSel ? FText::FromString(FString::Printf(TEXT("> %s <"), *Label.ToString())) : Label,
			[this, D]() { SetDifficulty(D); ShowOptions(); },
			true, bSel ? BLMenu::Ember : BLMenu::Bone);
	};

	return BLMenu::Frame(FText::FromString(TEXT("OPTIONS")), BLMenu::Bone,
		FText::FromString(TEXT("DIFFICULTY SCALES HP, FIRE RATE, AND AIM - NEVER THE TELLS")),
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[ DiffBtn(FText::FromString(TEXT("EASY")), EBLDifficulty::Easy) ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[ DiffBtn(FText::FromString(TEXT("MEDIUM")), EBLDifficulty::Medium) ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[ DiffBtn(FText::FromString(TEXT("HARD")), EBLDifficulty::Hard) ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(FMargin(0, 18, 0, 0))
			[ BLMenu::Button(FText::FromString(TEXT("BACK")), [this]() { ShowMainMenu(); }) ]
	);
}

TSharedRef<SWidget> UBLUISubsystem::BuildPause()
{
	return BLMenu::Frame(FText::FromString(TEXT("FILE SUSPENDED")), BLMenu::Bone,
		FText::FromString(TEXT("THE AUDITOR IS WATCHING")),
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[ BLMenu::Button(FText::FromString(TEXT("RESUME")), [this]() { TogglePause(); }) ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[ BLMenu::Button(FText::FromString(TEXT("RESTART")), [this]() { RetryMatch(); }) ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[ BLMenu::Button(FText::FromString(TEXT("QUIT TO MENU")), [this]() { QuitToMenu(); }) ]
	);
}

TSharedRef<SWidget> UBLUISubsystem::BuildResult(bool bVictory)
{
	if (bVictory)
	{
		return BLMenu::Frame(FText::FromString(TEXT("ACCOUNT SETTLED")), BLMenu::Bone,
			FText::FromString(TEXT("THE FOUNDRYMAN IS CLOSED")),
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
				[ BLMenu::Button(FText::FromString(TEXT("REPLAY")), [this]() { RetryMatch(); }) ]
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
				[ BLMenu::Button(FText::FromString(TEXT("RETURN TO THE LEDGER")), [this]() { QuitToMenu(); }) ]
		);
	}
	return BLMenu::Frame(FText::FromString(TEXT("ACCOUNT CLOSED")), BLMenu::Blood,
		FText::FromString(TEXT("THE FIELD CLAIMED YOU")),
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[ BLMenu::Button(FText::FromString(TEXT("REOPEN THE FILE")), [this]() { RetryMatch(); }) ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[ BLMenu::Button(FText::FromString(TEXT("QUIT TO MENU")), [this]() { QuitToMenu(); }) ]
	);
}

TSharedRef<SWidget> UBLUISubsystem::BuildUnlock()
{
	return BLMenu::Frame(FText::FromString(TEXT("COLLECTOR CLAIMED")), BLMenu::Ember,
		FText::FromString(TEXT("A NEW FILE ENTERS THE LEDGER")),
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(FMargin(0, 0, 0, 22))
			[ BLMenu::Label(FText::FromString(
				TEXT("THE FOUNDRYMAN - the kaiju of the Mill - is now part of your roster.")),
				18, BLMenu::Bone) ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[ BLMenu::Button(FText::FromString(TEXT("CONTINUE")),
				[this]()
				{
					// roll the deployed contestant's ending, then back to the menu
					const FBLVehicleBio* E = DeployedBio ? DeployedBio : &GBLLaunchBios[0];
					PlayCinematic(BLSplitBeats(E->Ending), [this]() { QuitToMenu(); });
				}) ]
	);
}

// ----------------------------------------------------------------------------
// Flow
// ----------------------------------------------------------------------------
void UBLUISubsystem::ShowMainMenu()
{
	// pull persisted difficulty into the live GameInstance so it actually applies
	if (UBLSaveGame* Save = Cast<UBLSaveGame>(
		UGameplayStatics::LoadGameFromSlot(UBLSaveGame::SlotName, 0)))
	{
		if (UBLGameInstance* GI = Cast<UBLGameInstance>(GetGameInstance()))
		{
			GI->SetDifficulty(Save->Difficulty);
		}
	}
	bPaused = false;
	SetScreen(BuildMainMenu(), true);
}

void UBLUISubsystem::ShowRoster()  { SetScreen(BuildRoster(), true); }
void UBLUISubsystem::ShowOptions() { SetScreen(BuildOptions(), true); }

void UBLUISubsystem::ShowBiography(const FBLVehicleBio* Bio)
{
	if (Bio) { SetScreen(BuildBiography(Bio), true); }
}

void UBLUISubsystem::ShowIntro(const FBLVehicleBio* Bio)
{
	// replay from the dossier: play the intro, then return to the same bio page
	if (Bio)
	{
		PlayCinematic(BLSplitBeats(Bio->Intro), [this, Bio]() { ShowBiography(Bio); });
	}
}

void UBLUISubsystem::DeployContestant(const FBLVehicleBio* Bio)
{
	// remember who we deployed (drives which ending plays) then: intro -> match
	DeployedBio = Bio;
	if (Bio)
	{
		PlayCinematic(BLSplitBeats(Bio->Intro), [this]() { StartMatch(); });
	}
	else
	{
		StartMatch();
	}
}

void UBLUISubsystem::PlayCinematic(const TArray<FString>& Beats, TFunction<void()> OnComplete)
{
	CineBeats = Beats;
	CineOnComplete = OnComplete;
	CineIndex = 0;
	if (CineBeats.Num() == 0)
	{
		CineFinish();
		return;
	}
	ShowCineBeat();
}

void UBLUISubsystem::ShowCineBeat()
{
	SetScreen(BuildCinematic(), true);
	// auto-advance, paced by beat length (readable but not draggy)
	if (UWorld* W = GetWorld())
	{
		const FString& Beat = CineBeats.IsValidIndex(CineIndex) ? CineBeats[CineIndex] : CineBeats.Last();
		const float Dur = FMath::Clamp(2.2f + Beat.Len() * 0.035f, 3.0f, 8.0f);
		W->GetTimerManager().SetTimer(CineTimer, this, &UBLUISubsystem::CineNext, Dur, false);
	}
}

void UBLUISubsystem::CineNext()
{
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(CineTimer);
	}
	++CineIndex;
	if (CineIndex >= CineBeats.Num())
	{
		CineFinish();
		return;
	}
	ShowCineBeat();
}

void UBLUISubsystem::CineFinish()
{
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(CineTimer);
	}
	// copy then clear, so a callback that starts another cinematic is safe
	TFunction<void()> Cb = MoveTemp(CineOnComplete);
	CineOnComplete = nullptr;
	CineBeats.Reset();
	if (Cb)
	{
		Cb();
	}
}

void UBLUISubsystem::ShowResult(bool bVictory, bool bNewUnlock)
{
	if (bVictory && bNewUnlock)
	{
		SetScreen(BuildUnlock(), true);
	}
	else
	{
		SetScreen(BuildResult(bVictory), true);
	}
}

void UBLUISubsystem::TogglePause()
{
	if (!bPaused)
	{
		bPaused = true;
		UGameplayStatics::SetGamePaused(this, true);
		SetScreen(BuildPause(), true);
	}
	else
	{
		bPaused = false;
		UGameplayStatics::SetGamePaused(this, false);
		ResumeGame();
	}
}

void UBLUISubsystem::StartMatch()
{
	UBLSaveGame* Save = LoadOrCreateSave();
	if (Save)
	{
		Save->bHasProgress = true;
		Save->LastCharacter = TEXT("Surgeon");
		CommitSave(Save);
	}
	bPaused = false;
	ClearScreen();
	UGameplayStatics::OpenLevel(this, FName(TEXT("L_Mill")));
}

void UBLUISubsystem::RetryMatch()
{
	bPaused = false;
	UGameplayStatics::SetGamePaused(this, false);
	ClearScreen();
	if (UWorld* W = GetWorld())
	{
		UGameplayStatics::OpenLevel(this, FName(*W->GetName()));
	}
}

void UBLUISubsystem::QuitToMenu()
{
	bPaused = false;
	UGameplayStatics::SetGamePaused(this, false);
	ClearScreen();
	UGameplayStatics::OpenLevel(this, FName(TEXT("L_MainMenu")));
}

void UBLUISubsystem::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, LocalPC(), EQuitPreference::Quit, false);
}

// ----------------------------------------------------------------------------
// Viewport + input plumbing
// ----------------------------------------------------------------------------
void UBLUISubsystem::SetScreen(TSharedRef<SWidget> NewScreen, bool bWantCursor)
{
	ClearScreen();
	if (UGameViewportClient* VP = GetGameInstance() ? GetGameInstance()->GetGameViewportClient() : nullptr)
	{
		VP->AddViewportWidgetContent(NewScreen, 100);
		CurrentScreen = NewScreen;
	}
	if (APlayerController* PC = LocalPC())
	{
		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(NewScreen);
		PC->SetInputMode(Mode);
		PC->bShowMouseCursor = bWantCursor;
	}
}

void UBLUISubsystem::ClearScreen()
{
	if (CurrentScreen.IsValid())
	{
		if (UGameViewportClient* VP = GetGameInstance() ? GetGameInstance()->GetGameViewportClient() : nullptr)
		{
			VP->RemoveViewportWidgetContent(CurrentScreen.ToSharedRef());
		}
		CurrentScreen.Reset();
	}
}

void UBLUISubsystem::ResumeGame()
{
	ClearScreen();
	if (APlayerController* PC = LocalPC())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
}

APlayerController* UBLUISubsystem::LocalPC() const
{
	return GetGameInstance() ? GetGameInstance()->GetFirstLocalPlayerController() : nullptr;
}

// ----------------------------------------------------------------------------
// Persistence
// ----------------------------------------------------------------------------
UBLSaveGame* UBLUISubsystem::LoadOrCreateSave() const
{
	UBLSaveGame* Save = Cast<UBLSaveGame>(
		UGameplayStatics::LoadGameFromSlot(UBLSaveGame::SlotName, 0));
	if (!Save)
	{
		Save = Cast<UBLSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UBLSaveGame::StaticClass()));
	}
	return Save;
}

void UBLUISubsystem::CommitSave(UBLSaveGame* Save) const
{
	if (Save)
	{
		UGameplayStatics::SaveGameToSlot(Save, UBLSaveGame::SlotName, 0);
	}
}

bool UBLUISubsystem::HasSaveProgress() const
{
	const UBLSaveGame* Save = Cast<UBLSaveGame>(
		UGameplayStatics::LoadGameFromSlot(UBLSaveGame::SlotName, 0));
	return Save && Save->bHasProgress;
}

bool UBLUISubsystem::IsCollectorUnlocked(FName CollectorName) const
{
	const UBLSaveGame* Save = Cast<UBLSaveGame>(
		UGameplayStatics::LoadGameFromSlot(UBLSaveGame::SlotName, 0));
	return Save && Save->UnlockedCollectors.Contains(CollectorName);
}

EBLDifficulty UBLUISubsystem::GetDifficulty() const
{
	if (const UBLGameInstance* GI = Cast<UBLGameInstance>(GetGameInstance()))
	{
		return GI->GetDifficulty();
	}
	return EBLDifficulty::Medium;
}

void UBLUISubsystem::SetDifficulty(EBLDifficulty NewDifficulty)
{
	if (UBLGameInstance* GI = Cast<UBLGameInstance>(GetGameInstance()))
	{
		GI->SetDifficulty(NewDifficulty);
	}
	UBLSaveGame* Save = LoadOrCreateSave();
	if (Save)
	{
		Save->Difficulty = NewDifficulty;
		CommitSave(Save);
	}
}
