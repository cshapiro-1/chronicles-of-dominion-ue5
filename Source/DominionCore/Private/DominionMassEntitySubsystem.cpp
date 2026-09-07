#include "DominionMassEntitySubsystem.h"

void UDominionMassEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	PopCohorts.Empty();

	// Starting Bronze Age River Valley City-State (15,000 citizens)
	AddPopCohort(EDominionSocialClass::TribalElder, 750);       // High Priesthood & Chariot Nobles (5%)
	AddPopCohort(EDominionSocialClass::PlebeianCitizen, 2250);   // Cuneiform Scribes & Bronze Smiths (15%)
	AddPopCohort(EDominionSocialClass::PeasantSerfs, 9000);      // Irrigated Canal Farmers (60%)
	AddPopCohort(EDominionSocialClass::SlavesAndCaptives, 3000); // Quarry & Monument Laborers (20%)
}

void UDominionMassEntitySubsystem::ProcessDemographicsTick(float DeltaTime)
{
	for (FDominionPopCohort& Cohort : PopCohorts)
	{
		const int32 NetBirths = FMath::FloorToInt(Cohort.HeadCount * (BirthRate - MortalityRate) * (DeltaTime / 12.0f));
		Cohort.HeadCount = FMath::Max(1, Cohort.HeadCount + NetBirths);
	}
}

void UDominionMassEntitySubsystem::AddPopCohort(EDominionSocialClass SocialClass, int32 HeadCount)
{
	FDominionPopCohort NewCohort;
	NewCohort.SocialClass = SocialClass;
	NewCohort.HeadCount = HeadCount;
	NewCohort.Happiness = 82.0f;
	PopCohorts.Add(NewCohort);
}

int64 UDominionMassEntitySubsystem::GetTotalEmpirePopulation() const
{
	int64 Total = 0;
	for (const FDominionPopCohort& Cohort : PopCohorts)
	{
		Total += Cohort.HeadCount;
	}
	return Total;
}
