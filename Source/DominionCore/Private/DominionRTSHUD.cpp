#include "DominionRTSHUD.h"
#include "DominionRTSPlayerController.h"
#include "DominionUnitActor.h"
#include "DominionBuildingActor.h"
#include "DominionTutorialSubsystem.h"
#include "DominionSupplyLineSubsystem.h"
#include "DominionFormationSystem.h"
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

	// 1. Draw Top Resource & Epoch Header Bar
	const float ScreenW = Canvas->SizeX;
	const float ScreenH = Canvas->SizeY;
	const float TopBarH = 50.0f;

	// Top Bar Dark Gold Translucent Background
	FCanvasTileItem TopBarBG(FVector2D(0, 0), FVector2D(ScreenW, TopBarH), FLinearColor(0.03f, 0.04f, 0.06f, 0.96f));
	TopBarBG.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TopBarBG);

	// Top Bar Gold Border Line
	Canvas->K2_DrawLine(FVector2D(0, TopBarH), FVector2D(ScreenW, TopBarH), 2.5f, FLinearColor(0.92f, 0.72f, 0.22f, 1.0f));

	// Resource Badges (Spacious layout with generous padding)
	const FString ResBadges[] = {
		TEXT("GRAIN: 1,420 (+45/m)"),
		TEXT("TIMBER: 860 (+18/m)"),
		TEXT("GOLD: 5,200"),
		TEXT("BRONZE: 650"),
		TEXT("POP: 48/80"),
		TEXT("INFAMY: 12%")
	};

	float ResX = 24.0f;
	for (const FString& Badge : ResBadges)
	{
		float BadgeW = Badge.Len() * 8.5f + 20.0f;
		FCanvasTileItem BadgeBG(FVector2D(ResX, 8.0f), FVector2D(BadgeW, 34.0f), FLinearColor(0.08f, 0.11f, 0.16f, 0.85f));
		Canvas->DrawItem(BadgeBG);

		FCanvasBoxItem BadgeBorder(FVector2D(ResX, 8.0f), FVector2D(BadgeW, 34.0f));
		BadgeBorder.SetColor(FLinearColor(0.35f, 0.45f, 0.55f, 0.5f));
		BadgeBorder.LineThickness = 1.0f;
		Canvas->DrawItem(BadgeBorder);

		DrawShadowText(DefaultFont, Badge, ResX + 10.0f, 16.0f, 1.0f, FLinearColor(1.0f, 0.88f, 0.45f, 1.0f));
		ResX += BadgeW + 12.0f;
	}

	// Dynamic Logistics & Ox-Cart Supply Line Status Badge
	UDominionSupplyLineSubsystem* SupplySys = GetWorld() ? GetWorld()->GetSubsystem<UDominionSupplyLineSubsystem>() : nullptr;
	bool bSupplyActive = SupplySys ? SupplySys->IsSupplyLineActive() : true;
	FString LogisticsBadge = bSupplyActive ? TEXT("SUPPLY: [ACTIVE OX-CART 100%]") : TEXT("SUPPLY: [!] SEVERED (ATTRITION) [T]");
	FLinearColor SupplyCol = bSupplyActive ? FLinearColor(0.2f, 0.95f, 0.4f, 1.0f) : FLinearColor(1.0f, 0.25f, 0.15f, 1.0f);

	float LogW = LogisticsBadge.Len() * 8.5f + 20.0f;
	FCanvasTileItem LogBG(FVector2D(ResX, 8.0f), FVector2D(LogW, 34.0f), FLinearColor(0.08f, 0.11f, 0.16f, 0.85f));
	Canvas->DrawItem(LogBG);

	FCanvasBoxItem LogBorder(FVector2D(ResX, 8.0f), FVector2D(LogW, 34.0f));
	LogBorder.SetColor(SupplyCol);
	LogBorder.LineThickness = 1.5f;
	Canvas->DrawItem(LogBorder);

	DrawShadowText(DefaultFont, LogisticsBadge, ResX + 10.0f, 16.0f, 1.0f, SupplyCol);

	// Epoch Badge (Right Aligned)
	const FString EpochText = TEXT("EPOCH I: BRONZE AGE (3,000 BCE)");
	const float EpochW = 340.0f;
	const float EpochX = ScreenW - EpochW - 24.0f;
	FCanvasTileItem EpochBG(FVector2D(EpochX, 8.0f), FVector2D(EpochW, 34.0f), FLinearColor(0.08f, 0.12f, 0.20f, 0.9f));
	Canvas->DrawItem(EpochBG);

	FCanvasBoxItem EpochBorder(FVector2D(EpochX, 8.0f), FVector2D(EpochW, 34.0f));
	EpochBorder.SetColor(FLinearColor(0.92f, 0.72f, 0.22f, 0.8f));
	EpochBorder.LineThickness = 1.5f;
	Canvas->DrawItem(EpochBorder);
	DrawShadowText(MediumFont, EpochText, EpochX + 14.0f, 14.0f, 0.9f, FLinearColor(0.45f, 0.88f, 1.0f, 1.0f));

	// 2. Draw Interactive Tutorial / Campaign Quest Card (Spacious layout with generous padding)
	UDominionTutorialSubsystem* Tutorial = GetWorld() ? GetWorld()->GetSubsystem<UDominionTutorialSubsystem>() : nullptr;
	if (Tutorial && bShowTutorialCard)
	{
		const float TutX = 24.0f;
		const float TutY = TopBarH + 16.0f;
		const float TutW = 720.0f;
		const float TutH = 114.0f;

		// Card Background
		FCanvasTileItem TutBG(FVector2D(TutX, TutY), FVector2D(TutW, TutH), FLinearColor(0.03f, 0.04f, 0.07f, 0.94f));
		TutBG.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TutBG);

		// Gold Filigree Border
		FCanvasBoxItem TutBorder(FVector2D(TutX, TutY), FVector2D(TutW, TutH));
		TutBorder.SetColor(FLinearColor(0.92f, 0.72f, 0.22f, 1.0f));
		TutBorder.LineThickness = 2.0f;
		Canvas->DrawItem(TutBorder);

		// Header Step Pill
		FString StepHeader = FString::Printf(TEXT("[TUTORIAL STEP %d / %d]   %s"), Tutorial->GetStepNumber(), Tutorial->GetTotalSteps(), *Tutorial->GetObjectiveTitle());
		DrawShadowText(MediumFont, StepHeader, TutX + 18.0f, TutY + 12.0f, 0.95f, FLinearColor(1.0f, 0.84f, 0.0f, 1.0f));

		// Divider Line
		Canvas->K2_DrawLine(FVector2D(TutX + 16.0f, TutY + 38.0f), FVector2D(TutX + TutW - 16.0f, TutY + 38.0f), 1.0f, FLinearColor(0.4f, 0.5f, 0.6f, 0.4f));

		// Objective Instructions (Generous vertical spacing)
		DrawShadowText(DefaultFont, Tutorial->GetObjectiveDescription(), TutX + 18.0f, TutY + 48.0f, 1.05f, FLinearColor::White);

		// Advisor Quote (Amber/Cyan dialogue)
		DrawShadowText(DefaultFont, Tutorial->GetAdvisorAdvice(), TutX + 18.0f, TutY + 80.0f, 0.95f, FLinearColor(0.55f, 0.88f, 1.0f, 0.95f));
	}

	// 3. Draw Bottom Command Console (AoE2 3-Section Frame)
	const float ConsoleH = 180.0f;
	const float ConsoleY = ScreenH - ConsoleH;

	// Console Background Ribbon
	FCanvasTileItem ConsoleBG(FVector2D(0, ConsoleY), FVector2D(ScreenW, ConsoleH), FLinearColor(0.03f, 0.04f, 0.06f, 0.97f));
	ConsoleBG.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(ConsoleBG);

	Canvas->K2_DrawLine(FVector2D(0, ConsoleY), FVector2D(ScreenW, ConsoleY), 3.0f, FLinearColor(0.92f, 0.72f, 0.22f, 1.0f));

	// --- SECTION 1: LEFT MINIMAP RADAR ---
	const float MinimapMargin = 14.0f;
	const float MinimapW = 230.0f;
	const float MinimapH = ConsoleH - (MinimapMargin * 2.0f);
	FVector2D MinimapPos(MinimapMargin, ConsoleY + MinimapMargin);

	FCanvasTileItem MinimapBG(MinimapPos, FVector2D(MinimapW, MinimapH), FLinearColor(0.05f, 0.08f, 0.12f, 1.0f));
	Canvas->DrawItem(MinimapBG);

	FCanvasBoxItem MinimapBorder(MinimapPos, FVector2D(MinimapW, MinimapH));
	MinimapBorder.SetColor(FLinearColor(0.92f, 0.72f, 0.22f, 1.0f));
	MinimapBorder.LineThickness = 2.0f;
	Canvas->DrawItem(MinimapBorder);

	// Minimap Label & River Curve
	DrawShadowText(DefaultFont, TEXT("STRATEGIC RADAR // SECTOR 07"), MinimapPos.X + 10.0f, MinimapPos.Y + 8.0f, 0.95f, FLinearColor(1.0f, 0.85f, 0.3f, 1.0f));
	Canvas->K2_DrawLine(MinimapPos + FVector2D(20, 120), MinimapPos + FVector2D(120, 60), 3.0f, FLinearColor(0.15f, 0.45f, 0.85f, 0.8f));
	Canvas->K2_DrawLine(MinimapPos + FVector2D(120, 60), MinimapPos + FVector2D(200, 20), 3.0f, FLinearColor(0.15f, 0.45f, 0.85f, 0.8f));

	// Draw Real-Time Radar Blips for all Units
	TArray<AActor*> AllUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADominionUnitActor::StaticClass(), AllUnits);
	for (AActor* Act : AllUnits)
	{
		if (ADominionUnitActor* Unit = Cast<ADominionUnitActor>(Act))
		{
			FVector UnitLoc = Unit->GetActorLocation();
			float BlipX = MinimapPos.X + (MinimapW * 0.5f) + (UnitLoc.X / 100.0f);
			float BlipY = MinimapPos.Y + (MinimapH * 0.5f) + (UnitLoc.Y / 100.0f);
			BlipX = FMath::Clamp(BlipX, MinimapPos.X + 6.0f, MinimapPos.X + MinimapW - 12.0f);
			BlipY = FMath::Clamp(BlipY, MinimapPos.Y + 6.0f, MinimapPos.Y + MinimapH - 12.0f);

			FLinearColor BlipCol = (Unit->TeamID == 0) ? FLinearColor(0.2f, 0.7f, 1.0f) : FLinearColor(1.0f, 0.2f, 0.2f);
			FCanvasTileItem Blip(FVector2D(BlipX, BlipY), FVector2D(7.0f, 7.0f), BlipCol);
			Canvas->DrawItem(Blip);
		}
	}

	// --- SECTION 2: CENTER SELECTION CARD ---
	const float CenterX = MinimapPos.X + MinimapW + 20.0f;
	const float CenterW = ScreenW - CenterX - 440.0f;
	FVector2D CenterPos(CenterX, ConsoleY + MinimapMargin);

	FCanvasTileItem CenterPanel(CenterPos, FVector2D(CenterW, MinimapH), FLinearColor(0.04f, 0.06f, 0.09f, 0.92f));
	Canvas->DrawItem(CenterPanel);

	FCanvasBoxItem CenterBorder(CenterPos, FVector2D(CenterW, MinimapH));
	CenterBorder.SetColor(FLinearColor(0.4f, 0.5f, 0.6f, 0.6f));
	CenterBorder.LineThickness = 1.5f;
	Canvas->DrawItem(CenterBorder);

	ADominionRTSPlayerController* PC = Cast<ADominionRTSPlayerController>(GetOwningPlayerController());
	if (PC && PC->GetSelectedUnits().Num() > 0)
	{
		ADominionUnitActor* SelUnit = PC->GetSelectedUnits()[0];
		if (IsValid(SelUnit))
		{
			// Portrait Box
			const float PortW = 86.0f;
			const float PortH = MinimapH - 24.0f;
			FCanvasTileItem PortraitBG(CenterPos + FVector2D(12, 12), FVector2D(PortW, PortH), FLinearColor(0.10f, 0.15f, 0.22f));
			Canvas->DrawItem(PortraitBG);

			FCanvasBoxItem PortBorder(CenterPos + FVector2D(12, 12), FVector2D(PortW, PortH));
			PortBorder.SetColor(FLinearColor(0.92f, 0.72f, 0.22f, 0.7f));
			PortBorder.LineThickness = 1.5f;
			Canvas->DrawItem(PortBorder);

			DrawShadowText(DefaultFont, TEXT("[UNIT]"), CenterPos.X + 24.0f, CenterPos.Y + 56.0f, 1.25f, FLinearColor(1.0f, 0.85f, 0.2f, 1.0f));

			// Unit Title & Subtitle
			const float TextLeft = CenterPos.X + PortW + 28.0f;
			DrawShadowText(MediumFont, SelUnit->UnitName.ToUpper(), TextLeft, CenterPos.Y + 14.0f, 1.15f, FLinearColor::White);
			FString Subtitle = FString::Printf(TEXT("Regiment Size: %d units   |   Stance: %s"), PC->GetSelectedUnits().Num(), SelUnit->bInShieldWall ? TEXT("PHALANX BRACED (+40% Armor)") : TEXT("OPEN CHARGE"));
			DrawShadowText(DefaultFont, Subtitle, TextLeft, CenterPos.Y + 44.0f, 1.05f, FLinearColor(0.75f, 0.85f, 0.95f, 1.0f));

			// Health Bar
			float HPPercent = FMath::Clamp(SelUnit->Health / SelUnit->MaxHealth, 0.0f, 1.0f);
			const float BarW = 320.0f;
			const float BarH = 18.0f;
			FCanvasTileItem HPBarBG(FVector2D(TextLeft, CenterPos.Y + 74.0f), FVector2D(BarW, BarH), FLinearColor(0.15f, 0.15f, 0.15f));
			Canvas->DrawItem(HPBarBG);

			FLinearColor HPCol = (HPPercent > 0.5f) ? FLinearColor(0.2f, 0.85f, 0.3f) : FLinearColor(0.9f, 0.2f, 0.2f);
			FCanvasTileItem HPBarFill(FVector2D(TextLeft, CenterPos.Y + 74.0f), FVector2D(BarW * HPPercent, BarH), HPCol);
			Canvas->DrawItem(HPBarFill);

			FString HPText = FString::Printf(TEXT("HEALTH: %.0f / %.0f (%.0f%%)"), SelUnit->Health, SelUnit->MaxHealth, HPPercent * 100.0f);
			DrawShadowText(DefaultFont, HPText, TextLeft + 8.0f, CenterPos.Y + 76.0f, 0.95f, FLinearColor::White);

			// Morale Bar
			FCanvasTileItem MoraleBG(FVector2D(TextLeft, CenterPos.Y + 102.0f), FVector2D(BarW, BarH), FLinearColor(0.15f, 0.15f, 0.15f));
			Canvas->DrawItem(MoraleBG);
			FCanvasTileItem MoraleFill(FVector2D(TextLeft, CenterPos.Y + 102.0f), FVector2D(BarW * (SelUnit->Morale / 100.0f), BarH), FLinearColor(0.2f, 0.6f, 0.95f));
			Canvas->DrawItem(MoraleFill);
			FString MoraleText = FString::Printf(TEXT("MORALE: %.0f%% [STEADFAST]   |   SUPPLY: 28 DAYS"), SelUnit->Morale);
			DrawShadowText(DefaultFont, MoraleText, TextLeft + 8.0f, CenterPos.Y + 104.0f, 0.95f, FLinearColor::White);
		}
	}
	else if (PC && PC->GetSelectedBuilding())
	{
		ADominionBuildingActor* Bld = PC->GetSelectedBuilding();
		DrawShadowText(MediumFont, Bld->BuildingName.ToUpper(), CenterPos.X + 24.0f, CenterPos.Y + 18.0f, 1.25f, FLinearColor(1.0f, 0.85f, 0.2f, 1.0f));
		DrawShadowText(DefaultFont, TEXT("Town Center & Civic Granary   |   HP: 2,500 / 2,500"), CenterPos.X + 24.0f, CenterPos.Y + 52.0f, 1.15f, FLinearColor::White);
		DrawShadowText(DefaultFont, TEXT("Press [A] to Train Spearman   •   Press [S] to Train Chariot   •   Press [D] for Ox-Cart"), CenterPos.X + 24.0f, CenterPos.Y + 90.0f, 1.1f, FLinearColor(0.45f, 0.9f, 1.0f, 1.0f));
	}
	else
	{
		DrawShadowText(MediumFont, TEXT("NO SELECTION // CLICK A REGIMENT OR DRAG-BOX ON TERRAIN"), CenterPos.X + 24.0f, CenterPos.Y + 42.0f, 1.1f, FLinearColor(0.85f, 0.85f, 0.85f, 1.0f));
		DrawShadowText(DefaultFont, TEXT("WASD / Arrows: Pan   •   Q/E: Rotate   •   Mouse Wheel: Zoom   •   [1/2/3/4]: Formations"), CenterPos.X + 24.0f, CenterPos.Y + 78.0f, 1.1f, FLinearColor(0.92f, 0.72f, 0.22f, 1.0f));
	}

	// --- SECTION 3: RIGHT 3x5 ACTION GRID (Two-Line Centered Buttons) ---
	const float GridX = ScreenW - 400.0f;
	const float GridY = ConsoleY + MinimapMargin;
	const float BtnW = 84.0f;
	const float BtnH = 68.0f;
	const float Gap = 10.0f;

	struct FActionBtn
	{
		const TCHAR* Key;
		const TCHAR* Action;
	};

	const FActionBtn Actions[8] = {
		{ TEXT("[1]"), TEXT("PHALANX") },
		{ TEXT("[2]"), TEXT("WEDGE") },
		{ TEXT("[3]"), TEXT("SKIRMISH") },
		{ TEXT("[4]"), TEXT("SQUARE") },
		{ TEXT("[Z]"), TEXT("SPEARMAN") },
		{ TEXT("[C]"), TEXT("CHARIOT") },
		{ TEXT("[V]"), TEXT("OX-CART") },
		{ TEXT("[X]"), TEXT("NEXT ERA") }
	};

	for (int32 i = 0; i < 8; ++i)
	{
		int32 Row = i / 4;
		int32 Col = i % 4;

		FVector2D BtnPos(GridX + Col * (BtnW + Gap), GridY + Row * (BtnH + Gap));

		// Button Background
		FCanvasTileItem BtnBG(BtnPos, FVector2D(BtnW, BtnH), FLinearColor(0.09f, 0.13f, 0.19f, 0.98f));
		Canvas->DrawItem(BtnBG);

		// Gold Border
		FCanvasBoxItem BtnBorder(BtnPos, FVector2D(BtnW, BtnH));
		BtnBorder.SetColor(FLinearColor(0.92f, 0.72f, 0.22f, 0.85f));
		BtnBorder.LineThickness = 1.5f;
		Canvas->DrawItem(BtnBorder);

		// Two-Line Text with Centered Alignment
		DrawShadowText(DefaultFont, Actions[i].Key, BtnPos.X + 12.0f, BtnPos.Y + 12.0f, 1.05f, FLinearColor(1.0f, 0.85f, 0.2f, 1.0f));
		DrawShadowText(DefaultFont, Actions[i].Action, BtnPos.X + 8.0f, BtnPos.Y + 36.0f, 0.82f, FLinearColor::White);
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
