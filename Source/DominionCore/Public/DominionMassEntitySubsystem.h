#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DominionTypes.h"
#include "DominionMassEntitySubsystem.generated.h"

/**
 * High-performance MassEntity ECS Subsystem capable of simulating millions of citizens.
 * Processes demographic cohorts, employment allocations, food rationing, disease propagation, and mortality.
 */
UCLASS()
class DOMINIONCORE_API UDominionMassEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Executes one simulation tick across all demographic cohorts */
	UFUNCTION(BlueprintCallable, Category = "Dominion|MassEntity")
	void ProcessDemographicsTick(float DeltaTime);

	/** Adds population to a specific social class */
	UFUNCTION(BlueprintCallable, Category = "Dominion|MassEntity")
	void AddPopCohort(EDominionSocialClass SocialClass, int32 HeadCount);

	/** Total empire-wide population count */
	UFUNCTION(BlueprintPure, Category = "Dominion|MassEntity")
	int64 GetTotalEmpirePopulation() const;

	/** Gets current empire-wide mortality rate */
	UFUNCTION(BlueprintPure, Category = "Dominion|MassEntity")
	float GetMortalityRate() const { return MortalityRate; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|MassEntity")
	TArray<FDominionPopCohort> PopCohorts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|MassEntity")
	float MortalityRate = 0.02f; // Baseline 2% annual mortality

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|MassEntity")
	float BirthRate = 0.035f;   // Baseline 3.5% birth rate under good nutrition
};
