// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DominionSupplySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCohortStarvationTick, FName, CohortID, float, DamagePercent);

USTRUCT(BlueprintType)
struct DOMINIONCORE_API FCohortSupplyState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Supply")
    FName CohortID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Supply")
    bool bIsTethered = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Supply")
    float RationsRemainingSeconds = 180.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Supply")
    float MaxRationsSeconds = 180.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Supply")
    float StarvationDamagePercentPerTick = 0.05f;

    FCohortSupplyState()
        : CohortID(NAME_None)
        , bIsTethered(true)
        , RationsRemainingSeconds(180.0f)
        , MaxRationsSeconds(180.0f)
        , StarvationDamagePercentPerTick(0.05f)
    {}
};

/**
 * UDominionSupplySubsystem
 * Logistical supply line interdiction and ration starvation subsystem.
 * Evaluates cohorts on a coarse 1.0s interval.
 */
UCLASS(BlueprintType)
class DOMINIONCORE_API UDominionSupplySubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    UDominionSupplySubsystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UDominionSupplySubsystem, STATGROUP_Tickables); }

    /** Register a cohort into the supply line tracking network */
    UFUNCTION(BlueprintCallable, Category = "Dominion|Supply")
    void RegisterCohort(FName CohortID, float InitialRations = 180.0f, float MaxRations = 180.0f);

    /** Unregister a cohort when disbanded or destroyed */
    UFUNCTION(BlueprintCallable, Category = "Dominion|Supply")
    void UnregisterCohort(FName CohortID);

    /** Set tether status for a cohort (e.g. within range of ox-cart or depot) */
    UFUNCTION(BlueprintCallable, Category = "Dominion|Supply")
    void SetCohortTethered(FName CohortID, bool bTethered);

    /** Query cohort supply status */
    UFUNCTION(BlueprintPure, Category = "Dominion|Supply")
    bool GetCohortSupplyState(FName CohortID, FCohortSupplyState& OutState) const;

    /** Delegate fired whenever an untethered cohort with 0 rations suffers starvation attrition */
    UPROPERTY(BlueprintAssignable, Category = "Dominion|Supply")
    FOnCohortStarvationTick OnCohortStarvationTick;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dominion|Supply")
    TMap<FName, FCohortSupplyState> TrackedCohorts;

private:
    float CoarseTickAccumulator = 0.0f;
    static constexpr float CoarseInterval = 1.0f;
};
