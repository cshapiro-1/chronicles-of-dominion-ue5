#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DominionRTSHUD.generated.h"

class SDominionCommandGridWidget;

USTRUCT(BlueprintType)
struct DOMINIONCORE_API FDominionCommandAction
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    FString ActionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    FText DisplayLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    FString HotkeyCharacter; // e.g. "Q", "W", "E", "R"

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    FText CostDescription; // e.g. "50 Grain, 20 Bronze"

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    TSoftObjectPtr<UTexture2D> IconTexture;
};

/**
 * ADominionRTSHUD
 * Unreal Engine 5 HUD managing the Triple-A AoE2 bottom console:
 * - Left: Real-time Minimap radar with team color blips
 * - Center: Selection Portrait, integrity bar, demographic stats
 * - Right: Contextual 3x5 Slate Command Grid with hotkey triggers
 */
UCLASS()
class DOMINIONCORE_API ADominionRTSHUD : public AHUD
{
    GENERATED_BODY()

public:
    ADominionRTSHUD();

    virtual void DrawHUD() override;
    virtual void PostInitializeComponents() override;

    /** Updates the 15-button command grid based on currently selected unit/building */
    UFUNCTION(BlueprintCallable, Category = "Dominion|HUD")
    void SetCommandGridActions(const TArray<FDominionCommandAction>& Actions);

    /** Triggers action assigned to hotkey (e.g. Q, W, E, R) */
    UFUNCTION(BlueprintCallable, Category = "Dominion|HUD")
    bool ExecuteHotkeyAction(const FString& Hotkey);

    UFUNCTION(BlueprintCallable, Category = "Dominion|HUD")
    void ToggleTutorial() { bShowTutorialCard = !bShowTutorialCard; }

    UFUNCTION(BlueprintCallable, Category = "Dominion|HUD")
    void ToggleProductionLedger() { bShowProductionLedger = !bShowProductionLedger; }

    UFUNCTION(BlueprintCallable, Category = "Dominion|HUD")
    void SetActiveLedgerTab(int32 TabIndex) { ActiveLedgerTab = FMath::Clamp(TabIndex, 0, 3); }

    UFUNCTION(BlueprintCallable, Category = "Dominion|HUD")
    void NextLedgerTab() { ActiveLedgerTab = (ActiveLedgerTab + 1) % 4; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|HUD")
    bool bShowTutorialCard = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|HUD")
    bool bShowProductionLedger = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|HUD")
    int32 ActiveLedgerTab = 1; // 0 = Production Flow, 1 = Necessities & Living, 2 = Demographics, 3 = Trade & Logistics

protected:
    UPROPERTY(EditDefaultsOnly, Category = "HUD Colors")
    FLinearColor FriendlyColor = FLinearColor(0.14f, 0.54f, 0.95f, 1.0f);

    UPROPERTY(EditDefaultsOnly, Category = "HUD Colors")
    FLinearColor HostileColor = FLinearColor(0.93f, 0.15f, 0.15f, 1.0f);

    UPROPERTY(EditDefaultsOnly, Category = "HUD Colors")
    FLinearColor SelectionRingColor = FLinearColor(0.98f, 0.75f, 0.14f, 1.0f);

private:
    void DrawSelectionMarquee();
    void DrawMinimapOverlay();

    TSharedPtr<SDominionCommandGridWidget> CommandGridWidget;
    TArray<FDominionCommandAction> CurrentActions;
};


