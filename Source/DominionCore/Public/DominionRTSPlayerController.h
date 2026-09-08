#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DominionTypes.h"
#include "DominionRTSPlayerController.generated.h"

class ADominionUnitActor;
class ADominionBuildingActor;
class ADominionRTSHUD;
class ADominionRTSPawn;

/**
 * ADominionRTSPlayerController
 * Handles box-selection marquee, unit right-click movement/attack commands,
 * Hotkey execution (Q/W/E/R formation switching, A/S/D unit training),
 * and live synchronization with the AoE2 HUD.
 */
UCLASS()
class DOMINIONCORE_API ADominionRTSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADominionRTSPlayerController();

	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	// --- Selection Queries ---
	UFUNCTION(BlueprintPure, Category = "Dominion|Selection")
	const TArray<ADominionUnitActor*>& GetSelectedUnits() const { return SelectedUnits; }

	UFUNCTION(BlueprintPure, Category = "Dominion|Selection")
	ADominionBuildingActor* GetSelectedBuilding() const { return SelectedBuilding; }

	// --- Tactical Commands ---
	UFUNCTION(BlueprintCallable, Category = "Dominion|Tactics")
	void OrderMoveSelectedUnits(const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Dominion|Tactics")
	void OrderAttackTarget(ADominionUnitActor* TargetUnit);

	UFUNCTION(BlueprintCallable, Category = "Dominion|Tactics")
	void SetSelectedUnitsFormation(int32 FormationMode); // 0 = Phalanx, 1 = Wedge, 2 = Skirmish, 3 = Line

	UFUNCTION(BlueprintCallable, Category = "Dominion|Economy")
	void TrainUnit(int32 UnitTypeIndex);

	UFUNCTION(BlueprintCallable, Category = "Dominion|Epoch")
	void AdvanceToNextEpoch();

protected:
	virtual void BeginPlay() override;

	// Mouse Input Events
	void OnLeftClickPressed();
	void OnLeftClickReleased();
	void OnRightClickPressed();
	void OnZoomIn();
	void OnZoomOut();
	void OnMiddleMousePressed();
	void OnMiddleMouseReleased();

	// Directional Input Events (WASD & Arrows)
	void OnMoveForwardPressed();
	void OnMoveForwardReleased();
	void OnMoveBackwardPressed();
	void OnMoveBackwardReleased();
	void OnMoveLeftPressed();
	void OnMoveLeftReleased();
	void OnMoveRightPressed();
	void OnMoveRightReleased();

	// Hotkey Actions (Formations & Tactics)
	void OnHotkey_1();
	void OnHotkey_2();
	void OnHotkey_3();
	void OnHotkey_4();
	void OnHotkey_Z();
	void OnHotkey_X();
	void OnHotkey_C();
	void OnHotkey_V();
	void OnHotkey_T();

	// Imperial Edict Hotkeys (3-Estate Lawbook)
	void OnHotkey_F1();
	void OnHotkey_F2();
	void OnHotkey_F3();
	void OnHotkey_F4();
	void OnHotkey_E();

	ADominionRTSPawn* EnsureRTSPawn();

private:
	UPROPERTY()
	TArray<TObjectPtr<ADominionUnitActor>> SelectedUnits;

	UPROPERTY()
	TObjectPtr<ADominionBuildingActor> SelectedBuilding;

	FVector2D MarqueeStartPos;
	bool bIsMarqueeSelecting = false;
	bool bIsMiddleMouseDragging = false;
	bool bMoveForward = false;
	bool bMoveBackward = false;
	bool bMoveLeft = false;
	bool bMoveRight = false;
	int32 CurrentFormationMode = 0;
	FVector2D LastMousePosition;
};
