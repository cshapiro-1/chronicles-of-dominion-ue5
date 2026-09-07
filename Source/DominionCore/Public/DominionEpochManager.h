#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DominionTypes.h"
#include "DominionEpochManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEpochAdvanced, EDominionEpoch, NewEpoch);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSeasonChanged, EDominionSeason, NewSeason);

/**
 * Manages the advancement across the 5 Epochs (Stone Age -> Industrial Revolution),
 * planetary time progression, seasons, and technological era transitions.
 */
UCLASS()
class DOMINIONCORE_API UDominionEpochManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Advances simulation time by DeltaSeconds */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Time")
	void TickSimulation(float DeltaSeconds);

	/** Advances the realm to the next Epoch when technological prerequisites are met */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Epoch")
	bool TryAdvanceEpoch();

	/** Gets current active Epoch */
	UFUNCTION(BlueprintPure, Category = "Dominion|Epoch")
	EDominionEpoch GetCurrentEpoch() const { return CurrentEpoch; }

	/** Gets current season */
	UFUNCTION(BlueprintPure, Category = "Dominion|Season")
	EDominionSeason GetCurrentSeason() const { return CurrentSeason; }

	UPROPERTY(BlueprintAssignable, Category = "Dominion|Events")
	FOnEpochAdvanced OnEpochAdvanced;

	UPROPERTY(BlueprintAssignable, Category = "Dominion|Events")
	FOnSeasonChanged OnSeasonChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Epoch")
	EDominionEpoch CurrentEpoch = EDominionEpoch::StoneAge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Season")
	EDominionSeason CurrentSeason = EDominionSeason::Spring;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Time")
	int32 CurrentYear = -10000; // 10,000 BCE

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Time")
	int32 CurrentMonth = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Time")
	float MonthTimer = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Time")
	float MonthDurationInSeconds = 12.0f;
};
