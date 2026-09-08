// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DominionTypes.h"
#include "DominionPoliticalEstatesSystem.generated.h"

UENUM(BlueprintType)
enum class EDominionEstateType : uint8
{
	Priesthood UMETA(DisplayName = "The High Priesthood (First Estate)"),
	Nobility   UMETA(DisplayName = "The Warlord Nobility (Second Estate)"),
	Masses     UMETA(DisplayName = "The Common Masses (Third Estate)")
};

UENUM(BlueprintType)
enum class EDominionEdictType : uint8
{
	SacredGrainTithe    UMETA(DisplayName = "Sacred Grain Tithe (Church)"),
	FeudalConscription  UMETA(DisplayName = "Feudal Corvée Conscription (Nobles)"),
	ImperialBreadDole   UMETA(DisplayName = "Imperial Bread Dole (Commoners)"),
	GladiatorCircus     UMETA(DisplayName = "Grand Gladiator & Chariot Games")
};

UENUM(BlueprintType)
enum class EDominionPowerBalance : uint8
{
	TheocraticSubjugation UMETA(DisplayName = "Theocracy (Altar Dominant)"),
	BalancedConcordat UMETA(DisplayName = "Concordat (Balanced Dual Power)"),
	SecularAutocracy UMETA(DisplayName = "Secular State (Throne Dominant)")
};

UENUM(BlueprintType)
enum class EDominionCulturalPolicy : uint8
{
	ImperialPluralism UMETA(DisplayName = "Imperial Pluralism & Provincial Autonomy"),
	ForcedAssimilation UMETA(DisplayName = "Forced Imperial Assimilation")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIntraCivCrisisTriggered, FString, CrisisTitle, FString, CrisisDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnImperialEdictEnacted, EDominionEdictType, EdictType, FString, EdictName);

/**
 * UDominionPoliticalEstatesSystem
 * Master Grand Strategy Subsystem managing:
 * 1. The 3-Estate Power Triad: High Priesthood, Warlord Nobility, Common Masses
 * 2. Frostpunk-style Hope & Discontent Gauges
 * 3. Imperial Edict Lawbook / Decrees
 * 4. Intra-Civ Crises, Strikes, Riots, and Baronial Coups
 */
UCLASS()
class DOMINIONCORE_API UDominionPoliticalEstatesSystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UDominionPoliticalEstatesSystem, STATGROUP_Tickables); }

	/** Enact an Imperial Edict from the Lawbook */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Politics")
	bool EnactEdict(EDominionEdictType EdictType);

	/** Adjust an estate's loyalty index */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Politics")
	void ModifyEstateLoyalty(EDominionEstateType Estate, float Delta);

	/** Adjust Hope meter */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Politics")
	void ModifyHope(float Delta);

	/** Adjust Discontent meter */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Politics")
	void ModifyDiscontent(float Delta);

	// --- Getters for HUD & Simulation ---
	UFUNCTION(BlueprintPure, Category = "Dominion|Politics")
	float GetPriesthoodLoyalty() const { return PriesthoodLoyalty; }

	UFUNCTION(BlueprintPure, Category = "Dominion|Politics")
	float GetNobilityLoyalty() const { return NobilityLoyalty; }

	UFUNCTION(BlueprintPure, Category = "Dominion|Politics")
	float GetMassesLoyalty() const { return MassesLoyalty; }

	UFUNCTION(BlueprintPure, Category = "Dominion|Politics")
	float GetHope() const { return Hope; }

	UFUNCTION(BlueprintPure, Category = "Dominion|Politics")
	float GetDiscontent() const { return Discontent; }

	UFUNCTION(BlueprintPure, Category = "Dominion|Politics")
	FString GetActiveEdictName() const { return ActiveEdictName; }

	UFUNCTION(BlueprintPure, Category = "Dominion|Politics")
	float GetActiveEdictRemainingTime() const { return ActiveEdictTimer; }

	UFUNCTION(BlueprintPure, Category = "Dominion|Politics")
	FString GetRecentCrisisNotification() const { return (CrisisDisplayTimer > 0.0f) ? ActiveCrisisTitle + TEXT(": ") + ActiveCrisisDesc : FString(); }

	UFUNCTION(BlueprintPure, Category = "Dominion|Politics")
	float GetCrisisDisplayTimer() const { return CrisisDisplayTimer; }

	/** Applies political and religious blowback from extreme military savagery */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Politics")
	void ApplySavageryBlowback(const FDominionSavageryMetrics& Metrics);

	UPROPERTY(BlueprintAssignable, Category = "Dominion|Politics")
	FOnIntraCivCrisisTriggered OnCrisisTriggered;

	UPROPERTY(BlueprintAssignable, Category = "Dominion|Politics")
	FOnImperialEdictEnacted OnEdictEnacted;

protected:
	// 3 Estates (0 - 100%)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Politics")
	float PriesthoodLoyalty = 65.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Politics")
	float NobilityLoyalty = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Politics")
	float MassesLoyalty = 58.0f;

	// Frostpunk Meters (0 - 100%)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Politics")
	float Hope = 65.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Politics")
	float Discontent = 22.0f;

	// Legacy / Compatibility fields
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Politics")
	EDominionPowerBalance PowerBalance = EDominionPowerBalance::BalancedConcordat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Politics")
	EDominionCulturalPolicy CulturalPolicy = EDominionCulturalPolicy::ImperialPluralism;

	// Active Edict
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dominion|Politics")
	FString ActiveEdictName = TEXT("None");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dominion|Politics")
	float ActiveEdictTimer = 0.0f;

	// Crisis Feedback
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dominion|Politics")
	FString ActiveCrisisTitle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dominion|Politics")
	FString ActiveCrisisDesc;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dominion|Politics")
	float CrisisDisplayTimer = 0.0f;

	float CrisisCooldown = 0.0f;

private:
	void CheckCrises(float DeltaTime);
	void TriggerCrisis(const FString& Title, const FString& Desc);
};
