// Copyright Epic Games, Inc. All Rights Reserved.

#include "DominionFormationSystem.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"

void UDominionFormationSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ActiveFormations.Empty();
}

void UDominionFormationSystem::Deinitialize()
{
    ActiveFormations.Empty();
    Super::Deinitialize();
}

int32 UDominionFormationSystem::RegisterFormation(const FFormationData& InitialData)
{
    int32 NewID = NextFormationID++;
    FFormationData NewData = InitialData;
    NewData.FormationID = NewID;
    
    ApplyFormationModifiers(NewData);
    ActiveFormations.Add(NewID, NewData);
    
    return NewID;
}

void UDominionFormationSystem::UnregisterFormation(int32 FormationID)
{
    ActiveFormations.Remove(FormationID);
}

void UDominionFormationSystem::SetFormationType(int32 FormationID, EFormationType NewType)
{
    if (FFormationData* Data = ActiveFormations.Find(FormationID))
    {
        Data->CurrentType = NewType;
        ApplyFormationModifiers(*Data);
    }
}

void UDominionFormationSystem::UpdateFormationState(int32 FormationID, const FVector& Location, const FVector& Forward, const FVector& Velocity, float DeltaTime)
{
    if (FFormationData* Data = ActiveFormations.Find(FormationID))
    {
        Data->CenterLocation = Location;
        Data->ForwardVector = Forward.GetSafeNormal();
        Data->Velocity = Velocity;

        // Dynamic Fatigue Calculation
        float Speed = Velocity.Size();
        if (Speed > 100.0f) // Moving
        {
            float FatigueIncrease = (Speed / 500.0f) * FatigueRateBase * DeltaTime;
            
            // Formation type impacts fatigue rate
            if (Data->CurrentType == EFormationType::Phalanx)
            {
                FatigueIncrease *= 1.5f; // Heavy armor and tight formation increases fatigue when moving
            }
            else if (Data->CurrentType == EFormationType::Skirmish)
            {
                FatigueIncrease *= 0.8f; // Loose formation reduces fatigue
            }
            
            Data->Fatigue = FMath::Clamp(Data->Fatigue + FatigueIncrease, 0.0f, 100.0f);
            
            // High fatigue impacts morale
            if (Data->Fatigue > 80.0f)
            {
                Data->Morale -= 0.5f * DeltaTime;
            }
        }
        else
        {
            // Resting recovery
            Data->Fatigue = FMath::Clamp(Data->Fatigue - (FatigueRateBase * 0.5f * DeltaTime), 0.0f, 100.0f);
        }
        
        // Routing check
        if (Data->Morale <= 0.0f && !Data->bIsRouting)
        {
            Data->bIsRouting = true;
            ProcessRoutingEvent(FormationID);
        }
    }
}

void UDominionFormationSystem::ApplyFormationModifiers(FFormationData& Formation)
{
    // Reset to baseline
    Formation.CombatStats = FFormationCombatStats();

    switch (Formation.CurrentType)
    {
        case EFormationType::Phalanx:
            Formation.CombatStats.BracingBonus = 3.0f; // High resistance to cavalry
            Formation.CombatStats.MissileVulnerability = 0.5f; // Good frontal shield coverage
            Formation.CombatStats.FlankVulnerability = 2.0f; // Weak to flanks
            break;
            
        case EFormationType::Wedge:
            Formation.CombatStats.PenetrationPower = 2.5f; // Excellent shock charge penetration
            Formation.CombatStats.FlankVulnerability = 1.5f;
            break;
            
        case EFormationType::Skirmish:
            Formation.CombatStats.MissileVulnerability = 0.3f; // Loose formation minimizes arrow casualties
            Formation.CombatStats.BracingBonus = 0.2f; // Terrible against cavalry charge
            break;
            
        case EFormationType::Square:
            Formation.CombatStats.BracingBonus = 2.0f; 
            Formation.CombatStats.FlankVulnerability = 0.0f; // No weak flanks
            Formation.CombatStats.MissileVulnerability = 1.2f; // Dense target for arrows
            break;
            
        default:
            break;
    }
}

float UDominionFormationSystem::CalculateDownhillMomentum(const FVector& AttackerVelocity, const FVector& AttackerLocation, const FVector& DefenderLocation) const
{
    float ElevationDifference = AttackerLocation.Z - DefenderLocation.Z;
    float DownhillMultiplier = 1.0f;

    if (ElevationDifference > 0)
    {
        // Attacker is above defender
        DownhillMultiplier += FMath::Clamp(ElevationDifference / 500.0f, 0.0f, 1.0f);
    }

    float VelocityMagnitude = AttackerVelocity.Size();
    return VelocityMagnitude * DownhillMultiplier;
}

float UDominionFormationSystem::CalculateTerrainDamageBonus(const FVector& AttackerLocation, const FVector& DefenderLocation) const
{
    float ElevationDifference = AttackerLocation.Z - DefenderLocation.Z;
    
    // For every 100 unreal units (1 meter) of elevation advantage, gain 5% damage, up to 50%
    float ElevationBonus = FMath::Clamp((ElevationDifference / 100.0f) * 0.05f, -0.5f, 0.5f);
    
    return 1.0f + ElevationBonus;
}

float UDominionFormationSystem::CalculateShieldCoverageBonus(const FVector& AttackDirection, const FVector& DefenderForward, float ShieldCoverageAngle) const
{
    FVector NormalizedAttack = AttackDirection.GetSafeNormal();
    FVector NormalizedDefend = DefenderForward.GetSafeNormal();

    // Dot product between facing direction and incoming attack
    float DotP = FVector::DotProduct(NormalizedDefend, -NormalizedAttack);
    
    // Convert dot product to angle
    float Angle = FMath::Acos(DotP);
    float AngleDegrees = FMath::RadiansToDegrees(Angle);

    if (AngleDegrees <= (ShieldCoverageAngle / 2.0f))
    {
        return 0.2f; // 80% damage reduction for direct shield hit
    }
    else if (AngleDegrees <= ShieldCoverageAngle)
    {
        return 0.6f; // Glancing blow
    }

    return 1.0f; // Unshielded
}

float UDominionFormationSystem::CalculateBracingReflectionMultiplier(bool bDefenderBraced, const FVector& AttackerVelocity, const FVector& DefenderForward) const
{
    if (!bDefenderBraced)
    {
        return 0.0f;
    }

    FVector NormVel = AttackerVelocity.GetSafeNormal2D();
    FVector NormFacing = DefenderForward.GetSafeNormal2D();

    // Dot product: frontal collision when charger moves directly into braced spearmen facing
    float FrontalDot = FVector::DotProduct(NormFacing, -NormVel);
    if (FrontalDot > 0.45f) // Frontal 90 degree cone
    {
        float SpeedFactor = FMath::Clamp(AttackerVelocity.Size2D() / 400.0f, 0.5f, 2.0f);
        return 0.70f * SpeedFactor; // 70% to 140% reflection damage based on impact velocity
    }

    return 0.0f;
}

void UDominionFormationSystem::ProcessRoutingEvent(int32 RoutingFormationID)
{
    if (const FFormationData* RoutingData = ActiveFormations.Find(RoutingFormationID))
    {
        PropagatePanic(RoutingData->CenterLocation, PanicRadiusBase, RoutingPanicDamage);
    }
}

void UDominionFormationSystem::ProcessBaggageTrainLost(int32 FormationID)
{
    if (FFormationData* Data = ActiveFormations.Find(FormationID))
    {
        Data->bLostBaggageTrain = true;
        PropagatePanic(Data->CenterLocation, PanicRadiusBase * 2.0f, BaggageLostPanicDamage);
    }
}

void UDominionFormationSystem::PropagatePanic(const FVector& SourceLocation, float Radius, float PanicDamage)
{
    float RadiusSq = Radius * Radius;

    for (auto& Pair : ActiveFormations)
    {
        FFormationData& Formation = Pair.Value;
        
        if (Formation.bIsRouting)
        {
            continue;
        }

        float DistanceSq = FVector::DistSquared(SourceLocation, Formation.CenterLocation);
        if (DistanceSq <= RadiusSq)
        {
            // Dropoff panic effect based on distance
            float Distance = FMath::Sqrt(DistanceSq);
            float Intensity = 1.0f - (Distance / Radius);
            float DamageApplied = PanicDamage * Intensity;

            Formation.Morale -= DamageApplied;
            Formation.Morale = FMath::Max(Formation.Morale, 0.0f);
            
            // Panic cascade causing routing
            if (Formation.Morale <= 0.0f)
            {
                Formation.bIsRouting = true;
            }
        }
    }
}

FFormationData UDominionFormationSystem::GetFormationData(int32 FormationID) const
{
    if (const FFormationData* Data = ActiveFormations.Find(FormationID))
    {
        return *Data;
    }
    return FFormationData();
}

void UDominionFormationSystem::CalculateFormationSlots(
    EFormationType FormationType,
    int32 TotalUnits,
    const FVector& Destination,
    const FVector& HeadingVector,
    TArray<FVector>& OutSlotLocations,
    TArray<FRotator>& OutSlotRotations) const
{
    OutSlotLocations.Empty(TotalUnits);
    OutSlotRotations.Empty(TotalUnits);

    if (TotalUnits <= 0) return;

    FVector ForwardDir = HeadingVector.GetSafeNormal2D();
    if (ForwardDir.IsNearlyZero())
    {
        ForwardDir = FVector::ForwardVector;
    }
    FRotator HeadingRot = ForwardDir.Rotation();
    FVector RightDir = FRotationMatrix(HeadingRot).GetUnitAxis(EAxis::Y);

    for (int32 i = 0; i < TotalUnits; ++i)
    {
        float LocalX = 0.0f;
        float LocalY = 0.0f;

        switch (FormationType)
        {
        case EFormationType::Phalanx:
        default:
            {
                // 10-column rigid phalanx shield wall
                const int32 Cols = FMath::Clamp(10, 1, TotalUnits);
                int32 Row = i / Cols;
                int32 Col = i % Cols;
                const float SpacingX = 140.0f;
                const float SpacingY = 135.0f;

                LocalX = -Row * SpacingX;
                LocalY = (Col - (Cols - 1) * 0.5f) * SpacingY;
            }
            break;

        case EFormationType::Wedge:
            {
                // V-shaped shock charge wedge
                // Row 0 = 1 leader, Row 1 = 2 wingmen, Row 2 = 2 wingmen, etc.
                if (i == 0)
                {
                    LocalX = 0.0f;
                    LocalY = 0.0f;
                }
                else
                {
                    int32 Tier = (i + 1) / 2;
                    int32 Side = (i % 2 == 1) ? 1 : -1;
                    LocalX = -Tier * 155.0f;
                    LocalY = Side * Tier * 145.0f;
                }
            }
            break;

        case EFormationType::Skirmish:
            {
                // Loose hexagonal dispersion (250cm spacing to minimize catapult damage)
                const int32 Cols = FMath::Max(1, FMath::CeilToInt(FMath::Sqrt((float)TotalUnits * 1.5f)));
                int32 Row = i / Cols;
                int32 Col = i % Cols;
                float Stagger = (Row % 2 == 1) ? 120.0f : 0.0f;

                LocalX = -Row * 240.0f;
                LocalY = (Col - (Cols - 1) * 0.5f) * 250.0f + Stagger;
            }
            break;

        case EFormationType::Square:
            {
                // Hollow perimeter box defense
                const int32 PerimeterSide = FMath::Max(2, (TotalUnits + 3) / 4);
                int32 SideIndex = i / PerimeterSide;
                int32 StepInSide = i % PerimeterSide;
                float HalfSide = (PerimeterSide - 1) * 0.5f * 150.0f;
                float StepOffset = -HalfSide + StepInSide * 150.0f;

                if (SideIndex == 0)      { LocalX =  HalfSide; LocalY = StepOffset; }
                else if (SideIndex == 1) { LocalX = -HalfSide; LocalY = StepOffset; }
                else if (SideIndex == 2) { LocalX = StepOffset; LocalY =  HalfSide; }
                else                     { LocalX = StepOffset; LocalY = -HalfSide; }
            }
            break;
        }

        // Transform local (X=Forward, Y=Right) to world space
        FVector WorldSlot = Destination + (ForwardDir * LocalX) + (RightDir * LocalY);
        WorldSlot.Z = Destination.Z;

        OutSlotLocations.Add(WorldSlot);
        OutSlotRotations.Add(HeadingRot);
    }
}
