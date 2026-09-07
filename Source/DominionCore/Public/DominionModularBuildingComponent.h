#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DominionBuildingVisualAsset.h"
#include "DominionModularBuildingComponent.generated.h"

class UStaticMeshComponent;
class UNiagaraComponent;

/**
 * UDominionModularBuildingComponent
 * Runtime actor component that manages modular mesh sockets, material swapping,
 * and damage/decay states without touching gameplay simulation logic.
 */
UCLASS(ClassGroup=(Dominion), meta=(BlueprintSpawnableComponent))
class DOMINIONCORE_API UDominionModularBuildingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDominionModularBuildingComponent();

    /** Swaps the visual skin/theme of this building in real time */
    UFUNCTION(BlueprintCallable, Category = "Dominion|ModularGraphics")
    void ApplyVisualTheme(UDominionBuildingVisualAsset* NewVisualAsset);

    /** Swaps individual component (e.g. Roof or Walls) */
    UFUNCTION(BlueprintCallable, Category = "Dominion|ModularGraphics")
    void SwapRoofMesh(UStaticMesh* NewRoofMesh);

    /** Applies procedural burn/ash shaders during conquest or siege */
    UFUNCTION(BlueprintCallable, Category = "Dominion|ModularGraphics")
    void ApplyDamageBurnOverlay(float DamagePercent);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> FoundationMeshComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> MainStructureMeshComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> RoofMeshComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UNiagaraComponent> SmogVFXComponent;

    UPROPERTY()
    TObjectPtr<UDominionBuildingVisualAsset> CurrentVisualAsset;
};


