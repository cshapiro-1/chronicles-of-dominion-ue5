#include "DominionMilitarySubsystem.h"

void UDominionMilitarySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ArmyStrength = 2500;
	FleetStrength = 15;
	EnslavedCaptiveCount = 3000;
	BaggageGrainRations = 100.0f;
	MonthlyGoldUpkeepPerSoldier = 0.05f;
}

void UDominionMilitarySubsystem::ProcessMilitaryTick(float DeltaTime, float AvailableTreasuryGold, float AvailableGranaryGrain)
{
	const float MonthlyGoldCost = ArmyStrength * MonthlyGoldUpkeepPerSoldier;
	const float MonthlyGrainCost = ArmyStrength * 0.8f;

	// Check Baggage Train Grain supply
	if (AvailableGranaryGrain < MonthlyGrainCost * 0.4f)
	{
		BaggageGrainRations = FMath::Max(0.0f, BaggageGrainRations - 15.0f * DeltaTime);
		if (BaggageGrainRations <= 10.0f && FMath::FRand() < 0.08f)
		{
			OnMilitaryCrisis.Broadcast(TEXT("MILITARY BAGGAGE ATTRITION"), TEXT("Grain supply lines have been severed! Soldiers are starving and abandoning their posts!"));
			ArmyStrength = FMath::Max(100, FMath::FloorToInt(ArmyStrength * 0.90f));
		}
	}
	else
	{
		BaggageGrainRations = FMath::Min(100.0f, BaggageGrainRations + 10.0f * DeltaTime);
	}

	// Check Treasury Gold for Mercenary / Legion upkeep
	if (AvailableTreasuryGold < MonthlyGoldCost * 0.2f && FMath::FRand() < 0.05f)
	{
		OnMilitaryCrisis.Broadcast(TEXT("MERCENARY TREASURY MUTINY"), TEXT("The Royal Treasury is empty! Unpaid mercenaries have mutinied and are pillaging the countryside!"));
	}
}

bool UDominionMilitarySubsystem::MobilizeRegiment(EDominionRecruitmentType RecruitmentType, int32 TroopCount)
{
	if (RecruitmentType == EDominionRecruitmentType::GalleySlaveFleet)
	{
		if (EnslavedCaptiveCount >= TroopCount * 50)
		{
			EnslavedCaptiveCount -= (TroopCount * 50);
			FleetStrength += TroopCount;
			return true;
		}
		return false;
	}

	ArmyStrength += TroopCount;
	return true;
}

void UDominionMilitarySubsystem::ExecuteConquestPolicy(FString TargetProvince, EDominionConquestPolicy Policy, int32 DefeatedPops)
{
	int32 LootedGold = 0;
	int32 EnslavedPops = 0;

	switch (Policy)
	{
	case EDominionConquestPolicy::PillageAndSack:
		LootedGold = 12000;
		EnslavedPops = 0;
		SavageryState.SavageryIndex = FMath::Min(100.0f, SavageryState.SavageryIndex + 8.0f);
		SavageryState.WorldInfamy = FMath::Min(100.0f, SavageryState.WorldInfamy + 10.0f);
		break;

	case EDominionConquestPolicy::EnslaveAndDeport:
		LootedGold = 4000;
		EnslavedPops = FMath::FloorToInt(DefeatedPops * 0.60f);
		EnslavedCaptiveCount += EnslavedPops;
		SavageryState.SavageryIndex = FMath::Min(100.0f, SavageryState.SavageryIndex + 15.0f);
		SavageryState.WorldInfamy = FMath::Min(100.0f, SavageryState.WorldInfamy + 18.0f);
		break;

	case EDominionConquestPolicy::TributarySubjugation:
		LootedGold = 2500;
		EnslavedPops = 0;
		SavageryState.WorldInfamy = FMath::Min(100.0f, SavageryState.WorldInfamy + 4.0f);
		break;

	case EDominionConquestPolicy::TotalImperialAnnexation:
		LootedGold = 1000;
		EnslavedPops = 0;
		SavageryState.WorldInfamy = FMath::Min(100.0f, SavageryState.WorldInfamy + 6.0f);
		break;

	case EDominionConquestPolicy::TotalAnnihilation:
		LootedGold = 35000;
		EnslavedPops = 0; // All slaughtered
		SavageryState.TotalExtinguishedEmpires++;
		SavageryState.SavageryIndex = FMath::Min(100.0f, SavageryState.SavageryIndex + 45.0f);
		SavageryState.WorldInfamy = 100.0f; // Maximum Infamy - Coalition Triggered
		SavageryState.MoralDecay = FMath::Min(100.0f, SavageryState.MoralDecay + 40.0f);
		SavageryState.CorpsePlagueRisk = FMath::Min(100.0f, SavageryState.CorpsePlagueRisk + 50.0f);
		SavageryState.LegionDisloyalty = FMath::Min(100.0f, SavageryState.LegionDisloyalty + 35.0f);

		OnSavageryBlowback.Broadcast(
			TEXT("CIVILIZATION ANNIHILATED"),
			FString::Printf(TEXT("The entire population of %s has been massacred and their cities burned to ash. The civilized world recoils in absolute terror!"), *TargetProvince)
		);
		break;

	case EDominionConquestPolicy::SaltTheEarthAndPoisonAquifers:
		LootedGold = 5000;
		EnslavedPops = 0;
		SavageryState.SavageryIndex = FMath::Min(100.0f, SavageryState.SavageryIndex + 30.0f);
		SavageryState.WorldInfamy = FMath::Min(100.0f, SavageryState.WorldInfamy + 30.0f);
		SavageryState.CorpsePlagueRisk = FMath::Min(100.0f, SavageryState.CorpsePlagueRisk + 30.0f);

		OnSavageryBlowback.Broadcast(
			TEXT("SCORCHED DESOLATION"),
			FString::Printf(TEXT("Fields of %s have been sowed with salt and aquifers poisoned. The land is rendered permanently barren."), *TargetProvince)
		);
		break;
	}

	OnConquestVictory.Broadcast(TargetProvince, LootedGold, EnslavedPops);
}

bool UDominionMilitarySubsystem::PayDonativumToLegions(float GoldAmount)
{
	if (GoldAmount >= ArmyStrength * 2.0f)
	{
		SavageryState.LegionDisloyalty = FMath::Max(0.0f, SavageryState.LegionDisloyalty - 40.0f);
		return true;
	}
	return false;
}
