#include "DominionRTSPlayerController.h"
#include "DominionRTSPawn.h"
#include "DominionUnitActor.h"
#include "DominionBuildingActor.h"
#include "DominionTutorialSubsystem.h"
#include "DominionFormationSystem.h"
#include "DominionSupplyLineSubsystem.h"
#include "DominionRTSHUD.h"
#include "DominionPoliticalEstatesSystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ADominionRTSPlayerController::ADominionRTSPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
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
	RTSPawn = World->SpawnActor<ADominionRTSPawn>(ADominionRTSPawn::StaticClass(), FVector(-1800.0f, 0.0f, 1200.0f), FRotator::ZeroRotator, SpawnParams);
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
		// Directional Movement Bindings (WASD + Arrows)
		InputComponent->BindKey(EKeys::W, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveForwardPressed);
		InputComponent->BindKey(EKeys::W, IE_Released, this, &ADominionRTSPlayerController::OnMoveForwardReleased);
		InputComponent->BindKey(EKeys::Up, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveForwardPressed);
		InputComponent->BindKey(EKeys::Up, IE_Released, this, &ADominionRTSPlayerController::OnMoveForwardReleased);

		InputComponent->BindKey(EKeys::S, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveBackwardPressed);
		InputComponent->BindKey(EKeys::S, IE_Released, this, &ADominionRTSPlayerController::OnMoveBackwardReleased);
		InputComponent->BindKey(EKeys::Down, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveBackwardPressed);
		InputComponent->BindKey(EKeys::Down, IE_Released, this, &ADominionRTSPlayerController::OnMoveBackwardReleased);

		InputComponent->BindKey(EKeys::A, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveLeftPressed);
		InputComponent->BindKey(EKeys::A, IE_Released, this, &ADominionRTSPlayerController::OnMoveLeftReleased);
		InputComponent->BindKey(EKeys::Left, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveLeftPressed);
		InputComponent->BindKey(EKeys::Left, IE_Released, this, &ADominionRTSPlayerController::OnMoveLeftReleased);

		InputComponent->BindKey(EKeys::D, IE_Pressed, this, &ADominionRTSPlayerController::OnMoveRightPressed);
		InputComponent->BindKey(EKeys::D, IE_Released, this, &ADominionRTSPlayerController::OnMoveRightReleased);
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

		// Tactical Command Hotkeys (Z, X, C, V, T)
		InputComponent->BindKey(EKeys::Z, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_Z);
		InputComponent->BindKey(EKeys::X, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_X);
		InputComponent->BindKey(EKeys::C, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_C);
		InputComponent->BindKey(EKeys::V, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_V);
		InputComponent->BindKey(EKeys::T, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_T);

		// Imperial Edict Hotkeys (F1, F2, F3, F4, E)
		InputComponent->BindKey(EKeys::F1, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_F1);
		InputComponent->BindKey(EKeys::F2, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_F2);
		InputComponent->BindKey(EKeys::F3, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_F3);
		InputComponent->BindKey(EKeys::F4, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_F4);
		InputComponent->BindKey(EKeys::E, IE_Pressed, this, &ADominionRTSPlayerController::OnHotkey_E);
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

	// 1. WASD & Arrow Key Movement (Double check: key state flag OR IsInputKeyDown)
	if (bMoveForward  || IsInputKeyDown(EKeys::W) || IsInputKeyDown(EKeys::Up))    PanAxis.Y += 1.0f;
	if (bMoveBackward || IsInputKeyDown(EKeys::S) || IsInputKeyDown(EKeys::Down))  PanAxis.Y -= 1.0f;
	if (bMoveLeft     || IsInputKeyDown(EKeys::A) || IsInputKeyDown(EKeys::Left))  PanAxis.X -= 1.0f;
	if (bMoveRight    || IsInputKeyDown(EKeys::D) || IsInputKeyDown(EKeys::Right)) PanAxis.X += 1.0f;

	// 2. Q / E Camera Orbital Rotation
	if (IsInputKeyDown(EKeys::Q)) RTSPawn->RotateCamera(-80.0f * DeltaTime);
	if (IsInputKeyDown(EKeys::E)) RTSPawn->RotateCamera(80.0f * DeltaTime);

	// 3. Edge Scrolling & Middle-Mouse Dragging
	float MouseX = 0.0f, MouseY = 0.0f;
	int32 ViewportW = 0, ViewportH = 0;
	GetViewportSize(ViewportW, ViewportH);

	if (GetMousePosition(MouseX, MouseY) && ViewportW > 0 && ViewportH > 0)
	{
		const float EdgeMargin = 16.0f;
		if (MouseX <= EdgeMargin) PanAxis.X -= 1.0f;
		else if (MouseX >= ViewportW - EdgeMargin) PanAxis.X += 1.0f;

		if (MouseY <= EdgeMargin) PanAxis.Y += 1.0f;
		else if (MouseY >= ViewportH - EdgeMargin) PanAxis.Y -= 1.0f;

		if (bIsMiddleMouseDragging)
		{
			FVector2D CurrentMouse(MouseX, MouseY);
			FVector2D DragDelta = CurrentMouse - LastMousePosition;
			PanAxis.X -= DragDelta.X * 0.45f;
			PanAxis.Y += DragDelta.Y * 0.45f;
			LastMousePosition = CurrentMouse;
		}
	}

	if (!PanAxis.IsNearlyZero())
	{
		RTSPawn->MoveCamera(PanAxis);
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
	GetMousePosition(MarqueeStartPos.X, MarqueeStartPos.Y);
	bIsMarqueeSelecting = true;
}

void ADominionRTSPlayerController::OnLeftClickReleased()
{
	bIsMarqueeSelecting = false;

	// Clear previous unit & building selection
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

	// 1. Trace under mouse cursor for unit
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

	// 2. Trace for building
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);
	if (Hit.bBlockingHit && Hit.GetActor())
	{
		if (ADominionBuildingActor* ClickedBuilding = Cast<ADominionBuildingActor>(Hit.GetActor()))
		{
			ClickedBuilding->SetSelected(true);
			SelectedBuilding = ClickedBuilding;
			return;
		}
	}

	// 3. Fallback: Raycast to terrain ground and select nearest friendly regiment
	FVector WorldOrigin, WorldDir;
	if (DeprojectMousePositionToWorld(WorldOrigin, WorldDir))
	{
		if (FMath::Abs(WorldDir.Z) > 0.0001f)
		{
			float T = (100.0f - WorldOrigin.Z) / WorldDir.Z;
			if (T > 0.0f)
			{
				FVector GroundPoint = WorldOrigin + WorldDir * T;
				TArray<AActor*> AllUnits;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADominionUnitActor::StaticClass(), AllUnits);

				float NearestDist = 900.0f;
				ADominionUnitActor* BestUnit = nullptr;
				for (AActor* Act : AllUnits)
				{
					if (ADominionUnitActor* Unit = Cast<ADominionUnitActor>(Act))
					{
						if (Unit->TeamID == 0) // Friendly
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
					// Select all nearby soldiers in the cohort
					for (AActor* Act : AllUnits)
					{
						if (ADominionUnitActor* Unit = Cast<ADominionUnitActor>(Act))
						{
							if (Unit->TeamID == 0 && FVector::Dist2D(Unit->GetActorLocation(), BestUnit->GetActorLocation()) < 1400.0f)
							{
								Unit->SetSelected(true);
								SelectedUnits.Add(Unit);
							}
						}
					}
				}
			}
		}
	}

	if (SelectedUnits.Num() > 0)
	{
		if (UDominionTutorialSubsystem* Tut = GetWorld()->GetSubsystem<UDominionTutorialSubsystem>())
		{
			Tut->NotifyArmySelected(SelectedUnits.Num());
		}
	}
}

void ADominionRTSPlayerController::OnRightClickPressed()
{
	if (SelectedUnits.Num() == 0) return;

	// Check if targeting an enemy unit
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Pawn, false, Hit);
	if (Hit.bBlockingHit && Hit.GetActor())
	{
		if (ADominionUnitActor* TargetUnit = Cast<ADominionUnitActor>(Hit.GetActor()))
		{
			if (TargetUnit->TeamID != 0) // Enemy Raider
			{
				OrderAttackTarget(TargetUnit);
				return;
			}
		}
	}

	// Trace visibility
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	if (Hit.bBlockingHit)
	{
		OrderMoveSelectedUnits(Hit.Location);
		return;
	}

	// Robust Mathematical Deprojection Raycast Fallback (Never Fails!)
	FVector WorldOrigin, WorldDir;
	if (DeprojectMousePositionToWorld(WorldOrigin, WorldDir))
	{
		if (FMath::Abs(WorldDir.Z) > 0.0001f)
		{
			float T = (100.0f - WorldOrigin.Z) / WorldDir.Z;
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

	// If units are already moving or standing, reform immediately to new formation geometry
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
	if (SelectedBuilding && IsValid(SelectedBuilding))
	{
		FVector SpawnLoc = SelectedBuilding->GetActorLocation() + FVector(400.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ADominionUnitActor* NewUnit = GetWorld()->SpawnActor<ADominionUnitActor>(ADominionUnitActor::StaticClass(), SpawnLoc, FRotator::ZeroRotator, SpawnParams);
		if (NewUnit)
		{
			NewUnit->TeamID = 0;
			if (UnitTypeIndex == 1)
			{
				NewUnit->UnitName = TEXT("Heavy War Chariot");
				NewUnit->UnitType = EDominionUnitType::HeavyChariot;
				NewUnit->MoveSpeed = 580.0f;
				NewUnit->AttackPower = 75.0f;
			}
			else if (UnitTypeIndex == 2)
			{
				NewUnit->UnitName = TEXT("Imperial Baggage Train");
				NewUnit->UnitType = EDominionUnitType::OxCartSupply;
				NewUnit->MoveSpeed = 260.0f;
			}

			if (UDominionTutorialSubsystem* Tut = GetWorld()->GetSubsystem<UDominionTutorialSubsystem>())
			{
				Tut->NotifyUnitRecruited(NewUnit->UnitName);
			}
		}
	}
}

void ADominionRTSPlayerController::AdvanceToNextEpoch()
{
	if (SelectedBuilding && IsValid(SelectedBuilding))
	{
		// Advance building architectural era
		uint8 CurrentStyle = (uint8)SelectedBuilding->ArchitecturalStyle;
		uint8 NextStyle = (CurrentStyle + 1) % 4;
		SelectedBuilding->SetArchitecturalStyle((EDominionArchitecturalStyle)NextStyle);
	}
}

// Tactical Hotkey Handlers
void ADominionRTSPlayerController::OnHotkey_1() { SetSelectedUnitsFormation(0); } // [1] Phalanx Shield Wall
void ADominionRTSPlayerController::OnHotkey_2() { SetSelectedUnitsFormation(1); } // [2] Wedge Shock Charge
void ADominionRTSPlayerController::OnHotkey_3() { SetSelectedUnitsFormation(2); } // [3] Skirmish Dispersion
void ADominionRTSPlayerController::OnHotkey_4() { SetSelectedUnitsFormation(3); } // [4] Square Defense
void ADominionRTSPlayerController::OnHotkey_Z() { TrainUnit(0); }                 // [Z] Train Spearman
void ADominionRTSPlayerController::OnHotkey_X() { AdvanceToNextEpoch(); }         // [X] Morph Architecture Era
void ADominionRTSPlayerController::OnHotkey_C() { TrainUnit(1); }                 // [C] Train Chariot
void ADominionRTSPlayerController::OnHotkey_V() { TrainUnit(2); }                 // [V] Train Baggage Train
void ADominionRTSPlayerController::OnHotkey_T()
{
	// Toggle Supply Line Interdiction for live test of Starvation Attrition
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

void ADominionRTSPlayerController::OnHotkey_E()
{
	// Cycle/Enact Next Edict
	static int32 EdictCycle = 0;
	if (UDominionPoliticalEstatesSystem* Estates = GetWorld()->GetSubsystem<UDominionPoliticalEstatesSystem>())
	{
		Estates->EnactEdict((EDominionEdictType)(EdictCycle % 4));
		EdictCycle++;
	}
}
