#include "DominionProvinceManager.h"

ADominionProvinceManager::ADominionProvinceManager()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentSoilFertility = 1.0f;
    CanalCoveragePercent = 88.0f;
}

void ADominionProvinceManager::BeginPlay()
{
    Super::BeginPlay();
}

void ADominionProvinceManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // Smoothly decay soil nitrogen over dry summer months unless fertilized
}

void ADominionProvinceManager::TriggerSpringFloodEvent()
{
    CurrentSoilFertility = 1.25f; // +25% crop yield bonus during spring inundation
}

bool ADominionProvinceManager::IsGridCellBuildable(FVector WorldLocation, FString& OutFailureReason) const
{
    if (WorldLocation.Z < RiverWaterLevel)
    {
        OutFailureReason = TEXT("Underwater: Requires Dredging or Sluice Canal Gate");
        return false;
    }
    return true;
}
