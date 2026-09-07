#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DominionTypes.h"
#include "DominionMilitarySubsystem.generated.h"

UENUM(BlueprintType)
enum class EDominionRecruitmentType : uint8
{
	PeasantLevyConscription UMETA(DisplayName = "Peasant Levy Conscription (Drains Farm Labor)"),
	StandingProfessionalLegion UMETA(DisplayName = "Professional Standing Legion (High Gold Upkeep)"),
	MercenaryCompany UMETA(DisplayName = "Mercenary Free Company (Immediate Bounty)"),
	GalleySlaveFleet UMETA(DisplayName = "Enslaved Galley Fleet (Chained Oarsmen)")
};

UENUM(BlueprintType)
enum class EDominionConquestPolicy : uint8
{
	PillageAndSack UMETA(DisplayName = "Pillage & Sack (Loot Gold & Burn Buildings)"),
	EnslaveAndDeport UMETA(DisplayName = "Enslave & Deport Captives (Fuel Capital Mines)"),
	TributarySubjugation UMETA(DisplayName = "Tributary Subjugation (Impose Annual Tribute)"),
	TotalImperialAnnexation UMETA(DisplayName = "Total Imperial Annexation (Install Governor)"),
	TotalAnnihilation UMETA(DisplayName = "💀 Total Annihilation (Eradicate All Inhabitants & Level Cities)"),
	SaltTheEarthAndPoisonAquifers UMETA(DisplayName = "🧂 Salt the Earth (Poison Aquifers & Permanent Wasteland)")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnConquestVictory, FString, TargetProvince, int32, LootedGold, int32, EnslavedPops);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMilitaryCrisis, FString, CrisisTitle, FString, CrisisDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSavageryBlowbackTriggered, FString, BlowbackType, FString, Details);

/**
 * Manages Military & Naval Mobilization, Armament Logistics, Baggage Trains,
 * Siege Warfare, Post-Conquest Subjugation, Total Annihilation, and Societal Savagery Blowback.
 */
UCLASS()
class DOMINIONCORE_API UDominionMilitarySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Executes one tick of military upkeep, baggage grain consumption, and attrition */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Military")
	void ProcessMilitaryTick(float DeltaTime, float AvailableTreasuryGold, float AvailableGranaryGrain);

	/** Recruits military units */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Military")
	bool MobilizeRegiment(EDominionRecruitmentType RecruitmentType, int32 TroopCount);

	/** Executes post-conquest spoils policy on captured province */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Conquest")
	void ExecuteConquestPolicy(FString TargetProvince, EDominionConquestPolicy Policy, int32 DefeatedPops);

	/** Pays blood money donativum to appease brutalized legions */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Military")
	bool PayDonativumToLegions(float GoldAmount);

	/** Gets active Army soldier count */
	UFUNCTION(BlueprintPure, Category = "Dominion|Military")
	int32 GetArmyStrength() const { return ArmyStrength; }

	/** Gets active Naval fleet warship count */
	UFUNCTION(BlueprintPure, Category = "Dominion|Military")
	int32 GetFleetStrength() const { return FleetStrength; }

	/** Gets total enslaved captives in empire */
	UFUNCTION(BlueprintPure, Category = "Dominion|Military")
	int32 GetEnslavedCaptiveCount() const { return EnslavedCaptiveCount; }

	/** Gets active Savagery Metrics */
	UFUNCTION(BlueprintPure, Category = "Dominion|Military")
	FDominionSavageryMetrics GetSavageryMetrics() const { return SavageryState; }

	UPROPERTY(BlueprintAssignable, Category = "Dominion|Military")
	FOnConquestVictory OnConquestVictory;

	UPROPERTY(BlueprintAssignable, Category = "Dominion|Military")
	FOnMilitaryCrisis OnMilitaryCrisis;

	UPROPERTY(BlueprintAssignable, Category = "Dominion|Military")
	FOnSavageryBlowbackTriggered OnSavageryBlowback;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Military")
	int32 ArmyStrength = 2500; // Active soldiers

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Military")
	int32 FleetStrength = 15; // War galleys / Frigates

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Military")
	int32 EnslavedCaptiveCount = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Military")
	float BaggageGrainRations = 100.0f; // % supplied

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Military")
	float MonthlyGoldUpkeepPerSoldier = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Military")
	FDominionSavageryMetrics SavageryState;
};
