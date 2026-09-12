#include "DominionRTSHUD.h"
#include "DominionGameModeBase.h"
#include "DominionRTSPlayerController.h"
#include "DominionRTSPawn.h"
#include "DominionUnitActor.h"
#include "DominionBuildingActor.h"
#include "DominionTutorialSubsystem.h"
#include "DominionSupplyLineSubsystem.h"
#include "DominionFormationSystem.h"
#include "DominionDemographicsSubsystem.h"
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

	const float ScreenW = Canvas->SizeX;
	const float ScreenH = Canvas->SizeY;
	const float DPIScale = FMath::Clamp(ScreenH / 1080.0f, 1.0f, 2.5f);

	UFont* DefaultFont = GEngine->GetSmallFont();
	UFont* MediumFont = GEngine->GetMediumFont() ? GEngine->GetMediumFont() : DefaultFont;
	UFont* LargeFont = GEngine->GetLargeFont() ? GEngine->GetLargeFont() : MediumFont;

	// High-contrast, drop-shadowed crisp text helper
	auto DrawParchmentText = [this, DPIScale](UFont* Font, const FString& Text, float X, float Y, float Scale, const FLinearColor& TextColor)
	{
		if (!Font) Font = GEngine->GetMediumFont() ? GEngine->GetMediumFont() : GEngine->GetSmallFont();
		float FinalScale = Scale * DPIScale;
		
		// Drop shadow
		Canvas->DrawColor = FColor(0, 0, 0, 255);
		Canvas->DrawText(Font, Text, X + (2.0f * DPIScale), Y + (2.0f * DPIScale), FinalScale, FinalScale, FFontRenderInfo());
		// Main text
		Canvas->DrawColor = TextColor.ToFColor(true);
		Canvas->DrawText(Font, Text, X, Y, FinalScale, FinalScale, FFontRenderInfo());
	};

	// Ornate beveled tablet helper
	auto DrawBeveledTablet = [this, DPIScale](const FVector2D& Pos, const FVector2D& Size, const FLinearColor& BGColor, const FLinearColor& GoldColor)
	{
		// Main dark background fill
		FCanvasTileItem BG(Pos, Size, BGColor);
		BG.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(BG);

		// Outer gold frame
		FCanvasBoxItem OuterBox(Pos, Size);
		OuterBox.SetColor(GoldColor);
		OuterBox.LineThickness = 2.0f * DPIScale;
		Canvas->DrawItem(OuterBox);

		// Inner recessed bevel line
		if (Size.X > 10.0f * DPIScale && Size.Y > 10.0f * DPIScale)
		{
			FCanvasBoxItem InnerBox(Pos + FVector2D(3.0f * DPIScale, 3.0f * DPIScale), Size - FVector2D(6.0f * DPIScale, 6.0f * DPIScale));
			InnerBox.SetColor(GoldColor * 0.45f);
			InnerBox.LineThickness = 1.0f * DPIScale;
			Canvas->DrawItem(InnerBox);
		}

		// Corner accent brackets
		const float NotchLen = 8.0f * DPIScale;
		const float NotchThick = 3.0f * DPIScale;
		Canvas->K2_DrawLine(Pos, Pos + FVector2D(NotchLen, 0), NotchThick, GoldColor);
		Canvas->K2_DrawLine(Pos, Pos + FVector2D(0, NotchLen), NotchThick, GoldColor);
		Canvas->K2_DrawLine(Pos + FVector2D(Size.X, 0), Pos + FVector2D(Size.X - NotchLen, 0), NotchThick, GoldColor);
		Canvas->K2_DrawLine(Pos + FVector2D(Size.X, 0), Pos + FVector2D(Size.X, NotchLen), NotchThick, GoldColor);
		Canvas->K2_DrawLine(Pos + FVector2D(0, Size.Y), Pos + FVector2D(NotchLen, Size.Y), NotchThick, GoldColor);
		Canvas->K2_DrawLine(Pos + FVector2D(0, Size.Y), Pos + FVector2D(0, Size.Y - NotchLen), NotchThick, GoldColor);
		Canvas->K2_DrawLine(Pos + Size, Pos + Size - FVector2D(NotchLen, 0), NotchThick, GoldColor);
		Canvas->K2_DrawLine(Pos + Size, Pos + Size - FVector2D(0, NotchLen), NotchThick, GoldColor);
	};

	// Draw filled circle helper
	auto DrawFilledCircle = [this](const FVector2D& Center, float Radius, const FLinearColor& Color, int32 Segments = 48)
	{
		for (float r = 1.0f; r <= Radius; r += 3.0f)
		{
			float Step = 2.0f * PI / (float)Segments;
			for (int32 i = 0; i < Segments; ++i)
			{
				float A1 = (float)i * Step;
				float A2 = (float)(i + 1) * Step;
				FVector2D P1 = Center + FVector2D(FMath::Cos(A1) * r, FMath::Sin(A1) * r);
				FVector2D P2 = Center + FVector2D(FMath::Cos(A2) * r, FMath::Sin(A2) * r);
				Canvas->K2_DrawLine(P1, P2, 3.2f, Color);
			}
		}
	};

	// Draw circle ring outline helper
	auto DrawCircleRing = [this](const FVector2D& Center, float Radius, float Thickness, const FLinearColor& Color, int32 Segments = 64)
	{
		float Step = 2.0f * PI / (float)Segments;
		for (int32 i = 0; i < Segments; ++i)
		{
			float A1 = (float)i * Step;
			float A2 = (float)(i + 1) * Step;
			FVector2D P1 = Center + FVector2D(FMath::Cos(A1) * Radius, FMath::Sin(A1) * Radius);
			FVector2D P2 = Center + FVector2D(FMath::Cos(A2) * Radius, FMath::Sin(A2) * Radius);
			Canvas->K2_DrawLine(P1, P2, Thickness, Color);
		}
	};

	// Palette definitions
	const FLinearColor ColorBasaltBG = FLinearColor(0.045f, 0.04f, 0.035f, 0.94f);
	const FLinearColor ColorBurnishedGold = FLinearColor(0.88f, 0.72f, 0.30f, 1.0f);
	const FLinearColor ColorParchment = FLinearColor(0.96f, 0.92f, 0.82f, 1.0f);
	const FLinearColor ColorMutedGold = FLinearColor(1.0f, 0.84f, 0.40f, 1.0f);
	const FLinearColor ColorVerdigris = FLinearColor(0.18f, 0.42f, 0.38f, 1.0f);

	ADominionGameModeBase* GM = Cast<ADominionGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	ADominionRTSPlayerController* PC = Cast<ADominionRTSPlayerController>(GetOwningPlayerController());
	UDominionDemographicsSubsystem* Demo = GetWorld() ? GetWorld()->GetSubsystem<UDominionDemographicsSubsystem>() : nullptr;
	UDominionPoliticalEstatesSystem* Estates = GetWorld() ? GetWorld()->GetSubsystem<UDominionPoliticalEstatesSystem>() : nullptr;

	// =========================================================================
	// 0. IN-WORLD UNIT SELECTION & HOVER OVERLAYS (CRISP BRACKETS & HP BARS)
	// =========================================================================
	TArray<AActor*> AllUnitActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADominionUnitActor::StaticClass(), AllUnitActors);

	ADominionUnitActor* CurrentHoveredUnit = PC ? PC->GetHoveredUnit() : nullptr;
	const TArray<ADominionUnitActor*>& SelectedUnitsList = PC ? PC->GetSelectedUnits() : TArray<ADominionUnitActor*>();

	for (AActor* Act : AllUnitActors)
	{
		ADominionUnitActor* Unit = Cast<ADominionUnitActor>(Act);
		if (!Unit || !IsValid(Unit) || Unit->Health <= 0.0f) continue;

		const bool bIsSelected = SelectedUnitsList.Contains(Unit);
		const bool bIsHovered = (Unit == CurrentHoveredUnit);

		if (!bIsSelected && !bIsHovered) continue;

		FVector FeetLoc = Unit->GetActorLocation() - FVector(0.0f, 0.0f, 90.0f);
		FVector HeadLoc = Unit->GetActorLocation() + FVector(0.0f, 0.0f, 95.0f);
		FVector CenterLoc = Unit->GetActorLocation();

		FVector2D ScreenCenter, ScreenHead, ScreenFeet;
		if (PC && PC->ProjectWorldLocationToScreen(CenterLoc, ScreenCenter) &&
			PC->ProjectWorldLocationToScreen(HeadLoc, ScreenHead) &&
			PC->ProjectWorldLocationToScreen(FeetLoc, ScreenFeet))
		{
			float BoxHeight = FMath::Abs(ScreenFeet.Y - ScreenHead.Y);
			BoxHeight = FMath::Clamp(BoxHeight, 40.0f * DPIScale, 180.0f * DPIScale);
			float BoxWidth = BoxHeight * 0.65f;

			FVector2D BoxTopLeft(ScreenCenter.X - BoxWidth * 0.5f, ScreenHead.Y);
			FVector2D BoxBottomRight(ScreenCenter.X + BoxWidth * 0.5f, ScreenFeet.Y);

			FLinearColor BracketColor = bIsSelected ? 
				((Unit->TeamID == 0) ? ColorBurnishedGold : FLinearColor(1.0f, 0.25f, 0.25f)) : 
				FLinearColor(0.4f, 0.85f, 1.0f);

			float BracketLen = FMath::Clamp(BoxWidth * 0.28f, 6.0f * DPIScale, 22.0f * DPIScale);
			float LineThick = 2.0f * DPIScale;

			// 4 Corner brackets [ ]
			// Top-Left
			Canvas->K2_DrawLine(BoxTopLeft, BoxTopLeft + FVector2D(BracketLen, 0.0f), LineThick, BracketColor);
			Canvas->K2_DrawLine(BoxTopLeft, BoxTopLeft + FVector2D(0.0f, BracketLen), LineThick, BracketColor);
			// Top-Right
			Canvas->K2_DrawLine(FVector2D(BoxBottomRight.X, BoxTopLeft.Y), FVector2D(BoxBottomRight.X - BracketLen, BoxTopLeft.Y), LineThick, BracketColor);
			Canvas->K2_DrawLine(FVector2D(BoxBottomRight.X, BoxTopLeft.Y), FVector2D(BoxBottomRight.X, BoxTopLeft.Y + BracketLen), LineThick, BracketColor);
			// Bottom-Left
			Canvas->K2_DrawLine(FVector2D(BoxTopLeft.X, BoxBottomRight.Y), FVector2D(BoxTopLeft.X + BracketLen, BoxBottomRight.Y), LineThick, BracketColor);
			Canvas->K2_DrawLine(FVector2D(BoxTopLeft.X, BoxBottomRight.Y), FVector2D(BoxTopLeft.X, BoxBottomRight.Y - BracketLen), LineThick, BracketColor);
			// Bottom-Right
			Canvas->K2_DrawLine(BoxBottomRight, BoxBottomRight - FVector2D(BracketLen, 0.0f), LineThick, BracketColor);
			Canvas->K2_DrawLine(BoxBottomRight, BoxBottomRight - FVector2D(0.0f, BracketLen), LineThick, BracketColor);

			// Overhead Health Bar
			float BarW = FMath::Clamp(BoxWidth * 1.2f, 50.0f * DPIScale, 120.0f * DPIScale);
			float BarH = 6.0f * DPIScale;
			float BarX = ScreenCenter.X - BarW * 0.5f;
			float BarY = ScreenHead.Y - (16.0f * DPIScale);

			// Bar Background
			FCanvasTileItem BarBG(FVector2D(BarX, BarY), FVector2D(BarW, BarH), FLinearColor(0.05f, 0.05f, 0.05f, 0.85f));
			Canvas->DrawItem(BarBG);

			// Bar Fill
			float HealthPercent = FMath::Clamp(Unit->Health / FMath::Max(1.0f, Unit->MaxHealth), 0.0f, 1.0f);
			FLinearColor HealthColor = (Unit->TeamID == 0) ?
				FMath::Lerp(FLinearColor(0.9f, 0.2f, 0.1f), FLinearColor(0.2f, 0.9f, 0.3f), HealthPercent) :
				FLinearColor(0.95f, 0.2f, 0.2f);
			FCanvasTileItem BarFill(FVector2D(BarX + 1.0f, BarY + 1.0f), FVector2D((BarW - 2.0f) * HealthPercent, BarH - 2.0f), HealthColor);
			Canvas->DrawItem(BarFill);

			// Bar Border
			FCanvasBoxItem BarBorder(FVector2D(BarX, BarY), FVector2D(BarW, BarH));
			BarBorder.SetColor(ColorBurnishedGold * 0.8f);
			BarBorder.LineThickness = 1.0f * DPIScale;
			Canvas->DrawItem(BarBorder);

			// Unit Name Tag on Hover/Select
			FString TagStr = FString::Printf(TEXT("%s (%d/%d)"), *Unit->UnitName, FMath::RoundToInt(Unit->Health), FMath::RoundToInt(Unit->MaxHealth));
			DrawParchmentText(DefaultFont, TagStr, BarX - (6.0f * DPIScale), BarY - (14.0f * DPIScale), 0.82f, BracketColor);
		}
	}

	// =========================================================================
	// 1. TOP HEADER (3 HIGH-CONTRAST TABLETS SCALED BY DPISCALE)
	// =========================================================================
	const float TopY = 12.0f * DPIScale;
	const float TopH = 44.0f * DPIScale;

	// --- 1A. TOP-LEFT: RESOURCE BAR ---
	const float ResW = 560.0f * DPIScale;
	const float ResX = 18.0f * DPIScale;
	DrawBeveledTablet(FVector2D(ResX, TopY), FVector2D(ResW, TopH), ColorBasaltBG, ColorBurnishedGold);

	int32 GrainVal = GM ? FMath::RoundToInt(GM->Grain) : 7;
	int32 BronzeVal = GM ? FMath::RoundToInt(GM->Bronze) : 201;
	int32 ClayVal = GM ? FMath::RoundToInt(GM->Clay) : 1302;
	int32 MilitaryVal = 11;
	int32 PopVal = 101;
	int32 StabilityVal = 10;

	if (GM)
	{
		TArray<AActor*> Units;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADominionUnitActor::StaticClass(), Units);
		MilitaryVal = FMath::Max(1, Units.Num());
	}
	if (Demo)
	{
		PopVal = FMath::Max(1, (int32)(Demo->GetRuralPopulation() / 120));
	}
	if (Estates)
	{
		StabilityVal = FMath::Clamp((int32)(Estates->GetMassesLoyalty() / 10.0f), 1, 10);
	}

	struct FResDisplay
	{
		const TCHAR* Tag;
		FString Value;
	};

	const FResDisplay Resources[6] = {
		{ TEXT("GRN"), FString::Printf(TEXT("%02d"), GrainVal) },
		{ TEXT("GLD"), FString::Printf(TEXT("%d"), BronzeVal) },
		{ TEXT("CLY"), FString::Printf(TEXT("%d"), ClayVal) },
		{ TEXT("WAR"), FString::Printf(TEXT("%d"), MilitaryVal) },
		{ TEXT("POP"), FString::Printf(TEXT("%d"), PopVal) },
		{ TEXT("ORD"), FString::Printf(TEXT("%d"), StabilityVal) }
	};

	const float ItemSpacing = ResW / 6.0f;
	for (int32 i = 0; i < 6; ++i)
	{
		float ItemX = ResX + i * ItemSpacing + (8.0f * DPIScale);
		DrawParchmentText(MediumFont, Resources[i].Tag, ItemX, TopY + (8.0f * DPIScale), 0.95f, ColorBurnishedGold);
		DrawParchmentText(MediumFont, Resources[i].Value, ItemX + (36.0f * DPIScale), TopY + (8.0f * DPIScale), 1.05f, ColorParchment);

		if (i < 5)
		{
			float DivX = ResX + (i + 1) * ItemSpacing;
			Canvas->K2_DrawLine(FVector2D(DivX, TopY + (8.0f * DPIScale)), FVector2D(DivX, TopY + TopH - (8.0f * DPIScale)), 1.5f * DPIScale, ColorBurnishedGold * 0.4f);
		}
	}

	// --- 1B. TOP-CENTER: ENGRAVED MONUMENTAL PLAQUE ---
	const float PlaqueW = 480.0f * DPIScale;
	const float PlaqueX = (ScreenW - PlaqueW) * 0.5f;
	DrawBeveledTablet(FVector2D(PlaqueX, TopY), FVector2D(PlaqueW, TopH), ColorBasaltBG, ColorBurnishedGold);

	// Winged Chamfers
	Canvas->K2_DrawLine(FVector2D(PlaqueX, TopY), FVector2D(PlaqueX - (12.0f * DPIScale), TopY + TopH * 0.5f), 2.5f * DPIScale, ColorBurnishedGold);
	Canvas->K2_DrawLine(FVector2D(PlaqueX - (12.0f * DPIScale), TopY + TopH * 0.5f), FVector2D(PlaqueX, TopY + TopH), 2.5f * DPIScale, ColorBurnishedGold);
	Canvas->K2_DrawLine(FVector2D(PlaqueX + PlaqueW, TopY), FVector2D(PlaqueX + PlaqueW + (12.0f * DPIScale), TopY + TopH * 0.5f), 2.5f * DPIScale, ColorBurnishedGold);
	Canvas->K2_DrawLine(FVector2D(PlaqueX + PlaqueW + (12.0f * DPIScale), TopY + TopH * 0.5f), FVector2D(PlaqueX + PlaqueW, TopY + TopH), 2.5f * DPIScale, ColorBurnishedGold);

	DrawParchmentText(MediumFont, TEXT("<>"), PlaqueX + (16.0f * DPIScale), TopY + (9.0f * DPIScale), 1.0f, ColorBurnishedGold);
	DrawParchmentText(LargeFont, TEXT("MYTHIC GRIMDARK BRONZE"), PlaqueX + (44.0f * DPIScale), TopY + (7.0f * DPIScale), 1.12f, ColorMutedGold);
	DrawParchmentText(MediumFont, TEXT("<>"), PlaqueX + PlaqueW - (36.0f * DPIScale), TopY + (9.0f * DPIScale), 1.0f, ColorBurnishedGold);

	// --- 1C. TOP-RIGHT: CONTROL CLUSTER ---
	const float CtrlW = 260.0f * DPIScale;
	const float CtrlX = ScreenW - CtrlW - (18.0f * DPIScale);
	DrawBeveledTablet(FVector2D(CtrlX, TopY), FVector2D(CtrlW, TopH), ColorBasaltBG, ColorBurnishedGold);

	bool bPaused = PC ? PC->IsTacticalPaused() : false;
	FLinearColor PauseColor = bPaused ? FLinearColor(1.0f, 0.35f, 0.25f) : ColorParchment;
	DrawParchmentText(MediumFont, TEXT("[||]"), CtrlX + (10.0f * DPIScale), TopY + (8.0f * DPIScale), 0.95f, PauseColor);
	DrawParchmentText(MediumFont, TEXT("[>] 1X"), CtrlX + (50.0f * DPIScale), TopY + (8.0f * DPIScale), 0.95f, !bPaused ? ColorMutedGold : ColorParchment * 0.6f);
	DrawParchmentText(MediumFont, TEXT("[>>] 2X"), CtrlX + (112.0f * DPIScale), TopY + (8.0f * DPIScale), 0.95f, ColorParchment * 0.6f);
	DrawParchmentText(MediumFont, TEXT("[L] LEDGER"), CtrlX + (176.0f * DPIScale), TopY + (8.0f * DPIScale), 0.95f, bShowProductionLedger ? ColorMutedGold : ColorParchment);


	// =========================================================================
	// 2. BOTTOM-RIGHT: MESOPOTAMIAN ASTROLABE CIRCULAR MINIMAP
	// =========================================================================
	const float MapRadius = 110.0f * DPIScale;
	const FVector2D MapCenter(ScreenW - MapRadius - (32.0f * DPIScale), ScreenH - MapRadius - (32.0f * DPIScale));

	// 2A. Dark Parchment Radar Disc
	DrawFilledCircle(MapCenter, MapRadius, FLinearColor(0.06f, 0.05f, 0.04f, 0.95f), 48);

	// 2B. Terrain Contours & River Canal
	Canvas->K2_DrawLine(MapCenter + FVector2D(12.0f * DPIScale, 90.0f * DPIScale), MapCenter + FVector2D(55.0f * DPIScale, 20.0f * DPIScale), 4.0f * DPIScale, ColorVerdigris);
	Canvas->K2_DrawLine(MapCenter + FVector2D(55.0f * DPIScale, 20.0f * DPIScale), MapCenter + FVector2D(90.0f * DPIScale, -25.0f * DPIScale), 4.0f * DPIScale, ColorVerdigris);

	// Citadel Compound Footprint
	FCanvasBoxItem CitadelBox(MapCenter - FVector2D(14.0f * DPIScale, 14.0f * DPIScale), FVector2D(28.0f * DPIScale, 28.0f * DPIScale));
	CitadelBox.SetColor(ColorBurnishedGold * 0.75f);
	CitadelBox.LineThickness = 2.0f * DPIScale;
	Canvas->DrawItem(CitadelBox);

	FCanvasBoxItem CitadelInner(MapCenter - FVector2D(7.0f * DPIScale, 7.0f * DPIScale), FVector2D(14.0f * DPIScale, 14.0f * DPIScale));
	CitadelInner.SetColor(ColorBurnishedGold);
	CitadelInner.LineThickness = 1.5f * DPIScale;
	Canvas->DrawItem(CitadelInner);

	// 2C. Concentric Astrolabe Rings
	DrawCircleRing(MapCenter, MapRadius + (5.0f * DPIScale), 3.5f * DPIScale, FLinearColor(0.20f, 0.15f, 0.08f, 0.95f), 64);
	DrawCircleRing(MapCenter, MapRadius, 2.5f * DPIScale, ColorBurnishedGold, 64);
	DrawCircleRing(MapCenter, MapRadius - (6.0f * DPIScale), 1.5f * DPIScale, ColorBurnishedGold * 0.5f, 64);
	DrawCircleRing(MapCenter, MapRadius * 0.55f, 1.0f * DPIScale, ColorBurnishedGold * 0.3f, 48);

	// Astrolabe Dial Ticks
	for (int32 deg = 0; deg < 360; deg += 30)
	{
		float Rad = FMath::DegreesToRadians((float)deg);
		FVector2D T1 = MapCenter + FVector2D(FMath::Cos(Rad) * (MapRadius - (6.0f * DPIScale)), FMath::Sin(Rad) * (MapRadius - (6.0f * DPIScale)));
		FVector2D T2 = MapCenter + FVector2D(FMath::Cos(Rad) * (MapRadius - (1.0f * DPIScale)), FMath::Sin(Rad) * (MapRadius - (1.0f * DPIScale)));
		Canvas->K2_DrawLine(T1, T2, 1.8f * DPIScale, ColorBurnishedGold * 0.8f);
	}

	// Cardinal Points
	DrawParchmentText(MediumFont, TEXT("N"), MapCenter.X - (6.0f * DPIScale), MapCenter.Y - MapRadius + (8.0f * DPIScale), 1.0f, ColorMutedGold);
	DrawParchmentText(MediumFont, TEXT("E"), MapCenter.X + MapRadius - (18.0f * DPIScale), MapCenter.Y - (8.0f * DPIScale), 0.9f, ColorBurnishedGold * 0.8f);
	DrawParchmentText(MediumFont, TEXT("S"), MapCenter.X - (5.0f * DPIScale), MapCenter.Y + MapRadius - (24.0f * DPIScale), 0.9f, ColorBurnishedGold * 0.8f);
	DrawParchmentText(MediumFont, TEXT("W"), MapCenter.X - MapRadius + (8.0f * DPIScale), MapCenter.Y - (8.0f * DPIScale), 0.9f, ColorBurnishedGold * 0.8f);

	// 2D. Real-Time Units Tactical Radar Blips
	TArray<AActor*> AllUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADominionUnitActor::StaticClass(), AllUnits);
	for (AActor* Act : AllUnits)
	{
		if (ADominionUnitActor* Unit = Cast<ADominionUnitActor>(Act))
		{
			FVector UnitLoc = Unit->GetActorLocation();
			float BlipX = MapCenter.X + (UnitLoc.X / (65.0f / DPIScale));
			float BlipY = MapCenter.Y + (UnitLoc.Y / (65.0f / DPIScale));

			float DistSq = FVector2D::DistSquared(FVector2D(BlipX, BlipY), MapCenter);
			if (DistSq < (MapRadius - (10.0f * DPIScale)) * (MapRadius - (10.0f * DPIScale)))
			{
				FLinearColor BlipColor = (Unit->TeamID == 0) ? FLinearColor(0.25f, 0.82f, 1.0f) : FLinearColor(0.95f, 0.25f, 0.20f);
				float BlipSize = 6.0f * DPIScale;
				FCanvasTileItem BlipItem(FVector2D(BlipX - BlipSize * 0.5f, BlipY - BlipSize * 0.5f), FVector2D(BlipSize, BlipSize), BlipColor);
				Canvas->DrawItem(BlipItem);
			}
		}
	}

	// 2E. Camera View Frustum Cone
	APawn* PlayerPawn = PC ? PC->GetPawn() : nullptr;
	if (PlayerPawn)
	{
		FVector CamLoc = PlayerPawn->GetActorLocation();
		float CamMapX = MapCenter.X + (CamLoc.X / (65.0f / DPIScale));
		float CamMapY = MapCenter.Y + (CamLoc.Y / (65.0f / DPIScale));
		FVector2D CamPt(CamMapX, CamMapY);

		float DistSq = FVector2D::DistSquared(CamPt, MapCenter);
		if (DistSq < (MapRadius - (12.0f * DPIScale)) * (MapRadius - (12.0f * DPIScale)))
		{
			float Yaw = PlayerPawn->GetActorRotation().Yaw;
			float FovHalf = 28.0f;
			float ViewLen = 38.0f * DPIScale;

			float LeftRad = FMath::DegreesToRadians(Yaw - FovHalf);
			float RightRad = FMath::DegreesToRadians(Yaw + FovHalf);
			FVector2D LeftPt = CamPt + FVector2D(FMath::Cos(LeftRad) * ViewLen, FMath::Sin(LeftRad) * ViewLen);
			FVector2D RightPt = CamPt + FVector2D(FMath::Cos(RightRad) * ViewLen, FMath::Sin(RightRad) * ViewLen);

			Canvas->K2_DrawLine(CamPt, LeftPt, 1.8f * DPIScale, ColorBurnishedGold * 0.9f);
			Canvas->K2_DrawLine(CamPt, RightPt, 1.8f * DPIScale, ColorBurnishedGold * 0.9f);
			Canvas->K2_DrawLine(LeftPt, RightPt, 1.8f * DPIScale, ColorBurnishedGold * 0.9f);
		}
	}

	// =========================================================================
	// 3. CONTEXTUAL SELECTION / COMMAND CARD (BOTTOM-CENTER)
	// =========================================================================
	const float CardW = 700.0f * DPIScale;
	const float CardH = 120.0f * DPIScale;
	const float CardX = (ScreenW - CardW) * 0.5f;
	const float CardY = ScreenH - CardH - (20.0f * DPIScale);

	if (PC && PC->GetSelectedUnits().Num() > 0)
	{
		ADominionUnitActor* SelUnit = PC->GetSelectedUnits()[0];
		if (IsValid(SelUnit))
		{
			DrawBeveledTablet(FVector2D(CardX, CardY), FVector2D(CardW, CardH), ColorBasaltBG, ColorBurnishedGold);

			// Portrait Box
			const float PortSize = 88.0f * DPIScale;
			const float PortX = CardX + (15.0f * DPIScale);
			const float PortY = CardY + (16.0f * DPIScale);
			DrawBeveledTablet(FVector2D(PortX, PortY), FVector2D(PortSize, PortSize), FLinearColor(0.08f, 0.06f, 0.05f), ColorBurnishedGold * 0.8f);
			DrawParchmentText(LargeFont, TEXT("[COHORT]"), PortX + (10.0f * DPIScale), PortY + (32.0f * DPIScale), 0.95f, ColorBurnishedGold);

			// Unit Info & Stats
			const float InfoX = PortX + PortSize + (20.0f * DPIScale);
			FString TitleStr = FString::Printf(TEXT("%s COHORT"), *SelUnit->UnitName.ToUpper());
			DrawParchmentText(LargeFont, TitleStr, InfoX, CardY + (12.0f * DPIScale), 1.15f, ColorParchment);

			FString SubStr = FString::Printf(TEXT("Rank: Veteran  |  Warriors: %d  |  Morale: %.0f%%"), PC->GetSelectedUnits().Num(), SelUnit->Morale);
			DrawParchmentText(MediumFont, SubStr, InfoX, CardY + (38.0f * DPIScale), 0.95f, ColorMutedGold);

			// Health Bar
			float HPPercent = FMath::Clamp(SelUnit->Health / SelUnit->MaxHealth, 0.0f, 1.0f);
			const float BarW = 240.0f * DPIScale;
			const float BarH = 12.0f * DPIScale;
			FCanvasTileItem BarBG(FVector2D(InfoX, CardY + (62.0f * DPIScale)), FVector2D(BarW, BarH), FLinearColor(0.12f, 0.10f, 0.08f));
			Canvas->DrawItem(BarBG);
			FCanvasTileItem BarFill(FVector2D(InfoX, CardY + (62.0f * DPIScale)), FVector2D(BarW * HPPercent, BarH), FLinearColor(0.22f, 0.85f, 0.35f));
			Canvas->DrawItem(BarFill);
			FCanvasBoxItem BarBorder(FVector2D(InfoX, CardY + (62.0f * DPIScale)), FVector2D(BarW, BarH));
			BarBorder.SetColor(ColorBurnishedGold * 0.8f);
			Canvas->DrawItem(BarBorder);

			// Combat Stats
			FString StatsLine = FString::Printf(TEXT("ATT: %.0f | DEF: %.0f | SPD: %.0f | RATIONS: %.0fs"), SelUnit->AttackPower, SelUnit->Armor, SelUnit->MoveSpeed, SelUnit->FieldRations);
			DrawParchmentText(MediumFont, StatsLine, InfoX, CardY + (82.0f * DPIScale), 0.90f, ColorParchment * 0.9f);

			// Formation Buttons
			const float FormX = CardX + CardW - (210.0f * DPIScale);
			struct FQuickBtn { const TCHAR* Key; const TCHAR* Name; };
			const FQuickBtn QuickActions[4] = {
				{ TEXT("[1]"), TEXT("PHALANX") },
				{ TEXT("[2]"), TEXT("WEDGE") },
				{ TEXT("[3]"), TEXT("SKIRMISH") },
				{ TEXT("[4]"), TEXT("LINE") }
			};

			const float QBtnW = 94.0f * DPIScale;
			const float QBtnH = 40.0f * DPIScale;
			for (int32 b = 0; b < 4; ++b)
			{
				float BX = FormX + (b % 2) * (QBtnW + (8.0f * DPIScale));
				float BY = CardY + (16.0f * DPIScale) + (b / 2) * (QBtnH + (8.0f * DPIScale));
				DrawBeveledTablet(FVector2D(BX, BY), FVector2D(QBtnW, QBtnH), FLinearColor(0.09f, 0.08f, 0.06f), ColorBurnishedGold * 0.7f);
				DrawParchmentText(MediumFont, QuickActions[b].Key, BX + (6.0f * DPIScale), BY + (4.0f * DPIScale), 0.85f, ColorMutedGold);
				DrawParchmentText(MediumFont, QuickActions[b].Name, BX + (6.0f * DPIScale), BY + (20.0f * DPIScale), 0.80f, ColorParchment);
			}
		}
	}
	else
	{
		// Master Citadel Recruitment Hub (When No Units Selected)
		DrawBeveledTablet(FVector2D(CardX, CardY), FVector2D(CardW, CardH), ColorBasaltBG, ColorBurnishedGold);

		// Left Header Info
		DrawParchmentText(LargeFont, TEXT("CITADEL COMMAND HUB"), CardX + (20.0f * DPIScale), CardY + (14.0f * DPIScale), 1.15f, ColorMutedGold);
		DrawParchmentText(MediumFont, TEXT("Recruit Cohorts & Defend Mesopotamian Citadel"), CardX + (20.0f * DPIScale), CardY + (42.0f * DPIScale), 0.90f, ColorParchment * 0.8f);
		DrawParchmentText(MediumFont, TEXT("Press [E] to summon invading Elamite Raider Wave"), CardX + (20.0f * DPIScale), CardY + (64.0f * DPIScale), 0.85f, FLinearColor(1.0f, 0.6f, 0.3f));
		DrawParchmentText(MediumFont, TEXT("WASD / Arrows to Pan  |  Scroll Wheel to Zoom"), CardX + (20.0f * DPIScale), CardY + (86.0f * DPIScale), 0.85f, ColorBurnishedGold * 0.9f);

		// 6 Recruitment / Wave Buttons
		struct FRecruitBtn { const TCHAR* Key; const TCHAR* Name; const TCHAR* Cost; };
		const FRecruitBtn RecruitBtns[6] = {
			{ TEXT("[Z]"), TEXT("SPEARMAN"), TEXT("50G 20B") },
			{ TEXT("[X]"), TEXT("SLINGER"), TEXT("40G 30C") },
			{ TEXT("[C]"), TEXT("CHARIOT"), TEXT("90G 50B") },
			{ TEXT("[V]"), TEXT("BAGGAGE"), TEXT("30G 15B") },
			{ TEXT("[E]"), TEXT("RAIDER WAVE"), TEXT("ATTACK") },
			{ TEXT("[R]"), TEXT("5x COHORT"), TEXT("250G 100B") }
		};

		const float RBtnW = 100.0f * DPIScale;
		const float RBtnH = 44.0f * DPIScale;
		const float RBtnGridX = CardX + CardW - (335.0f * DPIScale);

		for (int32 i = 0; i < 6; ++i)
		{
			float BX = RBtnGridX + (i % 3) * (RBtnW + (6.0f * DPIScale));
			float BY = CardY + (14.0f * DPIScale) + (i / 3) * (RBtnH + (8.0f * DPIScale));
			bool bIsWave = (i == 4);
			FLinearColor BtnBorder = bIsWave ? FLinearColor(0.85f, 0.25f, 0.2f) : ColorBurnishedGold * 0.75f;
			FLinearColor BtnBG = bIsWave ? FLinearColor(0.16f, 0.06f, 0.05f) : FLinearColor(0.09f, 0.08f, 0.06f);

			DrawBeveledTablet(FVector2D(BX, BY), FVector2D(RBtnW, RBtnH), BtnBG, BtnBorder);
			DrawParchmentText(MediumFont, RecruitBtns[i].Key, BX + (6.0f * DPIScale), BY + (4.0f * DPIScale), 0.85f, bIsWave ? FLinearColor(1.0f, 0.4f, 0.3f) : ColorMutedGold);
			DrawParchmentText(MediumFont, RecruitBtns[i].Name, BX + (6.0f * DPIScale), BY + (20.0f * DPIScale), 0.75f, ColorParchment);
			DrawParchmentText(MediumFont, RecruitBtns[i].Cost, BX + (6.0f * DPIScale), BY + (32.0f * DPIScale), 0.65f, ColorMutedGold * 0.8f);
		}
	}

	// =========================================================================
	// 4. IMPERIAL MANAGEMENT LEDGER DRAWER
	// =========================================================================
	if (bShowProductionLedger)
	{
		const float DrawerW = 860.0f * DPIScale;
		const float DrawerH = 320.0f * DPIScale;
		const float DrawerX = (ScreenW - DrawerW) * 0.5f;
		const float DrawerY = TopY + TopH + (16.0f * DPIScale);

		DrawBeveledTablet(FVector2D(DrawerX, DrawerY), FVector2D(DrawerW, DrawerH), FLinearColor(0.06f, 0.05f, 0.04f, 0.98f), ColorBurnishedGold);

		DrawParchmentText(LargeFont, TEXT("[ IMPERIAL MANAGEMENT LEDGER ]"), DrawerX + (24.0f * DPIScale), DrawerY + (16.0f * DPIScale), 1.15f, ColorMutedGold);
		DrawParchmentText(MediumFont, TEXT("[Press Tab / L to Toggle  |  Click Tab to Switch]"), DrawerX + DrawerW - (340.0f * DPIScale), DrawerY + (18.0f * DPIScale), 0.90f, ColorParchment * 0.75f);

		// Tabs
		const TCHAR* TabNames[4] = { TEXT("Production"), TEXT("Necessities"), TEXT("Demographics"), TEXT("Logistics") };
		const float TabW = 195.0f * DPIScale;
		const float TabH = 34.0f * DPIScale;
		const float TabY = DrawerY + (52.0f * DPIScale);

		for (int32 t = 0; t < 4; ++t)
		{
			bool bActive = (ActiveLedgerTab == t);
			float TX = DrawerX + (20.0f * DPIScale) + t * (TabW + (8.0f * DPIScale));
			FLinearColor TabBG = bActive ? FLinearColor(0.18f, 0.14f, 0.08f) : FLinearColor(0.08f, 0.07f, 0.05f);
			FLinearColor TabBorder = bActive ? ColorBurnishedGold : ColorBurnishedGold * 0.4f;
			DrawBeveledTablet(FVector2D(TX, TabY), FVector2D(TabW, TabH), TabBG, TabBorder);
			DrawParchmentText(MediumFont, TabNames[t], TX + (16.0f * DPIScale), TabY + (8.0f * DPIScale), 0.95f, bActive ? ColorMutedGold : ColorParchment * 0.75f);
		}

		// Content Area
		const float ContentY = TabY + TabH + (16.0f * DPIScale);
		if (ActiveLedgerTab == 1)
		{
			DrawParchmentText(MediumFont, TEXT("CANALS & WATER: 92% COVERAGE"), DrawerX + (32.0f * DPIScale), ContentY + (12.0f * DPIScale), 1.0f, FLinearColor(0.35f, 0.75f, 0.95f));
			DrawParchmentText(MediumFont, TEXT("- Euphrates Canal Aqueducts active  |  Silt Soil Fertility: 100%"), DrawerX + (44.0f * DPIScale), ContentY + (36.0f * DPIScale), 0.90f, ColorParchment);

			DrawParchmentText(MediumFont, TEXT("SHELTER & HOUSING: 88% FULFILLED"), DrawerX + (32.0f * DPIScale), ContentY + (70.0f * DPIScale), 1.0f, ColorMutedGold);
			DrawParchmentText(MediumFont, TEXT("- Mudbrick Tenements: 13.2k sheltered  |  Unhoused: 450 serfs"), DrawerX + (44.0f * DPIScale), ContentY + (94.0f * DPIScale), 0.90f, ColorParchment);

			DrawParchmentText(MediumFont, TEXT("HEARTH & TIMBER: 180 DAYS STOCK"), DrawerX + (32.0f * DPIScale), ContentY + (128.0f * DPIScale), 1.0f, FLinearColor(0.85f, 0.55f, 0.25f));
			DrawParchmentText(MediumFont, TEXT("- Kiln & brazier fuel buffer secure"), DrawerX + (44.0f * DPIScale), ContentY + (152.0f * DPIScale), 0.90f, ColorParchment);
		}
		else if (ActiveLedgerTab == 0)
		{
			DrawParchmentText(MediumFont, TEXT("GRAIN (1,420)  ->  FLOUR & BREAD (850)  ->  ARMY RATIONS (42 Days)"), DrawerX + (32.0f * DPIScale), ContentY + (20.0f * DPIScale), 1.05f, ColorMutedGold);
			DrawParchmentText(MediumFont, TEXT("Harvest: +45/min  |  Milling: 22/min  |  Net Surplus: +7/m"), DrawerX + (32.0f * DPIScale), ContentY + (50.0f * DPIScale), 0.95f, ColorParchment);

			DrawParchmentText(MediumFont, TEXT("COPPER/TIN (920)  ->  BRONZE INGOTS (650)  ->  WEAPONS & ARMOR (450)"), DrawerX + (32.0f * DPIScale), ContentY + (95.0f * DPIScale), 1.05f, ColorMutedGold);
			DrawParchmentText(MediumFont, TEXT("Mining: +24/min  |  Smelting: 16/min  |  Armory Output: 10/min"), DrawerX + (32.0f * DPIScale), ContentY + (125.0f * DPIScale), 0.95f, ColorParchment);
		}
		else if (ActiveLedgerTab == 2)
		{
			int64 RuralPop = Demo ? Demo->GetRuralPopulation() : 12000;
			int32 DraftedCohorts = Demo ? Demo->GetActiveDraftedCohorts() : 0;
			FString SocialStr = FString::Printf(TEXT("SOCIAL CLASSES: Patricians: 300 | Artisans: 1.2k | Serfs: %lld (Drafted: %d)"), RuralPop, DraftedCohorts);
			DrawParchmentText(MediumFont, SocialStr, DrawerX + (32.0f * DPIScale), ContentY + (20.0f * DPIScale), 1.0f, ColorMutedGold);
			DrawParchmentText(MediumFont, TEXT("AGE PYRAMID: Apprentices: 4,200 | Prime Laborers: 6,500 | Elders: 800"), DrawerX + (32.0f * DPIScale), ContentY + (58.0f * DPIScale), 1.0f, ColorParchment);
			DrawParchmentText(MediumFont, TEXT("MIGRATION: City Air Makes You Free (1 Year Resettlement)"), DrawerX + (32.0f * DPIScale), ContentY + (96.0f * DPIScale), 1.0f, FLinearColor(0.4f, 0.8f, 1.0f));
		}
		else
		{
			DrawParchmentText(MediumFont, TEXT("BAGGAGE TRAIN NETWORK: 100% Efficiency (Active Ox-Carts: 4)"), DrawerX + (32.0f * DPIScale), ContentY + (20.0f * DPIScale), 1.05f, ColorMutedGold);
			DrawParchmentText(MediumFont, TEXT("EUPHRATES RIVER DOCKS: 2 Cargo Barges active (+60t freight capacity)"), DrawerX + (32.0f * DPIScale), ContentY + (58.0f * DPIScale), 1.0f, ColorParchment);
			DrawParchmentText(MediumFont, TEXT("PROVINCIAL DIRECTIVE: Maintain 60 Days Food Buffer in Citadel Granary"), DrawerX + (32.0f * DPIScale), ContentY + (96.0f * DPIScale), 1.0f, FLinearColor(0.35f, 0.85f, 0.45f));
		}
	}

	// =========================================================================
	// 5. DRAG-SELECTION MARQUEE BOX (AOE2 STYLE EMERALD & GOLD)
	// =========================================================================
	DrawSelectionMarquee();
}

bool ADominionRTSHUD::HandleClick(float MouseX, float MouseY)
{
	if (!Canvas) return false;

	const float ScreenW = Canvas->SizeX;
	const float ScreenH = Canvas->SizeY;
	const float DPIScale = FMath::Clamp(ScreenH / 1080.0f, 1.0f, 2.5f);

	const float TopY = 12.0f * DPIScale;
	const float TopH = 44.0f * DPIScale;

	ADominionGameModeBase* GM = Cast<ADominionGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	ADominionRTSPlayerController* PC = Cast<ADominionRTSPlayerController>(GetOwningPlayerController());

	// Top-Right Speed & Control Cluster Click Detection
	const float CtrlW = 260.0f * DPIScale;
	const float CtrlX = ScreenW - CtrlW - (18.0f * DPIScale);
	if (MouseX >= CtrlX && MouseX <= CtrlX + CtrlW && MouseY >= TopY && MouseY <= TopY + TopH)
	{
		float RelX = MouseX - CtrlX;
		if (RelX < 45.0f * DPIScale)
		{
			if (PC) PC->ToggleTacticalPause();
			return true;
		}
		else if (RelX >= 165.0f * DPIScale && RelX <= 258.0f * DPIScale)
		{
			ToggleProductionLedger();
			return true;
		}
	}

	const float CardW = 700.0f * DPIScale;
	const float CardH = 120.0f * DPIScale;
	const float CardX = (ScreenW - CardW) * 0.5f;
	const float CardY = ScreenH - CardH - (20.0f * DPIScale);

	// Selection Card Formation Buttons Click Detection
	if (PC && PC->GetSelectedUnits().Num() > 0)
	{
		const float FormX = CardX + CardW - (210.0f * DPIScale);
		const float QBtnW = 94.0f * DPIScale;
		const float QBtnH = 40.0f * DPIScale;

		for (int32 b = 0; b < 4; ++b)
		{
			float BX = FormX + (b % 2) * (QBtnW + (8.0f * DPIScale));
			float BY = CardY + (16.0f * DPIScale) + (b / 2) * (QBtnH + (8.0f * DPIScale));
			if (MouseX >= BX && MouseX <= BX + QBtnW && MouseY >= BY && MouseY <= BY + QBtnH)
			{
				PC->SetSelectedUnitsFormation(b);
				return true;
			}
		}
	}
	else if (GM)
	{
		// Unselected Citadel Command Hub Buttons Click Detection
		const float RBtnW = 100.0f * DPIScale;
		const float RBtnH = 44.0f * DPIScale;
		const float RBtnGridX = CardX + CardW - (335.0f * DPIScale);

		for (int32 i = 0; i < 6; ++i)
		{
			float BX = RBtnGridX + (i % 3) * (RBtnW + (6.0f * DPIScale));
			float BY = CardY + (14.0f * DPIScale) + (i / 3) * (RBtnH + (8.0f * DPIScale));
			if (MouseX >= BX && MouseX <= BX + RBtnW && MouseY >= BY && MouseY <= BY + RBtnH)
			{
				if (i == 0) GM->SpawnSpearman(0);
				else if (i == 1) GM->SpawnSlinger(0);
				else if (i == 2) GM->SpawnChariot(0);
				else if (i == 3) GM->SpawnBaggageTrain(0);
				else if (i == 4) GM->SpawnEnemyWave(3, 2);
				else if (i == 5) GM->SpawnSpearmenBatch(5, 0);
				return true;
			}
		}
	}

	// Ledger Tab Click Detection
	if (bShowProductionLedger)
	{
		const float DrawerW = 860.0f * DPIScale;
		const float DrawerX = (ScreenW - DrawerW) * 0.5f;
		const float TabW = 195.0f * DPIScale;
		const float TabH = 34.0f * DPIScale;
		const float TabY = TopY + TopH + (16.0f * DPIScale) + (52.0f * DPIScale);

		if (MouseY >= TabY && MouseY <= TabY + TabH)
		{
			for (int32 t = 0; t < 4; ++t)
			{
				float TX = DrawerX + (20.0f * DPIScale) + t * (TabW + (8.0f * DPIScale));
				if (MouseX >= TX && MouseX <= TX + TabW)
				{
					SetActiveLedgerTab(t);
					return true;
				}
			}
		}
	}

	return false;
}

void ADominionRTSHUD::DrawSelectionMarquee()
{
	if (!Canvas) return;

	ADominionRTSPlayerController* PC = Cast<ADominionRTSPlayerController>(GetOwningPlayerController());
	if (!PC || !PC->IsMarqueeSelecting()) return;

	float MouseX = 0.0f, MouseY = 0.0f;
	if (!PC->GetMousePosition(MouseX, MouseY)) return;

	FVector2D StartPos = PC->GetMarqueeStartPos();
	FVector2D EndPos(MouseX, MouseY);

	float DragDist = FVector2D::Distance(StartPos, EndPos);
	if (DragDist < 4.0f) return;

	const float MinX = FMath::Min(StartPos.X, EndPos.X);
	const float MaxX = FMath::Max(StartPos.X, EndPos.X);
	const float MinY = FMath::Min(StartPos.Y, EndPos.Y);
	const float MaxY = FMath::Max(StartPos.Y, EndPos.Y);
	const FVector2D BoxPos(MinX, MinY);
	const FVector2D BoxSize(MaxX - MinX, MaxY - MinY);

	const float ScreenH = Canvas->SizeY;
	const float DPIScale = FMath::Clamp(ScreenH / 1080.0f, 1.0f, 2.5f);

	// 1. Translucent AoE2 Green Fill
	FCanvasTileItem FillItem(BoxPos, BoxSize, FLinearColor(0.18f, 0.85f, 0.35f, 0.18f));
	FillItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(FillItem);

	// 2. High-Visibility Gold / Emerald Border Box
	FCanvasBoxItem BorderItem(BoxPos, BoxSize);
	BorderItem.SetColor(FLinearColor(0.98f, 0.86f, 0.35f, 0.95f));
	BorderItem.LineThickness = 1.8f * DPIScale;
	Canvas->DrawItem(BorderItem);

	// 3. Inner accent border line
	if (BoxSize.X > 6.0f && BoxSize.Y > 6.0f)
	{
		FCanvasBoxItem InnerBorder(BoxPos + FVector2D(1.5f * DPIScale, 1.5f * DPIScale), BoxSize - FVector2D(3.0f * DPIScale, 3.0f * DPIScale));
		InnerBorder.SetColor(FLinearColor(0.2f, 0.95f, 0.4f, 0.6f));
		InnerBorder.LineThickness = 1.0f * DPIScale;
		Canvas->DrawItem(InnerBorder);
	}
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
	ADominionGameModeBase* GM = Cast<ADominionGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM) return false;

	if (Hotkey.Equals(TEXT("Q"), ESearchCase::IgnoreCase))
	{
		GM->SpawnSpearman(0);
		return true;
	}
	else if (Hotkey.Equals(TEXT("W"), ESearchCase::IgnoreCase))
	{
		GM->SpawnSlinger(0);
		return true;
	}
	else if (Hotkey.Equals(TEXT("E"), ESearchCase::IgnoreCase))
	{
		GM->SpawnEnemyDummy();
		return true;
	}
	else if (Hotkey.Equals(TEXT("R"), ESearchCase::IgnoreCase))
	{
		GM->SpawnSpearmenBatch(5, 0);
		return true;
	}
	return false;
}


