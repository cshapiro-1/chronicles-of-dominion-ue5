#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DominionSupplyLineSubsystem.generated.h"

UENUM(BlueprintType)
enum class ESupplyLineStatus : uint8
{
    Active,
    Strained,
    Interdicted,
    Severed
};

USTRUCT(BlueprintType)
struct FArmySupplyStatus
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Supply")
    FName ArmyID;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Supply")
    float CurrentSupplies;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Supply")
    float MaxSupplies;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Supply")
    float DailyConsumptionRate;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Supply")
    float Morale;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Supply")
    bool bIsMutinous;
};

USTRUCT(BlueprintType)
struct FBaggageTrain
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Supply")
    FName TrainID;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Supply")
    FName SourceGranaryID;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Supply")
    FName TargetArmyID;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Supply")
    float CarriedSupplies;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Supply")
    FVector CurrentLocation;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Supply")
    ESupplyLineStatus Status;
};

UCLASS(BlueprintType, Blueprintable)
class DOMINIONCORE_API UDominionSupplyLineSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UDominionSupplyLineSubsystem, STATGROUP_Tickables); }

    UFUNCTION(BlueprintCallable, Category = "Dominion|Supply")
    void RegisterArmy(const FArmySupplyStatus& ArmyStatus);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Supply")
    void DispatchBaggageTrain(FName TrainID, FName GranaryID, FName ArmyID, float SupplyAmount, FVector StartLocation);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Supply")
    void ResolveInterception(FName TrainID, float AmountStolen, bool bTrainDestroyed);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Supply")
    void ProcessStarvationAndMutiny(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Supply")
    void ToggleSupplyInterdiction();

    UFUNCTION(BlueprintPure, Category = "Dominion|Supply")
    bool IsSupplyLineActive() const { return !bSupplyInterdictedTest; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Supply")
    bool bSupplyInterdictedTest = false;

protected:
    UPROPERTY()
    TMap<FName, FArmySupplyStatus> Armies;

    UPROPERTY()
    TMap<FName, FBaggageTrain> BaggageTrains;
};
