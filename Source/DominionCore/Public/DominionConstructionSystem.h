#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DominionTypes.h"
#include "DominionConstructionSystem.generated.h"

UENUM(BlueprintType)
enum class EDominionZoneType : uint8
{
	Unzoned,
	ResidentialLot UMETA(DisplayName = "Residential (Mudbrick / Townhouses)"),
	CommercialSouk UMETA(DisplayName = "Commercial (Bazaars & Storefronts)"),
	IndustrialWard UMETA(DisplayName = "Industrial (Smelters, Tanneries, Kilns)"),
	AgrarianAllotment UMETA(DisplayName = "Agrarian (Strip Fields & Orchards)")
};

UENUM(BlueprintType)
enum class EDominionConstructionPhase : uint8
{
	SurveyingEarthworks UMETA(DisplayName = "Phase 1: Surveying & Foundation Earthworks"),
	MaterialHauling UMETA(DisplayName = "Phase 2: Physical Material Hauling (Ox Carts)"),
	ScaffoldingAndFraming UMETA(DisplayName = "Phase 3: Scaffolding & Structural Framing"),
	NaniteFinishing UMETA(DisplayName = "Phase 4: Masonry & Roofing Finishing"),
	CompletedOccupied UMETA(DisplayName = "Completed & Occupied")
};

USTRUCT(BlueprintType)
struct DOMINIONCORE_API FDominionConstructionSite
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	int32 SiteId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	EDominionZoneType ZoneType = EDominionZoneType::ResidentialLot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	EDominionConstructionPhase CurrentPhase = EDominionConstructionPhase::SurveyingEarthworks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	float Progress = 0.0f; // 0-100%

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	int32 RequiredTimber = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	int32 DeliveredTimber = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	int32 RequiredStone = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	int32 DeliveredStone = 0;
};

/**
 * Manages organic spline-based parcel zoning, step-by-step physical material hauling,
 * Nanite scaffolding construction, and Cities:Skylines-style district policies and pollution vectors.
 */
UCLASS()
class DOMINIONCORE_API UDominionConstructionSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Advances all active construction sites */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Construction")
	void ProcessConstructionTick(float DeltaTime);

	/** Creates a new construction site from zoning or direct placement */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Construction")
	int32 CreateConstructionSite(FVector Location, EDominionZoneType ZoneType, int32 TimberCost, int32 StoneCost);

	/** Delivers materials via logistics ox carts */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Construction")
	void DeliverMaterials(int32 SiteId, int32 TimberAmount, int32 StoneAmount);

	/** Gets active construction sites */
	UFUNCTION(BlueprintPure, Category = "Dominion|Construction")
	const TArray<FDominionConstructionSite>& GetActiveSites() const { return ActiveSites; }

	/** Wind vector for industrial pollution dispersion */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|CityPlanning")
	FVector2D WindDirection = FVector2D(1.0f, 0.2f); // Normalized wind vector

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Construction")
	TArray<FDominionConstructionSite> ActiveSites;

	int32 NextSiteId = 1;
};
