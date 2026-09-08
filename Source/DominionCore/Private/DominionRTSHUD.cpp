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
	const float TopBarH = 54.0f;

	// Top Bar Dark Weathered Slate Background
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
		float BadgeW = Badge.Label.Len() * 7.2f + 12.0f;
		FCanvasTileItem BadgeBG(FVector2D(ResX, 8.0f), FVector2D(BadgeW, 36.0f), FLinearColor(0.07f, 0.09f, 0.13f, 0.88f));
		Canvas->DrawItem(BadgeBG);

		FCanvasBoxItem BadgeBorder(FVector2D(ResX, 8.0f), FVector2D(BadgeW, 36.0f));
		BadgeBorder.SetColor(Badge.Color * 0.6f);
		BadgeBorder.LineThickness = 1.0f;
		Canvas->DrawItem(BadgeBorder);

		DrawShadowText(DefaultFont, Badge.Label, ResX + 6.0f, 18.0f, 0.86f, Badge.Color);
		ResX += BadgeW + 5.0f;
	}

	// Group 2: Demographics & Dynamic Supply Line (Center-Left)
	const FString ManpowerText = TEXT("MANPOWER: 6,500/15k");
	float ManpowerW = ManpowerText.Len() * 7.2f + 12.0f;
	FCanvasTileItem ManpowerBG(FVector2D(ResX, 8.0f), FVector2D(ManpowerW, 36.0f), FLinearColor(0.07f, 0.11f, 0.15f, 0.88f));
	Canvas->DrawItem(ManpowerBG);
	FCanvasBoxItem ManpowerBorder(FVector2D(ResX, 8.0f), FVector2D(ManpowerW, 36.0f));
	ManpowerBorder.SetColor(FLinearColor(0.35f, 0.65f, 0.95f, 0.8f));
	Canvas->DrawItem(ManpowerBorder);
	DrawShadowText(DefaultFont, ManpowerText, ResX + 6.0f, 18.0f, 0.86f, FLinearColor(0.65f, 0.88f, 1.0f));
	ResX += ManpowerW + 5.0f;

	// Dynamic Logistics Status Badge
	UDominionSupplyLineSubsystem* SupplySys = GetWorld() ? GetWorld()->GetSubsystem<UDominionSupplyLineSubsystem>() : nullptr;
	bool bSupplyActive = SupplySys ? SupplySys->IsSupplyLineActive() : true;
	FString LogisticsBadge = bSupplyActive ? TEXT("SUPPLY: 100% [42d]") : TEXT("SUPPLY: CUT [T]");
	FLinearColor SupplyCol = bSupplyActive ? FLinearColor(0.25f, 0.95f, 0.45f, 1.0f) : FLinearColor(1.0f, 0.25f, 0.15f, 1.0f);

	float LogW = LogisticsBadge.Len() * 7.2f + 12.0f;
	FCanvasTileItem LogBG(FVector2D(ResX, 8.0f), FVector2D(LogW, 36.0f), FLinearColor(0.07f, 0.09f, 0.13f, 0.88f));
	Canvas->DrawItem(LogBG);
	FCanvasBoxItem LogBorder(FVector2D(ResX, 8.0f), FVector2D(LogW, 36.0f));
	LogBorder.SetColor(SupplyCol);
	LogBorder.LineThickness = 1.2f;
	Canvas->DrawItem(LogBorder);
	DrawShadowText(DefaultFont, LogisticsBadge, ResX + 6.0f, 18.0f, 0.86f, SupplyCol);
	ResX += LogW + 16.0f;

	// --- Group 3: SOUL OF THE ESTATE - 3 ANTIQUE DIAL BAROMETERS ---
	UDominionPoliticalEstatesSystem* Estates = GetWorld() ? GetWorld()->GetSubsystem<UDominionPoliticalEstatesSystem>() : nullptr;
	if (Estates)
	{
		struct FEstateDial
		{
			FString Title;
			FString Subtitle;
			float Value;
			FLinearColor DialColor;
		};

		FEstateDial EstateDials[3] = {
			{ TEXT("ALTAR"), TEXT("CHURCH"), Estates->GetPriesthoodLoyalty(), FLinearColor(1.0f, 0.88f, 0.25f) },
			{ TEXT("THRONE"), TEXT("NOBLES"), Estates->GetNobilityLoyalty(), FLinearColor(0.95f, 0.40f, 0.40f) },
			{ TEXT("MASSES"), TEXT("SERFS"), Estates->GetMassesLoyalty(), FLinearColor(0.35f, 0.95f, 0.55f) }
		};

		const float DialDiameter = 40.0f;
		for (const FEstateDial& Dial : EstateDials)
		{
			// Barometer Housing Box
			const float HousingW = 100.0f;
			FCanvasTileItem HousingBG(FVector2D(ResX, 6.0f), FVector2D(HousingW, 42.0f), FLinearColor(0.08f, 0.07f, 0.05f, 0.95f));
			Canvas->DrawItem(HousingBG);

			FCanvasBoxItem HousingBorder(FVector2D(ResX, 6.0f), FVector2D(HousingW, 42.0f));
			HousingBorder.SetColor(FLinearColor(0.85f, 0.65f, 0.22f, 0.85f));
			HousingBorder.LineThickness = 1.2f;
			Canvas->DrawItem(HousingBorder);

			// Barometer Circular Needle Gauge (Simulated Dial Face)
			FVector2D DialCenter(ResX + 22.0f, 27.0f);
			FCanvasBoxItem DialRing(FVector2D(ResX + 4.0f, 9.0f), FVector2D(36.0f, 36.0f));
			DialRing.SetColor(Dial.DialColor * 0.7f);
			Canvas->DrawItem(DialRing);

			// Needle line
			float AngleRad = FMath::DegreesToRadians(-135.0f + (Dial.Value / 100.0f) * 270.0f);
			FVector2D NeedleEnd = DialCenter + FVector2D(FMath::Cos(AngleRad) * 14.0f, FMath::Sin(AngleRad) * 14.0f);
			Canvas->K2_DrawLine(DialCenter, NeedleEnd, 2.0f, Dial.DialColor);

			// Barometer Text Labels
			DrawShadowText(DefaultFont, Dial.Title, ResX + 44.0f, 10.0f, 0.76f, Dial.DialColor);
			DrawShadowText(DefaultFont, FString::Printf(TEXT("%.0f%%"), Dial.Value), ResX + 44.0f, 26.0f, 0.85f, FLinearColor::White);

			ResX += HousingW + 8.0f;
		}
	}

	// Pure Historical Epoch Label (Right Aligned - no "EPOCH X:")
	const FString EpochText = TEXT("BRONZE AGE");
	const float EpochW = 150.0f;
	const float EpochX = ScreenW - EpochW - 16.0f;
	FCanvasTileItem EpochBG(FVector2D(EpochX, 8.0f), FVector2D(EpochW, 36.0f), FLinearColor(0.08f, 0.12f, 0.18f, 0.92f));
	Canvas->DrawItem(EpochBG);
	FCanvasBoxItem EpochBorder(FVector2D(EpochX, 8.0f), FVector2D(EpochW, 36.0f));
	EpochBorder.SetColor(FLinearColor(0.92f, 0.72f, 0.22f, 0.9f));
	Canvas->DrawItem(EpochBorder);
	DrawShadowText(MediumFont, EpochText, EpochX + 16.0f, 15.0f, 0.95f, FLinearColor(1.0f, 0.88f, 0.40f, 1.0f));

	// Active Imperial Edict Banner under Top Header
	if (Estates && Estates->GetActiveEdictRemainingTime() > 0.0f)
	{
		FString EdictMsg = FString::Printf(TEXT("[ACTIVE IMPERIAL EDICT]: %s (%.0fs remaining)"), *Estates->GetActiveEdictName(), Estates->GetActiveEdictRemainingTime());
		const float EdictW = 640.0f;
		const float EdictX = (ScreenW - EdictW) * 0.5f;
		FCanvasTileItem EdictBG(FVector2D(EdictX, TopBarH + 4.0f), FVector2D(EdictW, 26.0f), FLinearColor(0.05f, 0.12f, 0.18f, 0.95f));
		Canvas->DrawItem(EdictBG);
		FCanvasBoxItem EdictBorder(FVector2D(EdictX, TopBarH + 4.0f), FVector2D(EdictW, 26.0f));
		EdictBorder.SetColor(FLinearColor(0.3f, 0.85f, 1.0f));
		Canvas->DrawItem(EdictBorder);
		DrawShadowText(DefaultFont, EdictMsg, EdictX + 16.0f, TopBarH + 9.0f, 0.92f, FLinearColor(1.0f, 0.95f, 0.6f));
	}

	// 2. Draw Bottom Command Console (Compact Slim 148px Low-Profile Frame)
	const float ConsoleH = 148.0f;
	const float ConsoleY = ScreenH - ConsoleH;

	// Dark Hewn Slate Bottom Console Frame
	FCanvasTileItem ConsoleBG(FVector2D(0, ConsoleY), FVector2D(ScreenW, ConsoleH), FLinearColor(0.04f, 0.04f, 0.05f, 0.98f));
	ConsoleBG.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(ConsoleBG);

	// Heavy Carved Brass Top Border
	Canvas->K2_DrawLine(FVector2D(0, ConsoleY), FVector2D(ScreenW, ConsoleY), 3.0f, FLinearColor(0.85f, 0.65f, 0.22f, 1.0f));
	Canvas->K2_DrawLine(FVector2D(0, ConsoleY + 2.0f), FVector2D(ScreenW, ConsoleY + 2.0f), 1.0f, FLinearColor(0.25f, 0.18f, 0.08f, 0.9f));

	// --- SECTION 1 (LEFT): DUAL STATE CARD (UNIT SELECTED VS REALM CAPITAL OVERVIEW) ---
	const float CardX = 20.0f;
	const float CardY = ConsoleY + 10.0f;
	const float CardW = 350.0f;
	const float CardH = ConsoleH - 20.0f;

	FCanvasTileItem CardBG(FVector2D(CardX, CardY), FVector2D(CardW, CardH), FLinearColor(0.08f, 0.07f, 0.06f, 0.95f));
	Canvas->DrawItem(CardBG);

	FCanvasBoxItem CardBorder(FVector2D(CardX, CardY), FVector2D(CardW, CardH));
	CardBorder.SetColor(FLinearColor(0.75f, 0.58f, 0.22f, 0.9f));
	CardBorder.LineThickness = 1.8f;
	Canvas->DrawItem(CardBorder);

	ADominionRTSPlayerController* PC = Cast<ADominionRTSPlayerController>(GetOwningPlayerController());
	if (PC && PC->GetSelectedUnits().Num() > 0)
	{
		ADominionUnitActor* SelUnit = PC->GetSelectedUnits()[0];
		if (IsValid(SelUnit))
		{
			// Portrait Box
			const float PortW = 64.0f;
			const float PortH = CardH - 18.0f;
			FCanvasTileItem PortBG(FVector2D(CardX + 10.0f, CardY + 9.0f), FVector2D(PortW, PortH), FLinearColor(0.14f, 0.11f, 0.08f));
			Canvas->DrawItem(PortBG);
			FCanvasBoxItem PortBorder(FVector2D(CardX + 10.0f, CardY + 9.0f), FVector2D(PortW, PortH));
			PortBorder.SetColor(FLinearColor(0.92f, 0.72f, 0.22f));
			Canvas->DrawItem(PortBorder);

			DrawShadowText(MediumFont, TEXT("⚔️"), CardX + 28.0f, CardY + 44.0f, 1.2f, FLinearColor(1.0f, 0.85f, 0.2f));

			// Unit Info Text
			const float TextX = CardX + PortW + 18.0f;
			DrawShadowText(MediumFont, SelUnit->UnitName.ToUpper(), TextX, CardY + 8.0f, 0.95f, FLinearColor::White);
			FString SubTitle = FString::Printf(TEXT("Rank: Veteran  |  Squad: %d"), PC->GetSelectedUnits().Num());
			DrawShadowText(DefaultFont, SubTitle, TextX, CardY + 28.0f, 0.80f, FLinearColor(0.75f, 0.85f, 0.95f));

			// Health Bar
			float HPPercent = FMath::Clamp(SelUnit->Health / SelUnit->MaxHealth, 0.0f, 1.0f);
			const float BarW = 220.0f;
			FCanvasTileItem HPBG(FVector2D(TextX, CardY + 46.0f), FVector2D(BarW, 9.0f), FLinearColor(0.15f, 0.15f, 0.15f));
			Canvas->DrawItem(HPBG);
			FCanvasTileItem HPFill(FVector2D(TextX, CardY + 46.0f), FVector2D(BarW * HPPercent, 9.0f), FLinearColor(0.2f, 0.85f, 0.35f));
			Canvas->DrawItem(HPFill);
			DrawShadowText(DefaultFont, FString::Printf(TEXT("HP: %.0f / %.0f"), SelUnit->Health, SelUnit->MaxHealth), TextX + 4.0f, CardY + 45.0f, 0.65f, FLinearColor::White);

			// Morale Bar
			FCanvasTileItem MoraleBG(FVector2D(TextX, CardY + 59.0f), FVector2D(BarW, 9.0f), FLinearColor(0.15f, 0.15f, 0.15f));
			Canvas->DrawItem(MoraleBG);
			FCanvasTileItem MoraleFill(FVector2D(TextX, CardY + 59.0f), FVector2D(BarW * 0.95f, 9.0f), FLinearColor(0.2f, 0.65f, 1.0f));
			Canvas->DrawItem(MoraleFill);
			DrawShadowText(DefaultFont, TEXT("MORALE: 95% (Steady)"), TextX + 4.0f, CardY + 58.0f, 0.65f, FLinearColor::White);

			// Stats: ATT / DEF / SPEED
			FString StatStr = FString::Printf(TEXT("ATT: %.0f • DEF: 35 • SPD: %.0f"), SelUnit->AttackPower, SelUnit->MoveSpeed);
			DrawShadowText(DefaultFont, StatStr, TextX, CardY + 76.0f, 0.78f, FLinearColor(1.0f, 0.88f, 0.45f));

			// Stance & Supply Days
			FString StanceStr = SelUnit->bInShieldWall ? TEXT("Stance: Phalanx (+40% Armor)  |  Food: 42d") : TEXT("Stance: Open Order  |  Food: 42d");
			DrawShadowText(DefaultFont, StanceStr, TextX, CardY + 96.0f, 0.75f, FLinearColor(0.65f, 0.85f, 1.0f));
		}
	}
	else
	{
		// UNSELECTED STATE: REALM CAPITAL CITADEL OVERVIEW CARD
		const float CrestW = 64.0f;
		const float CrestH = CardH - 18.0f;
		FCanvasTileItem CrestBG(FVector2D(CardX + 10.0f, CardY + 9.0f), FVector2D(CrestW, CrestH), FLinearColor(0.12f, 0.08f, 0.06f));
		Canvas->DrawItem(CrestBG);
		FCanvasBoxItem CrestBorder(FVector2D(CardX + 10.0f, CardY + 9.0f), FVector2D(CrestW, CrestH));
		CrestBorder.SetColor(FLinearColor(0.92f, 0.72f, 0.22f));
		Canvas->DrawItem(CrestBorder);

		// Golden Imperial Eagle Icon
		DrawShadowText(MediumFont, TEXT("🦅"), CardX + 26.0f, CardY + 44.0f, 1.3f, FLinearColor(1.0f, 0.85f, 0.2f));

		const float TextX = CardX + CrestW + 18.0f;
		DrawShadowText(MediumFont, TEXT("CITADEL OF UR-KISH"), TextX, CardY + 8.0f, 0.96f, FLinearColor(1.0f, 0.88f, 0.40f));
		DrawShadowText(DefaultFont, TEXT("Imperial Capital (No Unit Selected)"), TextX, CardY + 28.0f, 0.78f, FLinearColor(0.75f, 0.85f, 0.95f));

		DrawShadowText(DefaultFont, TEXT("Province Pop: 15,000   •   Garrison: 450 Hoplites"), TextX, CardY + 50.0f, 0.78f, FLinearColor::White);
		DrawShadowText(DefaultFont, TEXT("Stability: 88% (Concordat)   •   Granary: 60 Days"), TextX, CardY + 70.0f, 0.78f, FLinearColor(0.35f, 0.95f, 0.55f));
		DrawShadowText(DefaultFont, TEXT("[LMB] Select Legion   •   [RMB] Move/Attack"), TextX, CardY + 94.0f, 0.75f, FLinearColor(0.70f, 0.75f, 0.80f));
	}

	// --- SECTION 2 (CENTER): COMPACT COMMAND GRID (FORMATIONS & EDICTS) ---
	const float GridLeft = CardX + CardW + 24.0f;
	const float GridTop = ConsoleY + 10.0f;

	struct FCmdBtn { const TCHAR* Key; const TCHAR* Label; };
	const FCmdBtn Formations[4] = {
		{ TEXT("[1]"), TEXT("LINE") },
		{ TEXT("[2]"), TEXT("SQUARE") },
		{ TEXT("[3]"), TEXT("PHALANX") },
		{ TEXT("[4]"), TEXT("SKIRMISH") }
	};

	const float BtnW = 70.0f;
	const float BtnH = 48.0f;
	for (int32 i = 0; i < 4; ++i)
	{
		FVector2D Pos(GridLeft + i * (BtnW + 6.0f), GridTop + 14.0f);
		FCanvasTileItem BtnBG(Pos, FVector2D(BtnW, BtnH), FLinearColor(0.12f, 0.09f, 0.07f, 0.95f));
		Canvas->DrawItem(BtnBG);
		FCanvasBoxItem BtnBorder(Pos, FVector2D(BtnW, BtnH));
		BtnBorder.SetColor(FLinearColor(0.85f, 0.65f, 0.22f, 0.85f));
		Canvas->DrawItem(BtnBorder);
		DrawShadowText(DefaultFont, Formations[i].Key, Pos.X + 6.0f, Pos.Y + 5.0f, 0.75f, FLinearColor(1.0f, 0.85f, 0.2f));
		DrawShadowText(DefaultFont, Formations[i].Label, Pos.X + 6.0f, Pos.Y + 24.0f, 0.76f, FLinearColor::White);
	}

	const FCmdBtn Edicts[4] = {
		{ TEXT("[F1]"), TEXT("TITHE") },
		{ TEXT("[F2]"), TEXT("CORVEE") },
		{ TEXT("[F3]"), TEXT("DOLE") },
		{ TEXT("[F4]"), TEXT("GAMES") }
	};

	const float EdictTop = GridTop + 68.0f;
	for (int32 i = 0; i < 4; ++i)
	{
		FVector2D Pos(GridLeft + i * (BtnW + 6.0f), EdictTop);
		FCanvasTileItem BtnBG(Pos, FVector2D(BtnW, BtnH), FLinearColor(0.08f, 0.10f, 0.14f, 0.95f));
		Canvas->DrawItem(BtnBG);
		FCanvasBoxItem BtnBorder(Pos, FVector2D(BtnW, BtnH));
		BtnBorder.SetColor(FLinearColor(0.35f, 0.75f, 1.0f, 0.85f));
		Canvas->DrawItem(BtnBorder);
		DrawShadowText(DefaultFont, Edicts[i].Key, Pos.X + 6.0f, Pos.Y + 5.0f, 0.75f, FLinearColor(0.45f, 0.88f, 1.0f));
		DrawShadowText(DefaultFont, Edicts[i].Label, Pos.X + 6.0f, Pos.Y + 24.0f, 0.76f, FLinearColor::White);
	}

	// --- SECTION 3 (RIGHT): ANCIENT PARCHMENT CARTOGRAPHIC MINIMAP ---
	const float MapW = 190.0f;
	const float MapH = ConsoleH - 20.0f;
	const float MapX = ScreenW - MapW - 20.0f;
	const float MapY = ConsoleY + 10.0f;

	// Warm Aged Vellum / Parchment Map Background
	FCanvasTileItem MapBG(FVector2D(MapX, MapY), FVector2D(MapW, MapH), FLinearColor(0.84f, 0.76f, 0.60f, 0.98f));
	Canvas->DrawItem(MapBG);

	// Hand-Carved Walnut Wood & Bronze Double Border
	FCanvasBoxItem MapBorder(FVector2D(MapX, MapY), FVector2D(MapW, MapH));
	MapBorder.SetColor(FLinearColor(0.45f, 0.28f, 0.12f, 1.0f));
	MapBorder.LineThickness = 2.5f;
	Canvas->DrawItem(MapBorder);

	FCanvasBoxItem MapInnerBorder(FVector2D(MapX + 3.0f, MapY + 3.0f), FVector2D(MapW - 6.0f, MapH - 6.0f));
	MapInnerBorder.SetColor(FLinearColor(0.75f, 0.58f, 0.22f, 0.85f));
	MapInnerBorder.LineThickness = 1.0f;
	Canvas->DrawItem(MapInnerBorder);

	// Sepia Ink River Course Lines
	Canvas->K2_DrawLine(FVector2D(MapX + 15.0f, MapY + MapH - 20.0f), FVector2D(MapX + 85.0f, MapY + 60.0f), 2.5f, FLinearColor(0.25f, 0.35f, 0.45f, 0.9f));
	Canvas->K2_DrawLine(FVector2D(MapX + 85.0f, MapY + 60.0f), FVector2D(MapX + 155.0f, MapY + 15.0f), 2.5f, FLinearColor(0.25f, 0.35f, 0.45f, 0.9f));

	// Ancient Cartographic Stamp: "N" Compass Fleur & Map Title
	DrawShadowText(DefaultFont, TEXT("▲ N"), MapX + MapW - 28.0f, MapY + 8.0f, 0.75f, FLinearColor(0.35f, 0.22f, 0.10f));
	DrawShadowText(DefaultFont, TEXT("MESOPOTAMIA"), MapX + 8.0f, MapY + MapH - 18.0f, 0.70f, FLinearColor(0.35f, 0.22f, 0.10f));

	// Real-Time Tactical Troop Heraldic Pins
	TArray<AActor*> AllRadarUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADominionUnitActor::StaticClass(), AllRadarUnits);
	for (AActor* Act : AllRadarUnits)
	{
		if (ADominionUnitActor* Unit = Cast<ADominionUnitActor>(Act))
		{
			FVector UnitLoc = Unit->GetActorLocation();
			float PinX = MapX + (MapW * 0.5f) + (UnitLoc.X / 120.0f);
			float PinY = MapY + (MapH * 0.5f) + (UnitLoc.Y / 120.0f);
			PinX = FMath::Clamp(PinX, MapX + 8.0f, MapX + MapW - 14.0f);
			PinY = FMath::Clamp(PinY, MapY + 8.0f, MapY + MapH - 14.0f);

			// Blue Imperial Heraldic Crest vs Red Barbarian Shield
			FLinearColor CrestColor = (Unit->TeamID == 0) ? FLinearColor(0.10f, 0.35f, 0.85f) : FLinearColor(0.85f, 0.15f, 0.10f);
			FCanvasTileItem CrestPin(FVector2D(PinX, PinY), FVector2D(6.0f, 6.0f), CrestColor);
			Canvas->DrawItem(CrestPin);
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
