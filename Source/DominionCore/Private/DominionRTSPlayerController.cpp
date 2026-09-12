#include "DominionRTSPlayerController.h"
#include "DominionGameModeBase.h"
#include "DominionRTSPawn.h"
#include "DominionUnitActor.h"
#include "DominionBuildingActor.h"
#include "DominionTutorialSubsystem.h"
#include "DominionFormationSystem.h"
#include "DominionSupplyLineSubsystem.h"
#include "DominionDemographicsSubsystem.h"
#include "DominionRTSHUD.h"
#include "DominionPoliticalEstatesSystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "UnrealClient.h"
#include "Misc/CommandLine.h"

ADominionRTSPlayerController::ADominionRTSPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	bShouldPerformFullTickWhenPaused = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ADominionRTSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);

	EnsureRTSPawn();

	// Automated In-Engine Visual Test Pipeline
	FString ScreenshotPath;
	if (FParse::Value(FCommandLine::Get(), TEXT("CaptureScreenshot="), ScreenshotPath))
	{
		FTimerHandle CaptureTimerHandle;
		GetWorldTimerManager().SetTimer(CaptureTimerHandle, [this, ScreenshotPath]()
		{
			FScreenshotRequest::RequestScreenshot(ScreenshotPath, false, false);
			
			FTimerHandle ExitTimerHandle;
			GetWorldTimerManager().SetTimer(ExitTimerHandle, []()
			{
				FGenericPlatformMisc::RequestExit(false);
			}, 1.2f, false);
		}, 2.5f, false);
	}
}

ADominionRTSPawn* ADominionRTSPlayerController::EnsureRTSPawn()
{
	ADominionRTSPawn* RTSPawn = Cast<ADominionRTSPawn>(GetPawn());
	if (RTSPawn && IsValid(RTSPawn))
	{
		return RTSPawn;
	}

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	// 1. Destroy any rogue default characters (e.g. TopDownCharacter)
	TArray<AActor*> AllPawns;
	UGameplayStatics::GetAllActorsOfClass(World, APawn::StaticClass(), AllPawns);
	for (AActor* Act : AllPawns)
	{
		if (Act && !Act->IsA<ADominionRTSPawn>())
		{
			FString ActorName = Act->GetName();
			if (ActorName.Contains(TEXT("TopDownCharacter")) || ActorName.Contains(TEXT("BP_TopDown")))
			{
				Act->Destroy();
			}
		}
	}

	// 2. Find existing ADominionRTSPawn in level
	TArray<AActor*> FoundRTSPawns;
	UGameplayStatics::GetAllActorsOfClass(World, ADominionRTSPawn::StaticClass(), FoundRTSPawns);
	for (AActor* Act : FoundRTSPawns)
	{
		if (ADominionRTSPawn* Candidate = Cast<ADominionRTSPawn>(Act))
		{
			Possess(Candidate);
			return Candidate;
		}
	}

	// 3. Spawn a dedicated ADominionRTSPawn at ideal tactical elevation
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	RTSPawn = World->SpawnActor<ADominionRTSPawn>(ADominionRTSPawn::StaticClass(), FVector(-750.0f, -750.0f, 150.0f), FRotator(0.0f, 45.0f, 0.0f), SpawnParams);
	if (RTSPawn)
	{
		Possess(RTSPawn);
	}
	return RTSPawn;
}

void ADominionRTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		// Camera Directional Movement Bindings (WASD + Arrows)
		InputComponent->BindKey(EKeys::W, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveForwardPressed);
		InputComponent->BindKey(EKeys::W, IE_Released, this, &ADominionRTSPlayerController::OnMoveForwardReleased);
		InputComponent->BindKey(EKeys::S, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveBackwardPressed);
		InputComponent->BindKey(EKeys::S, IE_Released, this, &ADominionRTSPlayerController::OnMoveBackwardReleased);
		InputComponent->BindKey(EKeys::A, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveLeftPressed);
		InputComponent->BindKey(EKeys::A, IE_Released, this, &ADominionRTSPlayerController::OnMoveLeftReleased);
		InputComponent->BindKey(EKeys::D, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveRightPressed);
		InputComponent->BindKey(EKeys::D, IE_Released, this, &ADominionRTSPlayerController::OnMoveRightReleased);

		InputComponent->BindKey(EKeys::Up, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveForwardPressed);
		InputComponent->BindKey(EKeys::Up, IE_Released, this, &ADominionRTSPlayerController::OnMoveForwardReleased);
		InputComponent->BindKey(EKeys::Down, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveBackwardPressed);
		InputComponent->BindKey(EKeys::Down, IE_Released, this, &ADominionRTSPlayerController::OnMoveBackwardReleased);
		InputComponent->BindKey(EKeys::Left, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveLeftPressed);
		InputComponent->BindKey(EKeys::Left, IE_Released, this, &ADominionRTSPlayerController::OnMoveLeftReleased);
		InputComponent->BindKey(EKeys::Right, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveRightPressed);
		InputComponent->BindKey(EKeys::Right, IE_Released, this, &ADominionRTSPlayerController::OnMoveRightReleased);

		// Mouse Clicks
		InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ADominionRTSPlayerController::OnLeftClickPressed);
		InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &ADominionRTSPlayerController::OnLeftClickReleased);
		InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &ADominionRTSPlayerController::OnRightClickPressed);

		// Camera Zoom & Middle Mouse Drag
		InputComponent->BindKey(EKeys::MouseScrollUp, IE_Pressed, this, &ADominionRTSPlayerController::OnZoomIn);
		InputComponent->BindKey(EKeys::MouseScrollDown, IE_Pressed, this, &ADominionRTSPlayerController::OnZoomOut);
		InputComponent->BindKey(EKeys::PageUp, IE_Pressed, this, &ADominionRTSPlayerController::OnZoomIn);
		InputComponent->BindKey(EKeys::PageDown, IE_Pressed, this, &ADominionRTSPlayerController::OnZoomOut);
		InputComponent->BindKey(EKeys::MiddleMouseButton, IE_Pressed, this, &ADominionRTSPlayerController::OnMiddleMousePressed);
		InputComponent->BindKey(EKeys::MiddleMouseButton, IE_Released, this, &ADominionRTSPlayerController::OnMiddleMouseReleased);

		// Tactical Formation Hotkeys (1, 2, 3, 4)
		InputComponent->BindKey(EKeys::One, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_1);
		InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_2);
		InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_3);
		InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_4);

		// Unit Training & Tactical Command Hotkeys (Z, X, C, V, E, R, T, Spacebar)
		InputComponent->BindKey(EKeys::Z, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_Z);
		InputComponent->BindKey(EKeys::X, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_X);
		InputComponent->BindKey(EKeys::C, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_C);
		InputComponent->BindKey(EKeys::V, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_V);
		InputComponent->BindKey(EKeys::E, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_E);
		InputComponent->BindKey(EKeys::R, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_R);
		InputComponent->BindKey(EKeys::T, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_T);
		InputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_Space);

		// Imperial Edict Hotkeys (F1, F2, F3, F4, L, Tab)
		InputComponent->BindKey(EKeys::F1, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_F1);
		InputComponent->BindKey(EKeys::F2, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_F2);
		InputComponent->BindKey(EKeys::F3, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_F3);
		InputComponent->BindKey(EKeys::F4, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_F4);
		InputComponent->BindKey(EKeys::L, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_L);
		InputComponent->BindKey(EKeys::Tab, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_L);
	}
}

void ADominionRTSPlayerController::OnMoveForwardPressed()  { bMoveForward = true; }
void ADominionRTSPlayerController::OnMoveForwardReleased() { bMoveForward = false; }
void ADominionRTSPlayerController::OnMoveBackwardPressed() { bMoveBackward = true; }
void ADominionRTSPlayerController::OnMoveBackwardReleased(){ bMoveBackward = false; }
void ADominionRTSPlayerController::OnMoveLeftPressed()     { bMoveLeft = true; }
void ADominionRTSPlayerController::OnMoveLeftReleased()    { bMoveLeft = false; }
void ADominionRTSPlayerController::OnMoveRightPressed()    { bMoveRight = true; }
void ADominionRTSPlayerController::OnMoveRightReleased()   { bMoveRight = false; }

void ADominionRTSPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	ADominionRTSPawn* RTSPawn = EnsureRTSPawn();
	if (!RTSPawn) return;

	FVector2D PanAxis(0.0f, 0.0f);

	// 1. WASD & Arrow Key Movement
	if (bMoveForward  || IsInputKeyDown(EKeys::W) || IsInputKeyDown(EKeys::Up))    PanAxis.Y += 1.0f;
	if (bMoveBackward || IsInputKeyDown(EKeys::S) || IsInputKeyDown(EKeys::Down))  PanAxis.Y -= 1.0f;
	if (bMoveLeft     || IsInputKeyDown(EKeys::A) || IsInputKeyDown(EKeys::Left))  PanAxis.X -= 1.0f;
	if (bMoveRight    || IsInputKeyDown(EKeys::D) || IsInputKeyDown(EKeys::Right)) PanAxis.X += 1.0f;

	// 2. Middle-Mouse Dragging & Screen Edge Scrolling
	float MouseX = 0.0f, MouseY = 0.0f;
	int32 ViewportW = 0, ViewportH = 0;
	GetViewportSize(ViewportW, ViewportH);

	if (GetMousePosition(MouseX, MouseY) && ViewportW > 0 && ViewportH > 0)
	{
		if (bIsMiddleMouseDragging)
		{
			FVector2D CurrentMouse(MouseX, MouseY);
			FVector2D DragDelta = CurrentMouse - LastMousePosition;
			PanAxis.X -= DragDelta.X * 0.45f;
			PanAxis.Y += DragDelta.Y * 0.45f;
			LastMousePosition = CurrentMouse;
		}
		else
		{
			// Edge Panning
			const float EdgeBorder = 18.0f;
			if (MouseX <= EdgeBorder && MouseX >= 0.0f) PanAxis.X -= 1.0f;
			else if (MouseX >= (ViewportW - EdgeBorder) && MouseX <= ViewportW) PanAxis.X += 1.0f;

			if (MouseY <= EdgeBorder && MouseY >= 0.0f) PanAxis.Y += 1.0f;
			else if (MouseY >= (ViewportH - EdgeBorder) && MouseY <= ViewportH) PanAxis.Y -= 1.0f;
		}
	}

	if (!PanAxis.IsNearlyZero())
	{
		RTSPawn->MoveCamera(PanAxis);
	}

	// 3. Hover Detection under Cursor
	FHitResult CursorHit;
	ADominionUnitActor* FoundHoveredUnit = nullptr;
	if (GetHitResultUnderCursor(ECC_Pawn, false, CursorHit))
	{
		FoundHoveredUnit = Cast<ADominionUnitActor>(CursorHit.GetActor());
	}
	if (!FoundHoveredUnit && GetHitResultUnderCursor(ECC_Visibility, false, CursorHit))
	{
		FoundHoveredUnit = Cast<ADominionUnitActor>(CursorHit.GetActor());
	}

	if (FoundHoveredUnit != HoveredUnit)
	{
		if (HoveredUnit && IsValid(HoveredUnit))
		{
			HoveredUnit->SetHovered(false);
		}
		HoveredUnit = FoundHoveredUnit;
		if (HoveredUnit && IsValid(HoveredUnit))
		{
			HoveredUnit->SetHovered(true);
		}
	}
}

void ADominionRTSPlayerController::OnZoomIn()
{
	if (ADominionRTSPawn* RTSPawn = EnsureRTSPawn())
	{
		RTSPawn->ZoomCamera(1.0f);
	}
}

void ADominionRTSPlayerController::OnZoomOut()
{
	if (ADominionRTSPawn* RTSPawn = EnsureRTSPawn())
	{
		RTSPawn->ZoomCamera(-1.0f);
	}
}

void ADominionRTSPlayerController::OnMiddleMousePressed()
{
	bIsMiddleMouseDragging = true;
	float MouseX = 0.0f, MouseY = 0.0f;
	GetMousePosition(MouseX, MouseY);
	LastMousePosition = FVector2D(MouseX, MouseY);
}

void ADominionRTSPlayerController::OnMiddleMouseReleased()
{
	bIsMiddleMouseDragging = false;
}

void ADominionRTSPlayerController::OnLeftClickPressed()
{
	float MouseX = 0.0f, MouseY = 0.0f;
	if (GetMousePosition(MouseX, MouseY))
	{
		if (ADominionRTSHUD* RTSHUD = Cast<ADominionRTSHUD>(GetHUD()))
		{
			if (RTSHUD->HandleClick(MouseX, MouseY))
			{
				bIsMarqueeSelecting = false;
				return;
			}
		}
	}

	GetMousePosition(MarqueeStartPos.X, MarqueeStartPos.Y);
	bIsMarqueeSelecting = true;
}

void ADominionRTSPlayerController::OnLeftClickReleased()
{
	if (!bIsMarqueeSelecting) return;
	bIsMarqueeSelecting = false;

	float MouseX = 0.0f, MouseY = 0.0f;
	GetMousePosition(MouseX, MouseY);
	FVector2D MarqueeEndPos(MouseX, MouseY);

	// Clear previous selection
	for (ADominionUnitActor* Unit : SelectedUnits)
	{
		if (IsValid(Unit))
		{
			Unit->SetSelected(false);
		}
	}
	SelectedUnits.Empty();

	if (SelectedBuilding && IsValid(SelectedBuilding))
	{
		SelectedBuilding->SetSelected(false);
		SelectedBuilding = nullptr;
	}

	const float DragDistance = FVector2D::Distance(MarqueeStartPos, MarqueeEndPos);
	const bool bIsBoxDrag = (DragDistance > 8.0f);

	if (bIsBoxDrag)
	{
		// Marquee Box Selection
		const float MinX = FMath::Min(MarqueeStartPos.X, MarqueeEndPos.X);
		const float MaxX = FMath::Max(MarqueeStartPos.X, MarqueeEndPos.X);
		const float MinY = FMath::Min(MarqueeStartPos.Y, MarqueeEndPos.Y);
		const float MaxY = FMath::Max(MarqueeStartPos.Y, MarqueeEndPos.Y);

		TArray<AActor*> AllUnits;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADominionUnitActor::StaticClass(), AllUnits);

		for (AActor* Act : AllUnits)
		{
			if (ADominionUnitActor* Unit = Cast<ADominionUnitActor>(Act))
			{
				if (Unit->TeamID == 0 && Unit->Health > 0.0f)
				{
					FVector2D ScreenPos;
					if (ProjectWorldLocationToScreen(Unit->GetActorLocation(), ScreenPos))
					{
						if (ScreenPos.X >= MinX && ScreenPos.X <= MaxX && ScreenPos.Y >= MinY && ScreenPos.Y <= MaxY)
						{
							Unit->SetSelected(true);
							SelectedUnits.Add(Unit);
						}
					}
				}
			}
		}

		// Box drag completed: returns with either newly selected units or empty selection (clean deselect)
		return;
	}

	// Single Click Selection: 1. Trace directly for Unit under cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Pawn, false, Hit);
	if (Hit.bBlockingHit && Hit.GetActor())
	{
		if (ADominionUnitActor* ClickedUnit = Cast<ADominionUnitActor>(Hit.GetActor()))
		{
			ClickedUnit->SetSelected(true);
			SelectedUnits.Add(ClickedUnit);
			return;
		}
	}

	// 2. Trace Visibility for Unit or Building
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	if (Hit.bBlockingHit && Hit.GetActor())
	{
		if (ADominionUnitActor* ClickedUnit = Cast<ADominionUnitActor>(Hit.GetActor()))
		{
			ClickedUnit->SetSelected(true);
			SelectedUnits.Add(ClickedUnit);
			return;
		}
		if (ADominionBuildingActor* ClickedBuilding = Cast<ADominionBuildingActor>(Hit.GetActor()))
		{
			ClickedBuilding->SetSelected(true);
			SelectedBuilding = ClickedBuilding;
			return;
		}
	}

	// 3. Fallback: Raycast to terrain plane (Z = 0) with tight tolerance (~150cm)
	FVector WorldOrigin, WorldDir;
	if (DeprojectMousePositionToWorld(WorldOrigin, WorldDir))
	{
		if (FMath::Abs(WorldDir.Z) > 0.0001f)
		{
			float T = -WorldOrigin.Z / WorldDir.Z;
			if (T > 0.0f)
			{
				FVector GroundPoint = WorldOrigin + WorldDir * T;
				TArray<AActor*> AllUnits;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADominionUnitActor::StaticClass(), AllUnits);

				float NearestDist = 150.0f; // Tight tolerance ~1.5 meters around clicked spot
				ADominionUnitActor* BestUnit = nullptr;
				for (AActor* Act : AllUnits)
				{
					if (ADominionUnitActor* Unit = Cast<ADominionUnitActor>(Act))
					{
						if (Unit->TeamID == 0 && Unit->Health > 0.0f)
						{
							float D = FVector::Dist2D(Unit->GetActorLocation(), GroundPoint);
							if (D < NearestDist)
							{
								NearestDist = D;
								BestUnit = Unit;
							}
						}
					}
				}

				if (BestUnit)
				{
					BestUnit->SetSelected(true);
					SelectedUnits.Add(BestUnit);
				}
			}
		}
	}
}

void ADominionRTSPlayerController::OnRightClickPressed()
{
	if (SelectedUnits.Num() == 0) return;

	// 1. Check if targeting an enemy unit
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Pawn, false, Hit);
	if (Hit.bBlockingHit && Hit.GetActor())
	{
		if (ADominionUnitActor* TargetUnit = Cast<ADominionUnitActor>(Hit.GetActor()))
		{
			if (TargetUnit->TeamID != 0 && TargetUnit->Health > 0.0f)
			{
				OrderAttackTarget(TargetUnit);
				return;
			}
		}
	}

	// 2. Trace terrain visibility surface
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	if (Hit.bBlockingHit)
	{
		OrderMoveSelectedUnits(Hit.Location);
		return;
	}

	// 3. Ground plane mathematical intersection (Z = 0)
	FVector WorldOrigin, WorldDir;
	if (DeprojectMousePositionToWorld(WorldOrigin, WorldDir))
	{
		if (FMath::Abs(WorldDir.Z) > 0.0001f)
		{
			float T = -WorldOrigin.Z / WorldDir.Z;
			if (T > 0.0f)
			{
				FVector GroundTarget = WorldOrigin + WorldDir * T;
				OrderMoveSelectedUnits(GroundTarget);
			}
		}
	}
}

void ADominionRTSPlayerController::OrderMoveSelectedUnits(const FVector& TargetLocation)
{
	if (SelectedUnits.Num() == 0) return;

	if (UDominionTutorialSubsystem* Tut = GetWorld()->GetSubsystem<UDominionTutorialSubsystem>())
	{
		Tut->NotifyMovementCommand();
	}

	// Visual Tactical Move Waypoint Indicator (Green Ring)
	if (UWorld* World = GetWorld())
	{
		DrawDebugCircle(World, TargetLocation + FVector(0.0f, 0.0f, 6.0f), 65.0f, 24, FColor(50, 220, 90), false, 0.75f, 0, 3.5f, FVector(1, 0, 0), FVector(0, 1, 0), false);
		DrawDebugCircle(World, TargetLocation + FVector(0.0f, 0.0f, 6.0f), 25.0f, 16, FColor(180, 255, 180), false, 0.75f, 0, 2.0f, FVector(1, 0, 0), FVector(0, 1, 0), false);
	}

	// 1. Separate combat units and baggage wagons
	TArray<ADominionUnitActor*> CombatUnits;
	TArray<ADominionUnitActor*> BaggageWagons;
	FVector Centroid = FVector::ZeroVector;

	for (ADominionUnitActor* Unit : SelectedUnits)
	{
		if (IsValid(Unit))
		{
			if (Unit->UnitType == EDominionUnitType::OxCartSupply)
			{
				BaggageWagons.Add(Unit);
			}
			else
			{
				CombatUnits.Add(Unit);
				Centroid += Unit->GetActorLocation();
			}
		}
	}

	if (CombatUnits.Num() > 0)
	{
		Centroid /= (float)CombatUnits.Num();
	}
	else
	{
		Centroid = SelectedUnits[0]->GetActorLocation();
	}

	// 2. Compute Formation Marching Heading Vector
	FVector MoveHeading = (TargetLocation - Centroid);
	MoveHeading.Z = 0.0f;
	if (MoveHeading.IsNearlyZero())
	{
		MoveHeading = FVector::ForwardVector;
	}

	// 3. Request High-Fidelity Formation Slots from Formation System
	UDominionFormationSystem* FormationSys = GetWorld()->GetSubsystem<UDominionFormationSystem>();
	EFormationType FormType = (EFormationType)(CurrentFormationMode + 1); // 1=Phalanx, 2=Wedge, 3=Skirmish, 4=Square

	TArray<FVector> SlotLocations;
	TArray<FRotator> SlotRotations;

	if (FormationSys && CombatUnits.Num() > 0)
	{
		FormationSys->CalculateFormationSlots(
			FormType,
			CombatUnits.Num(),
			TargetLocation,
			MoveHeading,
			SlotLocations,
			SlotRotations
		);

		for (int32 i = 0; i < CombatUnits.Num(); ++i)
		{
			if (i < SlotLocations.Num())
			{
				CombatUnits[i]->MoveToFormationSlot(SlotLocations[i], SlotRotations[i]);
			}
		}
	}
	else
	{
		for (ADominionUnitActor* Unit : CombatUnits)
		{
			Unit->MoveToLocation(TargetLocation);
		}
	}

	// 4. Ox-Cart Baggage Train Following Logic (Positioned 800cm behind rearmost rank)
	FVector RearwardDir = -MoveHeading.GetSafeNormal();
	for (int32 w = 0; w < BaggageWagons.Num(); ++w)
	{
		FVector CartSlot = TargetLocation + (RearwardDir * (800.0f + w * 250.0f));
		BaggageWagons[w]->MoveToFormationSlot(CartSlot, MoveHeading.Rotation());
	}
}

void ADominionRTSPlayerController::OrderAttackTarget(ADominionUnitActor* TargetUnit)
{
	if (UDominionTutorialSubsystem* Tut = GetWorld()->GetSubsystem<UDominionTutorialSubsystem>())
	{
		Tut->NotifyCombatStarted();
	}

	// Visual Tactical Attack Target Indicator (Red Ring)
	if (TargetUnit && IsValid(TargetUnit))
	{
		if (UWorld* World = GetWorld())
		{
			DrawDebugCircle(World, TargetUnit->GetActorLocation() + FVector(0.0f, 0.0f, 6.0f), 85.0f, 24, FColor(240, 45, 35), false, 0.75f, 0, 4.0f, FVector(1, 0, 0), FVector(0, 1, 0), false);
			DrawDebugCircle(World, TargetUnit->GetActorLocation() + FVector(0.0f, 0.0f, 6.0f), 35.0f, 16, FColor(255, 140, 140), false, 0.75f, 0, 2.5f, FVector(1, 0, 0), FVector(0, 1, 0), false);
		}
	}

	for (ADominionUnitActor* Unit : SelectedUnits)
	{
		if (IsValid(Unit))
		{
			Unit->AttackTarget(TargetUnit);
		}
	}
}

void ADominionRTSPlayerController::SetSelectedUnitsFormation(int32 FormationMode)
{
	CurrentFormationMode = FMath::Clamp(FormationMode, 0, 3);

	const TCHAR* FormNames[] = { TEXT("Phalanx Shield Wall"), TEXT("Wedge Shock Charge"), TEXT("Skirmish Dispersion"), TEXT("Square Perimeter") };
	if (UDominionTutorialSubsystem* Tut = GetWorld()->GetSubsystem<UDominionTutorialSubsystem>())
	{
		Tut->NotifyFormationTriggered(FormNames[CurrentFormationMode]);
	}

	for (ADominionUnitActor* Unit : SelectedUnits)
	{
		if (IsValid(Unit))
		{
			Unit->bInShieldWall = (CurrentFormationMode == 0);
		}
	}

	if (SelectedUnits.Num() > 0)
	{
		FVector Centroid = FVector::ZeroVector;
		for (ADominionUnitActor* Unit : SelectedUnits)
		{
			if (IsValid(Unit)) Centroid += Unit->GetActorLocation();
		}
		Centroid /= (float)SelectedUnits.Num();
		OrderMoveSelectedUnits(Centroid + FVector(50.0f, 0.0f, 0.0f));
	}
}

void ADominionRTSPlayerController::TrainUnit(int32 UnitTypeIndex)
{
	ADominionGameModeBase* GM = Cast<ADominionGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM) return;

	if (UnitTypeIndex == 0)
	{
		GM->SpawnSpearman(0);
	}
	else if (UnitTypeIndex == 1)
	{
		GM->SpawnSlinger(0);
	}
	else if (UnitTypeIndex == 2)
	{
		GM->SpawnChariot(0);
	}
	else if (UnitTypeIndex == 3)
	{
		GM->SpawnBaggageTrain(0);
	}
}

void ADominionRTSPlayerController::AdvanceToNextEpoch()
{
	if (SelectedBuilding && IsValid(SelectedBuilding))
	{
		uint8 CurrentStyle = (uint8)SelectedBuilding->ArchitecturalStyle;
		uint8 NextStyle = (CurrentStyle + 1) % 4;
		SelectedBuilding->SetArchitecturalStyle((EDominionArchitecturalStyle)NextStyle);
	}
}

void ADominionRTSPlayerController::ToggleTacticalPause()
{
	bIsTacticalPaused = !bIsTacticalPaused;
	UGameplayStatics::SetGamePaused(GetWorld(), bIsTacticalPaused);
}

// Tactical Hotkey Handlers
void ADominionRTSPlayerController::OnHotkey_1() { SetSelectedUnitsFormation(0); } // [1] Phalanx
void ADominionRTSPlayerController::OnHotkey_2() { SetSelectedUnitsFormation(1); } // [2] Wedge
void ADominionRTSPlayerController::OnHotkey_3() { SetSelectedUnitsFormation(2); } // [3] Skirmish
void ADominionRTSPlayerController::OnHotkey_4() { SetSelectedUnitsFormation(3); } // [4] Line/Square

void ADominionRTSPlayerController::OnHotkey_Z() // [Z] Recruit Spearman
{
	if (ADominionGameModeBase* GM = Cast<ADominionGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GM->SpawnSpearman(0);
	}
}

void ADominionRTSPlayerController::OnHotkey_X() // [X] Recruit Slinger
{
	if (ADominionGameModeBase* GM = Cast<ADominionGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GM->SpawnSlinger(0);
	}
}

void ADominionRTSPlayerController::OnHotkey_C() // [C] Recruit Heavy Chariot
{
	if (ADominionGameModeBase* GM = Cast<ADominionGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GM->SpawnChariot(0);
	}
}

void ADominionRTSPlayerController::OnHotkey_V() // [V] Recruit Baggage Wagon
{
	if (ADominionGameModeBase* GM = Cast<ADominionGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GM->SpawnBaggageTrain(0);
	}
}

void ADominionRTSPlayerController::OnHotkey_E() // [E] Summon Enemy Raider Wave
{
	if (ADominionGameModeBase* GM = Cast<ADominionGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GM->SpawnEnemyWave(3, 2);
	}
}

void ADominionRTSPlayerController::OnHotkey_R() // [R] Levy 5x Spearmen Cohort
{
	if (ADominionGameModeBase* GM = Cast<ADominionGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GM->SpawnSpearmenBatch(5, 0);
	}
}

void ADominionRTSPlayerController::OnHotkey_Space() { ToggleTacticalPause(); }

void ADominionRTSPlayerController::OnHotkey_T()
{
	if (UDominionSupplyLineSubsystem* SupplySys = GetWorld()->GetSubsystem<UDominionSupplyLineSubsystem>())
	{
		SupplySys->ToggleSupplyInterdiction();
	}

	if (ADominionRTSHUD* RTSHUD = Cast<ADominionRTSHUD>(GetHUD()))
	{
		RTSHUD->ToggleTutorial();
	}
}

// Imperial Edict Lawbook Hotkeys
void ADominionRTSPlayerController::OnHotkey_F1()
{
	if (UDominionPoliticalEstatesSystem* Estates = GetWorld()->GetSubsystem<UDominionPoliticalEstatesSystem>())
	{
		Estates->EnactEdict(EDominionEdictType::SacredGrainTithe);
	}
}

void ADominionRTSPlayerController::OnHotkey_F2()
{
	if (UDominionPoliticalEstatesSystem* Estates = GetWorld()->GetSubsystem<UDominionPoliticalEstatesSystem>())
	{
		Estates->EnactEdict(EDominionEdictType::FeudalConscription);
	}
}

void ADominionRTSPlayerController::OnHotkey_F3()
{
	if (UDominionPoliticalEstatesSystem* Estates = GetWorld()->GetSubsystem<UDominionPoliticalEstatesSystem>())
	{
		Estates->EnactEdict(EDominionEdictType::ImperialBreadDole);
	}
}

void ADominionRTSPlayerController::OnHotkey_F4()
{
	if (UDominionPoliticalEstatesSystem* Estates = GetWorld()->GetSubsystem<UDominionPoliticalEstatesSystem>())
	{
		Estates->EnactEdict(EDominionEdictType::GladiatorCircus);
	}
}

void ADominionRTSPlayerController::OnHotkey_L()
{
	if (ADominionRTSHUD* RTSHUD = Cast<ADominionRTSHUD>(GetHUD()))
	{
		RTSHUD->ToggleProductionLedger();
	}
}

void ADominionRTSPlayerController::OnHotkey_Q()
{
	OnHotkey_Z();
}

void ADominionRTSPlayerController::OnHotkey_W()
{
	OnHotkey_X();
}
