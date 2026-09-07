#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DominionGeopoliticalAISubsystem.generated.h"

UENUM(BlueprintType)
enum class EAIRulerArchetype : uint8
{
    MerchantPrince UMETA(DisplayName = "Merchant Prince"),
    GodKing UMETA(DisplayName = "God-King"),
    NomadicWarlord UMETA(DisplayName = "Nomadic Warlord"),
    FanaticZealot UMETA(DisplayName = "Fanatic Zealot")
};

USTRUCT(BlueprintType)
struct FRulerState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RulerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAIRulerArchetype Archetype;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float InfamyIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SavageryIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> ActiveCoalitions;

    FRulerState() : Archetype(EAIRulerArchetype::MerchantPrince), InfamyIndex(0.0f), SavageryIndex(0.0f) {}
};

/**
 * Subsystem for Geopolitical AI Rulers, Anti-Snowball Coalitions
 */
UCLASS()
class DOMINIONCORE_API UDominionGeopoliticalAISubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Geopolitics")
    void RegisterRuler(FName RulerName, EAIRulerArchetype Archetype);

    UFUNCTION(BlueprintCallable, Category = "Geopolitics")
    void UpdatePlayerThreat(float AddedInfamy, float AddedSavagery);

    UFUNCTION(BlueprintCallable, Category = "Geopolitics")
    void ProcessCoalitions();

    UFUNCTION(BlueprintCallable, Category = "Geopolitics")
    void GenerateCassusBelli(FName SourceRuler, FName TargetRuler);

    UFUNCTION(BlueprintCallable, Category = "Geopolitics")
    void NegotiateHostage(FName RequestingRuler, FName TargetRuler);

    UFUNCTION(BlueprintCallable, Category = "Geopolitics")
    void ApplyTradeEmbargo(FName SourceRuler, FName TargetRuler);

private:
    UPROPERTY()
    TMap<FName, FRulerState> ActiveRulers;

    float PlayerGlobalInfamy = 0.0f;
    float PlayerGlobalSavagery = 0.0f;
    const float CoalitionThreshold = 50.0f;
};
