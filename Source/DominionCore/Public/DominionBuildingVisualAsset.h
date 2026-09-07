#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DominionTypes.h"
#include "DominionBuildingVisualAsset.generated.h"

class UStaticMesh;
class UMaterialInterface;
class UNiagaraSystem;
class USoundCue;

UENUM(BlueprintType)
enum class EDominionVisualCultureStyle : uint8
{
    AlluvialSunDrenched     UMETA(DisplayName = "Sun-Drenched Alluvial (Bronze)"),
    MythicGrimdarkSmog      UMETA(DisplayName = "Mythic Grimdark Smog"),
    ClassicalMarbleImperial UMETA(DisplayName = "Classical Imperial Marble"),
    NordicTimberMist        UMETA(DisplayName = "Nordic Timber & Mist"),
    IndustrialIronFoundry   UMETA(DisplayName = "Industrial Iron Foundry")
};

/**
 * UDominionBuildingVisualAsset
 * Pure Data Asset: Holds 3D Meshes, Materials, Particle Sockets, and Sounds.
 * Enables zero-code modular swapping of building designs in Unreal Engine 5.
 */
UCLASS(BlueprintType)
class DOMINIONCORE_API UDominionBuildingVisualAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Identity")
    FString BuildingID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Identity")
    EDominionVisualCultureStyle CultureStyle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Identity")
    EDominionEpoch TargetEpoch;

    // --- Modular Geometry Sockets (Kitbashing) ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modular Sockets")
    TSoftObjectPtr<UStaticMesh> BaseFoundationMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modular Sockets")
    TSoftObjectPtr<UStaticMesh> MainStructureMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modular Sockets")
    TSoftObjectPtr<UStaticMesh> RoofMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modular Sockets")
    TArray<TSoftObjectPtr<UStaticMesh>> AccessoryProps;

    // --- Materials & Textures ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shaders")
    TSoftObjectPtr<UMaterialInterface> PrimaryMaterialSkin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shaders")
    TSoftObjectPtr<UMaterialInterface> WeatheredOverlayMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shaders")
    TSoftObjectPtr<UMaterialInterface> BurnedRuinMaterial;

    // --- Dynamic VFX & Lighting ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    TSoftObjectPtr<UNiagaraSystem> ChimneySmogFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    TSoftObjectPtr<UNiagaraSystem> TorchBrazierFireFX;

    // --- Audio Soundscape ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TSoftObjectPtr<USoundCue> AmbientWorkLoopSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TSoftObjectPtr<USoundCue> DestructionCollapseSound;
};


