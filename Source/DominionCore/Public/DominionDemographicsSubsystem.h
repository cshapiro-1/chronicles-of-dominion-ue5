#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DominionTypes.h"
#include "DominionDemographicsSubsystem.generated.h"

UENUM(BlueprintType)
enum class EDominionMigrationPolicy : uint8
{
	StrictSerfdom UMETA(DisplayName = "Strict Feudal Serfdom (Passports Enforced)"),
	CityAirMakesYouFree UMETA(DisplayName = "City Air Makes You Free (1 Year & 1 Day)"),
	ForcedEnclosure UMETA(DisplayName = "Forced Land Enclosure & Urban Eviction")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDemographicCrisis, FString, CrisisTitle, FString, CrisisDescription);

/**
 * Manages the Demographic Age Pyramid, Infant Mortality, Rural-Urban Divide, and Healthcare.
 */
UCLASS()
class DOMINIONCORE_API UDominionDemographicsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Executes one tick of demographic cohort aging, births, and infant mortality */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Demographics")
	void ProcessDemographicsTick(float DeltaTime, float AvailableGrain, float WaterSanitation, float FirewoodStock, float DietaryDiversity, float MidwiferyTier);

	/** Sets Imperial Migration Policy */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Demographics")
	void SetMigrationPolicy(EDominionMigrationPolicy NewPolicy);

	/** Gets Age Pyramid struct */
	UFUNCTION(BlueprintPure, Category = "Dominion|Demographics")
	const FDominionAgePyramid& GetAgePyramid() const { return AgePyramid; }

	/** Drafts rural working-age adult population into active military cohorts */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Demographics")
	bool DraftLevy(int32 ManpowerCount);

	/** Gets current rural agricultural serf population */
	UFUNCTION(BlueprintPure, Category = "Dominion|Demographics")
	int64 GetRuralPopulation() const { return RuralPopulation; }

	/** Gets current urban artisan / citizen population */
	UFUNCTION(BlueprintPure, Category = "Dominion|Demographics")
	int64 GetUrbanPopulation() const { return UrbanPopulation; }

	/** Gets count of active drafted military cohorts */
	UFUNCTION(BlueprintPure, Category = "Dominion|Demographics")
	int32 GetActiveDraftedCohorts() const { return ActiveDraftedCohorts; }

	/** Gets dynamic grain harvest rate factoring in active drafted serfs */
	UFUNCTION(BlueprintPure, Category = "Dominion|Demographics")
	float GetGrainHarvestRate() const { return FMath::Max(10.0f, 45.0f - (ActiveDraftedCohorts * 15.0f)); }

	UPROPERTY(BlueprintAssignable, Category = "Dominion|Demographics")
	FOnDemographicCrisis OnDemographicCrisis;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Demographics")
	int32 ActiveDraftedCohorts = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Demographics")
	FDominionAgePyramid AgePyramid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Demographics")
	EDominionMigrationPolicy MigrationPolicy = EDominionMigrationPolicy::CityAirMakesYouFree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Demographics")
	float InfantMortalityRate = 0.38f; // Baseline 38% pre-modern infant mortality

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Demographics")
	float AnnualBirthRate = 0.075f; // Pre-modern high fertility (7.5% annual births)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Demographics")
	int64 RuralPopulation = 12000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Demographics")
	int64 UrbanPopulation = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Demographics")
	int64 SlumPopulation = 1200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Demographics")
	float UrbanGrainDaysBuffer = 30.0f;
};
