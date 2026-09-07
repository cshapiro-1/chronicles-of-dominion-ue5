#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DominionTechTreeSubsystem.generated.h"

UENUM(BlueprintType)
enum class ETechEpoch : uint8
{
    EpochI_Bronze UMETA(DisplayName = "Epoch I: Bronze"),
    EpochII_Iron UMETA(DisplayName = "Epoch II: Iron"),
    EpochIII_Feudal UMETA(DisplayName = "Epoch III: Feudal"),
    EpochIV_Steam UMETA(DisplayName = "Epoch IV: Steam")
};

USTRUCT(BlueprintType)
struct FTechInvention
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TechName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ETechEpoch Epoch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> Prerequisites;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsUnlocked;

    FTechInvention() : Epoch(ETechEpoch::EpochI_Bronze), bIsUnlocked(false) {}
};

/**
 * Subsystem for 4-Epoch Tech Progression
 */
UCLASS()
class DOMINIONCORE_API UDominionTechTreeSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "TechTree")
    void RegisterTech(FName TechName, ETechEpoch Epoch, TArray<FName> Prerequisites);

    UFUNCTION(BlueprintCallable, Category = "TechTree")
    bool UnlockTech(FName TechName);

    UFUNCTION(BlueprintCallable, Category = "TechTree")
    bool IsTechUnlocked(FName TechName) const;

    UFUNCTION(BlueprintCallable, Category = "TechTree")
    void AdvanceEpoch(ETechEpoch NewEpoch);

private:
    UPROPERTY()
    TMap<FName, FTechInvention> TechTree;

    UPROPERTY()
    ETechEpoch CurrentEpoch;
};
