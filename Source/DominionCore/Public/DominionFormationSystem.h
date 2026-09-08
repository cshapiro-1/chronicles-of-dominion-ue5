#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DominionTypes.h"
#include "DominionFormationSystem.generated.h"

/** Backward-compatibility formation type */
UENUM(BlueprintType)
enum class EFormationType : uint8
{
    None            UMETA(DisplayName = "None"),
    Phalanx         UMETA(DisplayName = "Phalanx Shield Wall"),
    Wedge           UMETA(DisplayName = "Wedge Shock Charge"),
    Skirmish        UMETA(DisplayName = "Skirmish Dispersion"),
    Square          UMETA(DisplayName = "Square Defense")
};

USTRUCT(BlueprintType)
struct FFormationCombatStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BracingBonus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PenetrationPower;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MissileVulnerability;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FlankVulnerability;

    FFormationCombatStats()
        : BracingBonus(1.0f)
        , PenetrationPower(1.0f)
        , MissileVulnerability(1.0f)
        , FlankVulnerability(1.0f)
    {}
};

USTRUCT(BlueprintType)
struct FFormationData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 FormationID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EFormationType CurrentType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FFormationCombatStats CombatStats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Morale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Fatigue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector CenterLocation;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector ForwardVector;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Velocity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsRouting;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bLostBaggageTrain;

    FFormationData()
        : FormationID(0)
        , CurrentType(EFormationType::None)
        , Morale(100.0f)
        , Fatigue(0.0f)
        , CenterLocation(FVector::ZeroVector)
        , ForwardVector(FVector::ForwardVector)
        , Velocity(FVector::ZeroVector)
        , bIsRouting(false)
        , bLostBaggageTrain(false)
    {}
};

/**
 * UDominionFormationSystem
 * Subsystem responsible for Advanced Mass Formations, Physics Combat, and Morale Cascades.
 */
UCLASS(BlueprintType)
class DOMINIONCORE_API UDominionFormationSystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** Pure local slot offsets calculation for Line, Square, Phalanx, and Skirmish formations */
    UFUNCTION(BlueprintCallable, Category = "Dominion|Formation")
    TArray<FVector> GetFormationOffsets(EDominionFormation Formation, int32 UnitCount, float Spacing) const;

    // Formation Lifecycle & State
    UFUNCTION(BlueprintCallable, Category = "Dominion|Formations")
    int32 RegisterFormation(const FFormationData& InitialData);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Formations")
    void UnregisterFormation(int32 FormationID);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Formations")
    void SetFormationType(int32 FormationID, EFormationType NewType);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Formations")
    void UpdateFormationState(int32 FormationID, const FVector& Location, const FVector& Forward, const FVector& Velocity, float DeltaTime);

    /** Calculate world-space slot positions and headings for N units in a formation */
    UFUNCTION(BlueprintCallable, Category = "Dominion|Formations")
    void CalculateFormationSlots(EFormationType FormationType, int32 TotalUnits, const FVector& Destination, const FVector& HeadingVector, TArray<FVector>& OutSlotLocations, TArray<FRotator>& OutSlotRotations) const;

    // Physics & Terrain Combat Calculations
    UFUNCTION(BlueprintPure, Category = "Dominion|PhysicsCombat")
    float CalculateDownhillMomentum(const FVector& AttackerVelocity, const FVector& AttackerLocation, const FVector& DefenderLocation) const;

    UFUNCTION(BlueprintPure, Category = "Dominion|PhysicsCombat")
    float CalculateTerrainDamageBonus(const FVector& AttackerLocation, const FVector& DefenderLocation) const;

    UFUNCTION(BlueprintPure, Category = "Dominion|PhysicsCombat")
    float CalculateShieldCoverageBonus(const FVector& AttackDirection, const FVector& DefenderForward, float ShieldCoverageAngle) const;

    /** Calculate reflection damage multiplier when cavalry charges into a braced Phalanx */
    UFUNCTION(BlueprintPure, Category = "Dominion|PhysicsCombat")
    float CalculateBracingReflectionMultiplier(bool bDefenderBraced, const FVector& AttackerVelocity, const FVector& DefenderForward) const;

    // Morale & Psychological Cascade
    UFUNCTION(BlueprintCallable, Category = "Dominion|Morale")
    void ProcessRoutingEvent(int32 RoutingFormationID);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Morale")
    void ProcessBaggageTrainLost(int32 FormationID);

    UFUNCTION(BlueprintCallable, Category = "Dominion|Morale")
    FFormationData GetFormationData(int32 FormationID) const;

private:
    void ApplyFormationModifiers(FFormationData& Formation);
    void PropagatePanic(const FVector& SourceLocation, float Radius, float PanicDamage);

    TMap<int32, FFormationData> ActiveFormations;
    int32 NextFormationID = 1;

    // Tuning constants
    const float PanicRadiusBase = 5000.0f; // 50 meters
    const float RoutingPanicDamage = 20.0f;
    const float BaggageLostPanicDamage = 35.0f;
    const float FatigueRateBase = 1.0f;
};
