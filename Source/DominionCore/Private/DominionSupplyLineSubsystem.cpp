#include "DominionSupplyLineSubsystem.h"
#include "DominionUnitActor.h"
#include "DominionBuildingActor.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void UDominionSupplyLineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    bSupplyInterdictedTest = false;
}

void UDominionSupplyLineSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UDominionSupplyLineSubsystem::ToggleSupplyInterdiction()
{
    bSupplyInterdictedTest = !bSupplyInterdictedTest;
}

void UDominionSupplyLineSubsystem::Tick(float DeltaTime)
{
    ProcessStarvationAndMutiny(DeltaTime);

    UWorld* World = GetWorld();
    if (!World) return;

    // 1. Gather all friendly combat units, mobile ox-carts, and supply buildings
    TArray<AActor*> AllUnitActors;
    UGameplayStatics::GetAllActorsOfClass(World, ADominionUnitActor::StaticClass(), AllUnitActors);

    TArray<AActor*> AllBuildingActors;
    UGameplayStatics::GetAllActorsOfClass(World, ADominionBuildingActor::StaticClass(), AllBuildingActors);

    TArray<ADominionUnitActor*> FriendlyCombatUnits;
    TArray<ADominionUnitActor*> FriendlyOxCarts;
    TArray<ADominionBuildingActor*> FriendlySupplyBases;

    for (AActor* Act : AllUnitActors)
    {
        if (ADominionUnitActor* Unit = Cast<ADominionUnitActor>(Act))
        {
            if (Unit->TeamID == 0) // Player
            {
                if (Unit->UnitType == EDominionUnitType::OxCartSupply)
                {
                    FriendlyOxCarts.Add(Unit);
                }
                else if (Unit->UnitType == EDominionUnitType::BronzeSpearman || Unit->UnitType == EDominionUnitType::HeavyChariot)
                {
                    FriendlyCombatUnits.Add(Unit);
                }
            }
        }
    }

    for (AActor* Act : AllBuildingActors)
    {
        if (ADominionBuildingActor* Building = Cast<ADominionBuildingActor>(Act))
        {
            if (Building->TeamID == 0)
            {
                FriendlySupplyBases.Add(Building);
            }
        }
    }

    // 2. Process Haversack Rations & Supply Tether per individual combat unit
    for (ADominionUnitActor* Unit : FriendlyCombatUnits)
    {
        if (!IsValid(Unit)) continue;

        bool bInSupplyRange = false;
        float MinDist = 999999.0f;

        if (!bSupplyInterdictedTest)
        {
            // Check mobile Ox-Carts (tether radius: 2,800 cm)
            for (ADominionUnitActor* Cart : FriendlyOxCarts)
            {
                if (IsValid(Cart) && Cart->Health > 0.0f)
                {
                    float Dist = FVector::Dist2D(Cart->GetActorLocation(), Unit->GetActorLocation());
                    if (Dist < MinDist)
                    {
                        MinDist = Dist;
                    }
                    if (Dist <= 2800.0f)
                    {
                        bInSupplyRange = true;
                        break;
                    }
                }
            }

            // Check static Granaries / Citadels / Marching Depots (radius: 3,500 cm)
            if (!bInSupplyRange)
            {
                for (ADominionBuildingActor* Base : FriendlySupplyBases)
                {
                    if (IsValid(Base))
                    {
                        float Dist = FVector::Dist2D(Base->GetActorLocation(), Unit->GetActorLocation());
                        if (Dist <= 3500.0f)
                        {
                            bInSupplyRange = true;
                            break;
                        }
                    }
                }
            }
        }

        // Apply 3-minute Haversack operational buffer vs. fast replenishment (+30s/s)
        if (bInSupplyRange)
        {
            Unit->ReplenishFieldRations(30.0f * DeltaTime);
        }
        else
        {
            Unit->ConsumeFieldRations(DeltaTime);
        }
    }

    // 3. Update Ox-Cart visual overhead status
    for (ADominionUnitActor* Cart : FriendlyOxCarts)
    {
        if (IsValid(Cart))
        {
            if (bSupplyInterdictedTest)
            {
                if (Cart->OverheadStatusText)
                {
                    Cart->OverheadStatusText->SetText(FText::FromString(TEXT("[!] SUPPLY INTERDICTED / AMBUSHED!")));
                    Cart->OverheadStatusText->SetTextRenderColor(FColor(255, 40, 40));
                }
            }
            else
            {
                if (Cart->OverheadStatusText)
                {
                    Cart->OverheadStatusText->SetText(FText::FromString(TEXT("[GRAIN SUPPLY CART: ACTIVE — REPLENISHING RATIONS]")));
                    Cart->OverheadStatusText->SetTextRenderColor(FColor(50, 255, 120));
                }
            }
        }
    }
}

void UDominionSupplyLineSubsystem::RegisterArmy(const FArmySupplyStatus& ArmyStatus)
{
    Armies.Add(ArmyStatus.ArmyID, ArmyStatus);
}

void UDominionSupplyLineSubsystem::DispatchBaggageTrain(FName TrainID, FName GranaryID, FName ArmyID, float SupplyAmount, FVector StartLocation)
{
    FBaggageTrain Train;
    Train.TrainID = TrainID;
    Train.SourceGranaryID = GranaryID;
    Train.TargetArmyID = ArmyID;
    Train.CarriedSupplies = SupplyAmount;
    Train.CurrentLocation = StartLocation;
    Train.Status = ESupplyLineStatus::Active;

    BaggageTrains.Add(TrainID, Train);
}

void UDominionSupplyLineSubsystem::ResolveInterception(FName TrainID, float AmountStolen, bool bTrainDestroyed)
{
    FBaggageTrain* Train = BaggageTrains.Find(TrainID);
    if (Train)
    {
        Train->CarriedSupplies = FMath::Max(0.0f, Train->CarriedSupplies - AmountStolen);
        if (bTrainDestroyed || Train->CarriedSupplies <= 0.0f)
        {
            Train->Status = ESupplyLineStatus::Severed;
            BaggageTrains.Remove(TrainID);
        }
        else
        {
            Train->Status = ESupplyLineStatus::Interdicted;
        }
    }
}

void UDominionSupplyLineSubsystem::ProcessStarvationAndMutiny(float DeltaTime)
{
    float DaysPassed = DeltaTime / 60.0f;

    for (auto& Elem : Armies)
    {
        FArmySupplyStatus& Army = Elem.Value;

        Army.CurrentSupplies -= Army.DailyConsumptionRate * DaysPassed;

        if (Army.CurrentSupplies <= 0.0f)
        {
            Army.CurrentSupplies = 0.0f;
            Army.Morale -= 10.0f * DaysPassed;
        }
        else
        {
            Army.Morale = FMath::Min(100.0f, Army.Morale + 2.0f * DaysPassed);
        }

        if (Army.Morale < 20.0f && !Army.bIsMutinous)
        {
            Army.bIsMutinous = true;
        }
    }
}
