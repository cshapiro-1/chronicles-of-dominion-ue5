#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DominionTypes.h"
#include "DominionProvinceManager.generated.h"

class UMaterialParameterCollection;

/**
 * ADominionProvinceManager
 * Coordinates the 3D alluvial river basin, seasonal spring inundations,
 * silt soil fertilization, dynamic drought, and grid construction zoning in Unreal Engine 5.
 */
UCLASS()
class DOMINIONCORE_API ADominionProvinceManager : public AActor
{
    GENERATED_BODY()

public:
    ADominionProvinceManager();

    virtual void Tick(float DeltaTime) override;

    /** Triggers annual spring silt flood (+100% soil nitrogen fertility) */
    UFUNCTION(BlueprintCallable, Category = "Dominion|Hydrology")
    void TriggerSpringFloodEvent();

    /** Checks if a coordinate on the terrain is buildable and within canal reach */
    UFUNCTION(BlueprintCallable, Category = "Dominion|Zoning")
    bool IsGridCellBuildable(FVector WorldLocation, FString& OutFailureReason) const;

    UFUNCTION(BlueprintPure, Category = "Dominion|Hydrology")
    float GetCurrentSoilFertility() const { return CurrentSoilFertility; }

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hydrology", meta = (AllowPrivateAccess = "true"))
    float RiverWaterLevel = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hydrology", meta = (AllowPrivateAccess = "true"))
    float CurrentSoilFertility = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hydrology", meta = (AllowPrivateAccess = "true"))
    float CanalCoveragePercent = 88.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Shaders")
    TObjectPtr<UMaterialParameterCollection> GlobalWeatherParameters;
};


