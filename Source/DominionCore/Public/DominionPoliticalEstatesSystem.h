#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DominionTypes.h"
#include "DominionPoliticalEstatesSystem.generated.h"

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

/**
 * Manages intra-civilization fractures:
 * 1. Dual Power balance between Religious Authority (The Church/Temple) and Political Power (The Crown/State)
 * 2. Class strife, strikes, and rebellions
 * 3. Ethnic & cultural tensions across multi-ethnic provincial populations
 */
UCLASS()
class DOMINIONCORE_API UDominionPoliticalEstatesSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Executes one tick of internal political tension calculations */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Politics")
	void ProcessPoliticsTick(float DeltaTime, float CurrentFoodSecurity, float CurrentTaxExploitation);

	/** Adjusts the Dual Power Balance */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Politics")
	void SetPowerBalance(EDominionPowerBalance NewBalance);

	/** Adjusts the Cultural / Ethnic Policy */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Politics")
	void SetCulturalPolicy(EDominionCulturalPolicy NewPolicy);

	/** Gets current Religious Authority Index (0-100) */
	UFUNCTION(BlueprintPure, Category = "Dominion|Politics")
	float GetReligiousAuthority() const { return ReligiousAuthority; }

	/** Gets current Secular State Power Index (0-100) */
	UFUNCTION(BlueprintPure, Category = "Dominion|Politics")
	float GetSecularStatePower() const { return SecularStatePower; }

	/** Gets Class Unrest Index (0-100) */
	UFUNCTION(BlueprintPure, Category = "Dominion|Politics")
	float GetClassUnrest() const { return ClassUnrest; }

	/** Gets Ethnic Separatism Index (0-100) */
	UFUNCTION(BlueprintPure, Category = "Dominion|Politics")
	float GetEthnicSeparatism() const { return EthnicSeparatism; }

	/** Applies political and religious blowback from extreme military savagery */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Politics")
	void ApplySavageryBlowback(const FDominionSavageryMetrics& Metrics);

	UPROPERTY(BlueprintAssignable, Category = "Dominion|Politics")
	FOnIntraCivCrisisTriggered OnCrisisTriggered;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Politics")
	EDominionPowerBalance PowerBalance = EDominionPowerBalance::BalancedConcordat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Politics")
	EDominionCulturalPolicy CulturalPolicy = EDominionCulturalPolicy::ImperialPluralism;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Politics")
	float ReligiousAuthority = 60.0f; // 0-100%

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Politics")
	float SecularStatePower = 60.0f; // 0-100%

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Politics")
	float ClassUnrest = 20.0f; // 0-100%

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Politics")
	float EthnicSeparatism = 25.0f; // 0-100%
};
