#include "DominionPoliticalEstatesSystem.h"

void UDominionPoliticalEstatesSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	PowerBalance = EDominionPowerBalance::BalancedConcordat;
	CulturalPolicy = EDominionCulturalPolicy::ImperialPluralism;
	ReligiousAuthority = 60.0f;
	SecularStatePower = 60.0f;
	ClassUnrest = 20.0f;
	EthnicSeparatism = 25.0f;
}

void UDominionPoliticalEstatesSystem::ProcessPoliticsTick(float DeltaTime, float CurrentFoodSecurity, float CurrentTaxExploitation)
{
	// 1. Class Unrest calculation
	if (CurrentFoodSecurity < 0.5f)
	{
		ClassUnrest = FMath::Min(100.0f, ClassUnrest + 15.0f * DeltaTime);
	}
	else
	{
		ClassUnrest = FMath::Max(5.0f, ClassUnrest - 5.0f * DeltaTime);
	}

	if (CurrentTaxExploitation > 25.0f)
	{
		ClassUnrest = FMath::Min(100.0f, ClassUnrest + 8.0f * DeltaTime);
	}

	// 2. Ethnic Separatism calculation based on policy
	if (CulturalPolicy == EDominionCulturalPolicy::ForcedAssimilation)
	{
		EthnicSeparatism = FMath::Min(100.0f, EthnicSeparatism + 10.0f * DeltaTime);
	}
	else
	{
		EthnicSeparatism = FMath::Max(10.0f, EthnicSeparatism - 4.0f * DeltaTime);
	}

	// 3. Religious vs Political Friction
	if (PowerBalance == EDominionPowerBalance::SecularAutocracy)
	{
		ReligiousAuthority = FMath::Max(10.0f, ReligiousAuthority - 6.0f * DeltaTime);
		SecularStatePower = FMath::Min(100.0f, SecularStatePower + 8.0f * DeltaTime);
		if (ReligiousAuthority < 25.0f && FMath::FRand() < 0.05f)
		{
			OnCrisisTriggered.Broadcast(TEXT("HOLY ZEALOT INSURRECTION"), TEXT("The High Priesthood has declared the Crown illegitimate! Zealots are storming the palace!"));
		}
	}
	else if (PowerBalance == EDominionPowerBalance::TheocraticSubjugation)
	{
		ReligiousAuthority = FMath::Min(100.0f, ReligiousAuthority + 8.0f * DeltaTime);
		SecularStatePower = FMath::Max(10.0f, SecularStatePower - 6.0f * DeltaTime);
		if (SecularStatePower < 25.0f && FMath::FRand() < 0.05f)
		{
			OnCrisisTriggered.Broadcast(TEXT("MILITARY LEVY MUTINY"), TEXT("Army generals refuse to surrender state authority to the religious inquisition!"));
		}
	}

	// Check Class Riots
	if (ClassUnrest > 80.0f && FMath::FRand() < 0.08f)
	{
		OnCrisisTriggered.Broadcast(TEXT("GENERAL SERF & WORKER STRIKE"), TEXT("Laborers have abandoned the fields and blast furnaces! Economic output is paralyzed!"));
	}

	// Check Ethnic Revolts
	if (EthnicSeparatism > 75.0f && FMath::FRand() < 0.08f)
	{
		OnCrisisTriggered.Broadcast(TEXT("PROVINCIAL SEPARATIST UPRISING"), TEXT("Conquered borderland tribes have raised the banner of independence!"));
	}
}

void UDominionPoliticalEstatesSystem::SetPowerBalance(EDominionPowerBalance NewBalance)
{
	PowerBalance = NewBalance;
}

void UDominionPoliticalEstatesSystem::SetCulturalPolicy(EDominionCulturalPolicy NewPolicy)
{
	CulturalPolicy = NewPolicy;
}

void UDominionPoliticalEstatesSystem::ApplySavageryBlowback(const FDominionSavageryMetrics& Metrics)
{
	// 1. Moral Decay causes Religious Anathema
	if (Metrics.MoralDecay > 30.0f)
	{
		ReligiousAuthority = FMath::Max(5.0f, ReligiousAuthority - (Metrics.MoralDecay * 0.4f));
		ClassUnrest = FMath::Min(100.0f, ClassUnrest + 25.0f);
		OnCrisisTriggered.Broadcast(
			TEXT("HOLY ANATHEMA & EXCOMMUNICATION"),
			TEXT("The Priesthood has cursed the Crown for slaughtering whole nations! Blood pollution defiles the kingdom; fanatic flagellant cults roam the cities!")
		);
	}

	// 2. Brutalized Legion Mutiny
	if (Metrics.LegionDisloyalty > 40.0f)
	{
		SecularStatePower = FMath::Max(10.0f, SecularStatePower - 30.0f);
		OnCrisisTriggered.Broadcast(
			TEXT("LEGION DONATIVUM COUP ATTEMPT"),
			TEXT("Veterans hardened by total massacre demand immediate gold extortion or will burn the imperial capital!")
		);
	}

	// 3. World Coalition Existential Threat
	if (Metrics.WorldInfamy >= 90.0f)
	{
		EthnicSeparatism = FMath::Min(100.0f, EthnicSeparatism + 35.0f);
		OnCrisisTriggered.Broadcast(
			TEXT("GRAND COALITION OF SURVIVAL"),
			TEXT("Every surviving nation on the continent has formed a holy military pact to exterminate our empire before we extinguish them!")
		);
	}
}

