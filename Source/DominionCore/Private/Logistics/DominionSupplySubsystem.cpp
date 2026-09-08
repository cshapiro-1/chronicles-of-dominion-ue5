// Copyright Epic Games, Inc. All Rights Reserved.

#include "DominionSupplySubsystem.h"
#include "Engine/World.h"

UDominionSupplySubsystem::UDominionSupplySubsystem()
{
}

void UDominionSupplySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    TrackedCohorts.Empty();
    CoarseTickAccumulator = 0.0f;
}

void UDominionSupplySubsystem::Deinitialize()
{
    TrackedCohorts.Empty();
    Super::Deinitialize();
}

void UDominionSupplySubsystem::RegisterCohort(FName CohortID, float InitialRations, float MaxRations)
{
    FCohortSupplyState NewState;
    NewState.CohortID = CohortID;
    NewState.bIsTethered = true;
    NewState.RationsRemainingSeconds = InitialRations;
    NewState.MaxRationsSeconds = MaxRations;
    TrackedCohorts.Add(CohortID, NewState);
}

void UDominionSupplySubsystem::UnregisterCohort(FName CohortID)
{
    TrackedCohorts.Remove(CohortID);
}

void UDominionSupplySubsystem::SetCohortTethered(FName CohortID, bool bTethered)
{
    if (FCohortSupplyState* State = TrackedCohorts.Find(CohortID))
    {
        State->bIsTethered = bTethered;
    }
}

bool UDominionSupplySubsystem::GetCohortSupplyState(FName CohortID, FCohortSupplyState& OutState) const
{
    if (const FCohortSupplyState* State = TrackedCohorts.Find(CohortID))
    {
        OutState = *State;
        return true;
    }
    return false;
}

void UDominionSupplySubsystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CoarseTickAccumulator += DeltaTime;
    if (CoarseTickAccumulator < CoarseInterval)
    {
        return;
    }

    const float ElapsedCoarseTime = CoarseTickAccumulator;
    CoarseTickAccumulator = 0.0f;

    for (auto& Pair : TrackedCohorts)
    {
        FCohortSupplyState& Cohort = Pair.Value;

        if (Cohort.bIsTethered)
        {
            // Replenish operational field rations when safely connected to supply lines
            Cohort.RationsRemainingSeconds = FMath::Min(Cohort.MaxRationsSeconds, Cohort.RationsRemainingSeconds + (30.0f * ElapsedCoarseTime));
        }
        else
        {
            // Supply tether severed: drain haversack operational rations
            Cohort.RationsRemainingSeconds -= ElapsedCoarseTime;

            if (Cohort.RationsRemainingSeconds <= 0.0f)
            {
                Cohort.RationsRemainingSeconds = 0.0f;
                // Broadcast coarse starvation attrition tick
                if (OnCohortStarvationTick.IsBound())
                {
                    OnCohortStarvationTick.Broadcast(Cohort.CohortID, Cohort.StarvationDamagePercentPerTick);
                }
            }
        }
    }
}
