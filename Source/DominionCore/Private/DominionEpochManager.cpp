#include "DominionEpochManager.h"

void UDominionEpochManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Default starting epoch: Epoch II (Bronze Age, 3,000 BCE) per Director instruction
	CurrentEpoch = EDominionEpoch::BronzeAge;
	CurrentSeason = EDominionSeason::Spring;
	CurrentYear = -3000; // 3,000 BCE
	CurrentMonth = 1;
}

void UDominionEpochManager::TickSimulation(float DeltaSeconds)
{
	MonthTimer += DeltaSeconds;
	if (MonthTimer >= MonthDurationInSeconds)
	{
		MonthTimer = 0.0f;
		CurrentMonth++;
		if (CurrentMonth > 12)
		{
			CurrentMonth = 1;
			CurrentYear++;
		}

		const int32 SeasonIdx = (CurrentMonth - 1) / 3;
		const EDominionSeason NewSeason = static_cast<EDominionSeason>(SeasonIdx);
		if (NewSeason != CurrentSeason)
		{
			CurrentSeason = NewSeason;
			OnSeasonChanged.Broadcast(CurrentSeason);
		}
	}
}

bool UDominionEpochManager::TryAdvanceEpoch()
{
	const uint8 NextEpochIdx = static_cast<uint8>(CurrentEpoch) + 1;
	if (NextEpochIdx <= static_cast<uint8>(EDominionEpoch::IndustrialRevolution))
	{
		CurrentEpoch = static_cast<EDominionEpoch>(NextEpochIdx);
		OnEpochAdvanced.Broadcast(CurrentEpoch);
		return true;
	}
	return false;
}
