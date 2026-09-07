#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DominionTutorialSubsystem.generated.h"

UENUM(BlueprintType)
enum class EDominionTutorialStep : uint8
{
	Step1_Selection       UMETA(DisplayName = "1. Army Selection"),
	Step2_Formations      UMETA(DisplayName = "2. Tactical Formations"),
	Step3_Movement        UMETA(DisplayName = "3. Marching Orders"),
	Step4_Combat          UMETA(DisplayName = "4. Battle & Flanking"),
	Step5_Recruitment     UMETA(DisplayName = "5. City Production"),
	Step6_MasteryCompleted UMETA(DisplayName = "6. Tutorial Complete")
};

UCLASS()
class DOMINIONCORE_API UDominionTutorialSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Step progression notifications */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Tutorial")
	void NotifyArmySelected(int32 UnitCount);

	UFUNCTION(BlueprintCallable, Category = "Dominion|Tutorial")
	void NotifyFormationTriggered(const FString& FormationName);

	UFUNCTION(BlueprintCallable, Category = "Dominion|Tutorial")
	void NotifyMovementCommand();

	UFUNCTION(BlueprintCallable, Category = "Dominion|Tutorial")
	void NotifyCombatStarted();

	UFUNCTION(BlueprintCallable, Category = "Dominion|Tutorial")
	void NotifyUnitRecruited(const FString& UnitName);

	UFUNCTION(BlueprintCallable, Category = "Dominion|Tutorial")
	void AdvanceStep();

	/** Getters for HUD rendering */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Tutorial")
	EDominionTutorialStep GetCurrentStep() const { return CurrentStep; }

	UFUNCTION(BlueprintCallable, Category = "Dominion|Tutorial")
	FString GetObjectiveTitle() const;

	UFUNCTION(BlueprintCallable, Category = "Dominion|Tutorial")
	FString GetObjectiveDescription() const;

	UFUNCTION(BlueprintCallable, Category = "Dominion|Tutorial")
	FString GetAdvisorAdvice() const;

	UFUNCTION(BlueprintCallable, Category = "Dominion|Tutorial")
	int32 GetStepNumber() const { return static_cast<int32>(CurrentStep) + 1; }

	UFUNCTION(BlueprintCallable, Category = "Dominion|Tutorial")
	int32 GetTotalSteps() const { return 5; }

	UFUNCTION(BlueprintCallable, Category = "Dominion|Tutorial")
	bool IsCompleted() const { return CurrentStep == EDominionTutorialStep::Step6_MasteryCompleted; }

private:
	UPROPERTY()
	EDominionTutorialStep CurrentStep = EDominionTutorialStep::Step1_Selection;

	int32 FormationsTestedCount = 0;
};
