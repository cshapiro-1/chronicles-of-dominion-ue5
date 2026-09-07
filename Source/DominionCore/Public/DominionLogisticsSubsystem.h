#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DominionLogisticsSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FProvincialQuota
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Logistics")
    FName ProvinceID;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Logistics")
    int32 TargetDaysOfFood;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Logistics")
    float CurrentFoodBuffer;
};

USTRUCT(BlueprintType)
struct FLogisticsNode
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Logistics")
    FName NodeID;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Logistics")
    FVector Location;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Logistics")
    float WarehouseGrainCapacity;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Logistics")
    float CurrentGrain;
};

USTRUCT(BlueprintType)
struct FLogisticsRoute
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Logistics")
    FName RouteID;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Logistics")
    FName StartNodeID;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Logistics")
    FName EndNodeID;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Logistics")
    bool bIsRiverBarge;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Logistics")
    float BaseFriction;

    UPROPERTY(BlueprintReadWrite, Category = "Dominion|Logistics")
    float CongestionLevel;
};

UCLASS(BlueprintType, Blueprintable)
class DOMINIONCORE_API UDominionLogisticsSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Optional: implement Tick if needed for dynamic updates
    virtual void Tick(float DeltaTime);

    // Getters and updaters
    UFUNCTION(BlueprintCallable, Category = "Dominion|Logistics")
    void RegisterNode(const FLogisticsNode& NewNode);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Logistics")
    void RegisterRoute(const FLogisticsRoute& NewRoute);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Logistics")
    void SetProvincialQuota(FName ProvinceID, int32 TargetDays);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Logistics")
    void UpdateCongestionHeatmap(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Logistics")
    float CalculateRouteThroughput(FName RouteID) const;

protected:
    UPROPERTY()
    TMap<FName, FLogisticsNode> Nodes;

    UPROPERTY()
    TMap<FName, FLogisticsRoute> Routes;

    UPROPERTY()
    TMap<FName, FProvincialQuota> Quotas;
};
