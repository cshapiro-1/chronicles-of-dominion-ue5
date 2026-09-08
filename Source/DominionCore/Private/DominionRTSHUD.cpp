#include "DominionRTSHUD.h"
#include "DominionRTSPlayerController.h"
#include "DominionUnitActor.h"
#include "DominionBuildingActor.h"
#include "DominionTutorialSubsystem.h"
#include "DominionSupplyLineSubsystem.h"
#include "DominionFormationSystem.h"
#include "DominionPoliticalEstatesSystem.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Kismet/GameplayStatics.h"

ADominionRTSHUD::ADominionRTSHUD()
{
}

void ADominionRTSHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ADominionRTSHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas) return;

	// Helper: Draw high-contrast text with drop shadow for 100% legibility
	auto DrawShadowText = [this](UFont* Font, const FString& Text, float X, float Y, float Scale, const FLinearColor& TextColor)
	{
		if (!Font) Font = GEngine->GetSmallFont();
		// Drop shadow (offset by 1.5px)
		Canvas->DrawColor = FColor(0, 0, 0, 240);
		Canvas->DrawText(Font, Text, X + 1.5f, Y + 1.5f, Scale, Scale, FFontRenderInfo());
		// Main text
		Canvas->DrawColor = TextColor.ToFColor(true);
		Canvas->DrawText(Font, Text, X, Y, Scale, Scale, FFontRenderInfo());
	};

	UFont* DefaultFont = GEngine->GetSmallFont();
	UFont* MediumFont = GEngine->GetMediumFont() ? GEngine->GetMediumFont() : DefaultFont;

	// 1. Draw Top Resource & Imperial Grand Strategy Header Bar
	const float ScreenW = Canvas->SizeX;
	const float ScreenH = Canvas->SizeY;
	const float TopBarH = 52.0f;

	// Top Bar Dark Weathered Slate Translucent Background
	FCanvasTileItem TopBarBG(FVector2D(0, 0), FVector2D(ScreenW, TopBarH), FLinearColor(0.03f, 0.04f, 0.06f, 0.97f));
	TopBarBG.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TopBarBG);

	// Top Bar Gold Border Line
	Canvas->K2_DrawLine(FVector2D(0, TopBarH), FVector2D(ScreenW, TopBarH), 2.5f, FLinearColor(0.92f, 0.72f, 0.22f, 1.0f));

	// Group 1: Core Physical Commodities (Left)
	struct FResourceBadge
	{
		FString Label;
		FLinearColor Color;
	};

	const FResourceBadge ResBadges[] = {
		{ TEXT("GRAIN: 1,420 (+45/m)"), FLinearColor(1.0f, 0.90f, 0.40f) },
		{ TEXT("TIMBER: 850 (+18/m)"), FLinearColor(0.85f, 0.65f, 0.40f) },
		{ TEXT("STONE: 1,200 (+25/m)"), FLinearColor(0.80f, 0.85f, 0.90f) },
		{ TEXT("BRONZE: 650 (+12/m)"), FLinearColor(0.95f, 0.65f, 0.25f) },
		{ TEXT("GOLD: 5,200 (+80/m)"), FLinearColor(1.0f, 0.84f, 0.0f) }
	};

	float ResX = 14.0f;
	for (const FResourceBadge& Badge : ResBadges)
	{
		float BadgeW = Badge.Label.Len() * 7.4f + 14.0f;
		FCanvasTileItem BadgeBG(FVector2D(ResX, 8.0f), FVector2D(BadgeW, 36.0f), FLinearColor(0.07f, 0.09f, 0.13f, 0.88f));
		Canvas->DrawItem(BadgeBG);

		FCanvasBoxItem BadgeBorder(FVector2D(ResX, 8.0f), FVector2D(BadgeW, 36.0f));
		BadgeBorder.SetColor(Badge.Color * 0.6f);
		BadgeBorder.LineThickness = 1.0f;
		Canvas->DrawItem(BadgeBorder);

		DrawShadowText(DefaultFont, Badge.Label, ResX + 7.0f, 17.0f, 0.88f, Badge.Color);
		ResX += BadgeW + 6.0f;
	}

	// Group 2: Demographics & Dynamic Supply Line (Center-Left)
	const FString ManpowerText = TEXT("MANPOWER: 6,500 / 15,000");
	float ManpowerW = ManpowerText.Len() * 7.4f + 14.0f;
	FCanvasTileItem ManpowerBG(FVector2D(ResX, 8.0f), FVector2D(ManpowerW, 36.0f), FLinearColor(0.07f, 0.11f, 0.15f, 0.88f));
	Canvas->DrawItem(ManpowerBG);
	FCanvasBoxItem ManpowerBorder(FVector2D(ResX, 8.0f), FVector2D(ManpowerW, 36.0f));
	ManpowerBorder.SetColor(FLinearColor(0.35f, 0.65f, 0.95f, 0.8f));
	Canvas->DrawItem(ManpowerBorder);
	DrawShadowText(DefaultFont, ManpowerText, ResX + 7.0f, 17.0f, 0.88f, FLinearColor(0.65f, 0.88f, 1.0f));
	ResX += ManpowerW + 6.0f;

	// Dynamic Logistics & Baggage Train Status Badge
	UDominionSupplyLineSubsystem* SupplySys = GetWorld() ? GetWorld()->GetSubsystem<UDominionSupplyLineSubsystem>() : nullptr;
	bool bSupplyActive = SupplySys ? SupplySys->IsSupplyLineActive() : true;
	FString LogisticsBadge = bSupplyActive ? TEXT("SUPPLY: 100% [42 Days] [T]") : TEXT("SUPPLY: SEVERED [T]");
	FLinearColor SupplyCol = bSupplyActive ? FLinearColor(0.25f, 0.95f, 0.45f, 1.0f) : FLinearColor(1.0f, 0.25f, 0.15f, 1.0f);

	float LogW = LogisticsBadge.Len() * 7.4f + 14.0f;
	FCanvasTileItem LogBG(FVector2D(ResX, 8.0f), FVector2D(LogW, 36.0f), FLinearColor(0.07f, 0.09f, 0.13f, 0.88f));
	Canvas->DrawItem(LogBG);
	FCanvasBoxItem LogBorder(FVector2D(ResX, 8.0f), FVector2D(LogW, 36.0f));
	LogBorder.SetColor(SupplyCol);
	LogBorder.LineThickness = 1.2f;
	Canvas->DrawItem(LogBorder);
	DrawShadowText(DefaultFont, LogisticsBadge, ResX + 7.0f, 17.0f, 0.88f, SupplyCol);
	ResX += LogW + 12.0f;

	// --- Group 3: 3-ESTATE POWER TRIAD & FROSTPUNK HOPE/DISCONTENT GAUGES ---
	UDominionPoliticalEstatesSystem* Estates = GetWorld() ? GetWorld()->GetSubsystem<UDominionPoliticalEstatesSystem>() : nullptr;
	if (Estates)
	{
		// Hope vs Discontent Dual Gauge
		FString HopeDiscText = FString::Printf(TEXT("HOPE: %.0f%%  vs  DISCONTENT: %.0f%%"), Estates->GetHope(), Estates->GetDiscontent());
		float HDWidth = HopeDiscText.Len() * 7.5f + 16.0f;
		FCanvasTileItem HDBG(FVector2D(ResX, 8.0f), FVector2D(HDWidth, 36.0f), FLinearColor(0.06f, 0.08f, 0.12f, 0.92f));
		Canvas->DrawItem(HDBG);
		FCanvasBoxItem HDBorder(FVector2D(ResX, 8.0f), FVector2D(HDWidth, 36.0f));
		HDBorder.SetColor(FLinearColor(0.92f, 0.72f, 0.22f, 0.75f));
		Canvas->DrawItem(HDBorder);
		DrawShadowText(DefaultFont, HopeDiscText, ResX + 8.0f, 17.0f, 0.88f, FLinearColor(0.35f, 0.90f, 1.0f));
		ResX += HDWidth + 8.0f;

		// 3 Estates Badges
		struct FEstateDisplay
		{
			FString Label;
			float Value;
			FLinearColor Color;
		};

		FEstateDisplay EstateBadges[3] = {
			{ FString::Printf(TEXT("ALTAR: %.0f%%"), Estates->GetPriesthoodLoyalty()), Estates->GetPriesthoodLoyalty(), FLinearColor(1.0f, 0.88f, 0.25f) },
			{ FString::Printf(TEXT("THRONE: %.0f%%"), Estates->GetNobilityLoyalty()), Estates->GetNobilityLoyalty(), FLinearColor(0.95f, 0.40f, 0.40f) },
			{ FString::Printf(TEXT("MASSES: %.0f%%"), Estates->GetMassesLoyalty()), Estates->GetMassesLoyalty(), FLinearColor(0.35f, 0.95f, 0.55f) }
		};

		for (const FEstateDisplay& Est : EstateBadges)
		{
			float EstW = Est.Label.Len() * 7.4f + 12.0f;
			FCanvasTileItem EstBG(FVector2D(ResX, 8.0f), FVector2D(EstW, 36.0f), FLinearColor(0.08f, 0.10f, 0.14f, 0.88f));
			Canvas->DrawItem(EstBG);
			FCanvasBoxItem EstBorder(FVector2D(ResX, 8.0f), FVector2D(EstW, 36.0f));
			EstBorder.SetColor(Est.Color * 0.7f);
			Canvas->DrawItem(EstBorder);
			DrawShadowText(DefaultFont, Est.Label, ResX + 6.0f, 17.0f, 0.86f, Est.Color);
			ResX += EstW + 5.0f;
		}
	}

	// Epoch Badge (Right Aligned)
	const FString EpochText = TEXT("EPOCH I: BRONZE AGE");
	const float EpochW = 190.0f;
	const float EpochX = ScreenW - EpochW - 14.0f;
	FCanvasTileItem EpochBG(FVector2D(EpochX, 8.0f), FVector2D(EpochW, 36.0f), FLinearColor(0.08f, 0.12f, 0.20f, 0.9f));
	Canvas->DrawItem(EpochBG);
	FCanvasBoxItem EpochBorder(FVector2D(EpochX, 8.0f), FVector2D(EpochW, 36.0f));
	EpochBorder.SetColor(FLinearColor(0.92f, 0.72f, 0.22f, 0.8f));
	Canvas->DrawItem(EpochBorder);
	DrawShadowText(MediumFont, EpochText, EpochX + 10.0f, 15.0f, 0.88f, FLinearColor(0.45f, 0.88f, 1.0f, 1.0f));

	// Active Imperial Edict Banner under Top Header
	if (Estates && Estates->GetActiveEdictRemainingTime() > 0.0f)
	{
		FString EdictMsg = FString::Printf(TEXT("[ACTIVE IMPERIAL EDICT]: %s (%.0fs remaining)"), *Estates->GetActiveEdictName(), Estates->GetActiveEdictRemainingTime());
		const float EdictW = 680.0f;
		const float EdictX = (ScreenW - EdictW) * 0.5f;
		FCanvasTileItem EdictBG(FVector2D(EdictX, TopBarH + 6.0f), FVector2D(EdictW, 28.0f), FLinearColor(0.05f, 0.12f, 0.18f, 0.95f));
		Canvas->DrawItem(EdictBG);
		FCanvasBoxItem EdictBorder(FVector2D(EdictX, TopBarH + 6.0f), FVector2D(EdictW, 28.0f));
		EdictBorder.SetColor(FLinearColor(0.3f, 0.85f, 1.0f));
		Canvas->DrawItem(EdictBorder);
		DrawShadowText(DefaultFont, EdictMsg, EdictX + 16.0f, TopBarH + 11.0f, 0.95f, FLinearColor(1.0f, 0.95f, 0.6f));
	}

	// Major Intra-Civ Crisis Alert Popup Banner
	if (Estates && Estates->GetCrisisDisplayTimer() > 0.0f)
	{
		const float CrisisW = 840.0f;
		const float CrisisH = 64.0f;
		const float CrisisX = (ScreenW - CrisisW) * 0.5f;
		const float CrisisY = TopBarH + 42.0f;

		FCanvasTileItem CrisisBG(FVector2D(CrisisX, CrisisY), FVector2D(CrisisW, CrisisH), FLinearColor(0.35f, 0.02f, 0.02f, 0.96f));
		Canvas->DrawItem(CrisisBG);

		FCanvasBoxItem CrisisBorder(FVector2D(CrisisX, CrisisY), FVector2D(CrisisW, CrisisH));
		CrisisBorder.SetColor(FLinearColor(1.0f, 0.2f, 0.2f));
		CrisisBorder.LineThickness = 2.0f;
		Canvas->DrawItem(CrisisBorder);

		FString CrisisMsg = Estates->GetRecentCrisisNotification().IsEmpty() ? TEXT("[!] IMPERIAL CRISIS TRIGGERED") : Estates->GetRecentCrisisNotification().ToUpper();
		DrawShadowText(MediumFont, CrisisMsg, CrisisX + 24.0f, CrisisY + 12.0f, 1.05f, FLinearColor(1.0f, 0.95f, 0.4f));
		DrawShadowText(DefaultFont, TEXT("Press [F1-F4] to enact Imperial Edicts or appease the striking estate!"), CrisisX + 24.0f, CrisisY + 38.0f, 0.95f, FLinearColor(1.0f, 0.85f, 0.85f));
	}

	// 2. Draw Bottom Command Console (AoE2 / Frostpunk Dark Bronze Frame)
	const float ConsoleH = 210.0f;
	const float ConsoleY = ScreenH - ConsoleH;

	// Dark Hewn Stone & Bronze Console Frame
	FCanvasTileItem ConsoleBG(FVector2D(0, ConsoleY), FVector2D(ScreenW, ConsoleH), FLinearColor(0.05f, 0.04f, 0.03f, 0.98f));
	ConsoleBG.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(ConsoleBG);

	// Heavy Carved Brass Top Border
	Canvas->K2_DrawLine(FVector2D(0, ConsoleY), FVector2D(ScreenW, ConsoleY), 3.5f, FLinearColor(0.85f, 0.65f, 0.22f, 1.0f));
	Canvas->K2_DrawLine(FVector2D(0, ConsoleY + 3.0f), FVector2D(ScreenW, ConsoleY + 3.0f), 1.0f, FLinearColor(0.25f, 0.18f, 0.08f, 0.9f));

	// --- SECTION 1 (LEFT): 3D UNIT PORTRAIT & ATTRIBUTES CARD ---
	const float CardX = 24.0f;
	const float CardY = ConsoleY + 16.0f;
	const float CardW = 340.0f;
	const float CardH = ConsoleH - 32.0f;

	FCanvasTileItem CardBG(FVector2D(CardX, CardY), FVector2D(CardW, CardH), FLinearColor(0.09f, 0.07f, 0.05f, 0.95f));
	Canvas->DrawItem(CardBG);

	FCanvasBoxItem CardBorder(FVector2D(CardX, CardY), FVector2D(CardW, CardH));
	CardBorder.SetColor(FLinearColor(0.75f, 0.58f, 0.22f, 0.9f));
	CardBorder.LineThickness = 2.0f;
	Canvas->DrawItem(CardBorder);

	ADominionRTSPlayerController* PC = Cast<ADominionRTSPlayerController>(GetOwningPlayerController());
	if (PC && PC->GetSelectedUnits().Num() > 0)
	{
		ADominionUnitActor* SelUnit = PC->GetSelectedUnits()[0];
		if (IsValid(SelUnit))
		{
			// Portrait Box
			const float PortW = 80.0f;
			const float PortH = CardH - 24.0f;
			FCanvasTileItem PortBG(FVector2D(CardX + 12.0f, CardY + 12.0f), FVector2D(PortW, PortH), FLinearColor(0.14f, 0.11f, 0.08f));
			Canvas->DrawItem(PortBG);
			FCanvasBoxItem PortBorder(FVector2D(CardX + 12.0f, CardY + 12.0f), FVector2D(PortW, PortH));
			PortBorder.SetColor(FLinearColor(0.92f, 0.72f, 0.22f));
			Canvas->DrawItem(PortBorder);

			DrawShadowText(MediumFont, TEXT("⚔️"), CardX + 38.0f, CardY + 60.0f, 1.4f, FLinearColor(1.0f, 0.85f, 0.2f));

			// Unit Info Text
			const float TextX = CardX + PortW + 20.0f;
			DrawShadowText(MediumFont, SelUnit->UnitName.ToUpper(), TextX, CardY + 10.0f, 1.02f, FLinearColor::White);
			FString SubTitle = FString::Printf(TEXT("Rank: Veteran  |  Squad: %d"), PC->GetSelectedUnits().Num());
			DrawShadowText(DefaultFont, SubTitle, TextX, CardY + 30.0f, 0.82f, FLinearColor(0.75f, 0.85f, 0.95f));

			// Health Bar
			float HPPercent = FMath::Clamp(SelUnit->Health / SelUnit->MaxHealth, 0.0f, 1.0f);
			const float BarW = 200.0f;
			FCanvasTileItem HPBG(FVector2D(TextX, CardY + 50.0f), FVector2D(BarW, 11.0f), FLinearColor(0.15f, 0.15f, 0.15f));
			Canvas->DrawItem(HPBG);
			FCanvasTileItem HPFill(FVector2D(TextX, CardY + 50.0f), FVector2D(BarW * HPPercent, 11.0f), FLinearColor(0.2f, 0.85f, 0.35f));
			Canvas->DrawItem(HPFill);
			DrawShadowText(DefaultFont, FString::Printf(TEXT("HP: %.0f / %.0f"), SelUnit->Health, SelUnit->MaxHealth), TextX + 4.0f, CardY + 50.0f, 0.70f, FLinearColor::White);

			// Morale Bar
			FCanvasTileItem MoraleBG(FVector2D(TextX, CardY + 66.0f), FVector2D(BarW, 11.0f), FLinearColor(0.15f, 0.15f, 0.15f));
			Canvas->DrawItem(MoraleBG);
			FCanvasTileItem MoraleFill(FVector2D(TextX, CardY + 66.0f), FVector2D(BarW * 0.95f, 11.0f), FLinearColor(0.2f, 0.65f, 1.0f));
			Canvas->DrawItem(MoraleFill);
			DrawShadowText(DefaultFont, TEXT("MORALE: 95% (Steady)"), TextX + 4.0f, CardY + 66.0f, 0.70f, FLinearColor::White);

			// Stats: ATT / DEF / SPEED
			FString StatStr = FString::Printf(TEXT("ATT: %.0f   •   DEF: 35   •   SPD: %.0f"), SelUnit->AttackPower, SelUnit->MoveSpeed);
			DrawShadowText(DefaultFont, StatStr, TextX, CardY + 86.0f, 0.80f, FLinearColor(1.0f, 0.88f, 0.45f));

			// Stance description & Supply Days
			FString StanceStr = SelUnit->bInShieldWall ? TEXT("Stance: Phalanx (+40% Armor)") : TEXT("Stance: Open Order");
			DrawShadowText(DefaultFont, StanceStr, TextX, CardY + 106.0f, 0.78f, FLinearColor(0.65f, 0.85f, 1.0f));

			FString SupplyStr = TEXT("Supply Reserve: 42 Days Food");
			DrawShadowText(DefaultFont, SupplyStr, TextX, CardY + 124.0f, 0.75f, FLinearColor(0.55f, 0.95f, 0.55f));
		}
	}
	else
	{
		DrawShadowText(MediumFont, TEXT("IMPERIAL COMMAND"), CardX + 20.0f, CardY + 24.0f, 1.05f, FLinearColor(1.0f, 0.85f, 0.2f));
		DrawShadowText(DefaultFont, TEXT("Select a Legion regiment or building"), CardX + 20.0f, CardY + 54.0f, 0.90f, FLinearColor::White);
		DrawShadowText(DefaultFont, TEXT("WASD: Pan   •   Q/E: Rotate"), CardX + 20.0f, CardY + 84.0f, 0.85f, FLinearColor(0.75f, 0.75f, 0.75f));
	}

	// --- SECTION 2 (CENTER): FORMATIONS & EDICTS COMMAND GRID ---
	const float GridLeft = CardX + CardW + 28.0f;
	const float GridTop = ConsoleY + 16.0f;

	// Formations Header
	DrawShadowText(DefaultFont, TEXT("TACTICAL FORMATIONS"), GridLeft, GridTop + 2.0f, 0.85f, FLinearColor(1.0f, 0.85f, 0.25f));

	struct FCmdBtn { const TCHAR* Key; const TCHAR* Label; const TCHAR* Icon; };
	const FCmdBtn Formations[4] = {
		{ TEXT("[1]"), TEXT("LINE"), TEXT("━") },
		{ TEXT("[2]"), TEXT("SQUARE"), TEXT("⧈") },
		{ TEXT("[3]"), TEXT("PHALANX"), TEXT("🛡️") },
		{ TEXT("[4]"), TEXT("SKIRMISH"), TEXT("⚔️") }
	};

	const float BtnW = 75.0f;
	const float BtnH = 55.0f;
	for (int32 i = 0; i < 4; ++i)
	{
		FVector2D Pos(GridLeft + i * (BtnW + 8.0f), GridTop + 20.0f);
		FCanvasTileItem BtnBG(Pos, FVector2D(BtnW, BtnH), FLinearColor(0.12f, 0.09f, 0.07f, 0.95f));
		Canvas->DrawItem(BtnBG);
		FCanvasBoxItem BtnBorder(Pos, FVector2D(BtnW, BtnH));
		BtnBorder.SetColor(FLinearColor(0.85f, 0.65f, 0.22f, 0.85f));
		Canvas->DrawItem(BtnBorder);
		DrawShadowText(DefaultFont, Formations[i].Key, Pos.X + 8.0f, Pos.Y + 6.0f, 0.80f, FLinearColor(1.0f, 0.85f, 0.2f));
		DrawShadowText(DefaultFont, Formations[i].Label, Pos.X + 8.0f, Pos.Y + 28.0f, 0.82f, FLinearColor::White);
	}

	// Imperial Edicts Header
	const float EdictTop = GridTop + 85.0f;
	DrawShadowText(DefaultFont, TEXT("IMPERIAL EDICT LAWS"), GridLeft, EdictTop + 2.0f, 0.85f, FLinearColor(0.45f, 0.88f, 1.0f));

	const FCmdBtn Edicts[4] = {
		{ TEXT("[F1]"), TEXT("TITHE"), TEXT("📜") },
		{ TEXT("[F2]"), TEXT("CORVEE"), TEXT("🔨") },
		{ TEXT("[F3]"), TEXT("DOLE"), TEXT("🍞") },
		{ TEXT("[F4]"), TEXT("GAMES"), TEXT("🏟️") }
	};

	for (int32 i = 0; i < 4; ++i)
	{
		FVector2D Pos(GridLeft + i * (BtnW + 8.0f), EdictTop + 20.0f);
		FCanvasTileItem BtnBG(Pos, FVector2D(BtnW, BtnH), FLinearColor(0.08f, 0.10f, 0.14f, 0.95f));
		Canvas->DrawItem(BtnBG);
		FCanvasBoxItem BtnBorder(Pos, FVector2D(BtnW, BtnH));
		BtnBorder.SetColor(FLinearColor(0.35f, 0.75f, 1.0f, 0.85f));
		Canvas->DrawItem(BtnBorder);
		DrawShadowText(DefaultFont, Edicts[i].Key, Pos.X + 8.0f, Pos.Y + 6.0f, 0.80f, FLinearColor(0.45f, 0.88f, 1.0f));
		DrawShadowText(DefaultFont, Edicts[i].Label, Pos.X + 8.0f, Pos.Y + 28.0f, 0.82f, FLinearColor::White);
	}

	// --- SECTION 3 (RIGHT): CIRCULAR BRASS COMPASS MINIMAP ---
	const float MapDiameter = 180.0f;
	const float MapX = ScreenW - MapDiameter - 35.0f;
	const float MapY = ConsoleY + 15.0f;

	// Circular Brass Compass Backing
	FCanvasTileItem MapBG(FVector2D(MapX, MapY), FVector2D(MapDiameter, MapDiameter), FLinearColor(0.06f, 0.08f, 0.12f, 1.0f));
	Canvas->DrawItem(MapBG);

	FCanvasBoxItem MapBorder(FVector2D(MapX, MapY), FVector2D(MapDiameter, MapDiameter));
	MapBorder.SetColor(FLinearColor(0.85f, 0.65f, 0.22f, 1.0f));
	MapBorder.LineThickness = 2.5f;
	Canvas->DrawItem(MapBorder);

	// Cardinal Compass Points (N, S, E, W)
	DrawShadowText(MediumFont, TEXT("N"), MapX + (MapDiameter * 0.5f) - 6.0f, MapY + 4.0f, 0.95f, FLinearColor(1.0f, 0.85f, 0.2f));
	DrawShadowText(MediumFont, TEXT("S"), MapX + (MapDiameter * 0.5f) - 6.0f, MapY + MapDiameter - 20.0f, 0.95f, FLinearColor(1.0f, 0.85f, 0.2f));
	DrawShadowText(MediumFont, TEXT("W"), MapX + 4.0f, MapY + (MapDiameter * 0.5f) - 10.0f, 0.95f, FLinearColor(1.0f, 0.85f, 0.2f));
	DrawShadowText(MediumFont, TEXT("E"), MapX + MapDiameter - 18.0f, MapY + (MapDiameter * 0.5f) - 10.0f, 0.95f, FLinearColor(1.0f, 0.85f, 0.2f));

	// River Canal Curve on Radar
	Canvas->K2_DrawLine(FVector2D(MapX + 30, MapY + 150), FVector2D(MapX + 100, MapY + 80), 3.0f, FLinearColor(0.15f, 0.45f, 0.85f, 0.8f));
	Canvas->K2_DrawLine(FVector2D(MapX + 100, MapY + 80), FVector2D(MapX + 160, MapY + 30), 3.0f, FLinearColor(0.15f, 0.45f, 0.85f, 0.8f));

	// Real-Time Tactical Troop Radar Blips
	TArray<AActor*> AllRadarUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADominionUnitActor::StaticClass(), AllRadarUnits);
	for (AActor* Act : AllRadarUnits)
	{
		if (ADominionUnitActor* Unit = Cast<ADominionUnitActor>(Act))
		{
			FVector UnitLoc = Unit->GetActorLocation();
			float BlipX = MapX + (MapDiameter * 0.5f) + (UnitLoc.X / 110.0f);
			float BlipY = MapY + (MapDiameter * 0.5f) + (UnitLoc.Y / 110.0f);
			BlipX = FMath::Clamp(BlipX, MapX + 12.0f, MapX + MapDiameter - 18.0f);
			BlipY = FMath::Clamp(BlipY, MapY + 12.0f, MapY + MapDiameter - 18.0f);

			FLinearColor BlipCol = (Unit->TeamID == 0) ? FLinearColor(0.2f, 0.7f, 1.0f) : FLinearColor(1.0f, 0.2f, 0.2f);
			FCanvasTileItem Blip(FVector2D(BlipX, BlipY), FVector2D(6.0f, 6.0f), BlipCol);
			Canvas->DrawItem(Blip);
		}
	}
}

void ADominionRTSHUD::DrawSelectionMarquee()
{
}

void ADominionRTSHUD::DrawMinimapOverlay()
{
}

void ADominionRTSHUD::SetCommandGridActions(const TArray<FDominionCommandAction>& Actions)
{
	CurrentActions = Actions;
}

bool ADominionRTSHUD::ExecuteHotkeyAction(const FString& Hotkey)
{
	return true;
}
