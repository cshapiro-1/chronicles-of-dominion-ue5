#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DominionTypes.h"
#include "DominionGameModeBase.generated.h"

class ADominionRTSPawn;
class ADominionRTSPlayerController;
class ADominionRTSHUD;
class ADominionUnitActor;
class ADominionBuildingActor;

/**
 * ADominionGameModeBase
 * Master GameMode binding PlayerController, RTS Camera Rig, AoE2 HUD,
 * and Extra-Small Playable MVP Economy & Unit Spawning.
 */
UCLASS()
class DOMINIONCORE_API ADominionGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    ADominionGameModeBase();

    virtual void StartPlay() override;
    virtual void Tick(float DeltaTime) override;

    // --- Global 3-Resource Economy ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Economy")
    float Grain = 350.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Economy")
    float Clay = 250.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Economy")
    float Bronze = 180.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Economy")
    float GrainIncomeRate = 2.5f; // Passive agrarian harvest tick

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Economy")
    float ClayIncomeRate = 1.5f;  // Passive riverbed brickworks tick

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Economy")
    float BronzeIncomeRate = 1.0f; // Passive smelter metallurgy tick

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Combat")
    int32 CurrentWave = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Combat")
    int32 EnemyKillCount = 0;

    UFUNCTION(BlueprintPure, Category = "Dominion|Economy")
    bool CanAfford(float InGrain, float InClay, float InBronze) const;

    UFUNCTION(BlueprintCallable, Category = "Dominion|Economy")
    bool SpendResources(float InGrain, float InClay, float InBronze);

    // --- Unit Spawning API ---
    UFUNCTION(BlueprintCallable, Category = "Dominion|Spawning")
    ADominionUnitActor* SpawnSpearman(int32 TeamID = 0, FVector Location = FVector::ZeroVector);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Spawning")
    ADominionUnitActor* SpawnSlinger(int32 TeamID = 0, FVector Location = FVector::ZeroVector);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Spawning")
    ADominionUnitActor* SpawnChariot(int32 TeamID = 0, FVector Location = FVector::ZeroVector);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Spawning")
    ADominionUnitActor* SpawnBaggageTrain(int32 TeamID = 0, FVector Location = FVector::ZeroVector);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Spawning")
    void SpawnSpearmenBatch(int32 Count = 5, int32 TeamID = 0);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Spawning")
    ADominionUnitActor* SpawnEnemyDummy(FVector Location = FVector(800.0f, 0.0f, 100.0f));

    UFUNCTION(BlueprintCallable, Category = "Dominion|Spawning")
    void SpawnEnemyWave(int32 SpearmenCount = 3, int32 SlingersCount = 2);

private:
    float ResourceTickAccumulator = 0.0f;
    float WaveTimerAccumulator = 0.0f;
};



