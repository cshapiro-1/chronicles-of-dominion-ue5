#include "DominionSupplyLineSubsystem.h"
#include "DominionUnitActor.h"
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

    // 1. Gather all friendly combat units and ox-carts
    TArray<AActor*> AllUnitActors;
    UGameplayStatics::GetAllActorsOfClass(World, ADominionUnitActor::StaticClass(), AllUnitActors);

    TArray<ADominionUnitActor*> FriendlyCombatUnits;
    TArray<ADominionUnitActor*> FriendlyOxCarts;
    FVector CohortCentroid = FVector::ZeroVector;

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
                    CohortCentroid += Unit->GetActorLocation();
                }
            }
        }
    }

    if (FriendlyCombatUnits.Num() > 0)
    {
        CohortCentroid /= (float)FriendlyCombatUnits.Num();
    }

    // 2. Check if active Ox-Cart is within supply tether range (2,800 cm)
    bool bSupplyLineActive = false;
    ADominionUnitActor* NearestOxCart = nullptr;
    float MinCartDist = 999999.0f;

    if (!bSupplyInterdictedTest && FriendlyOxCarts.Num() > 0)
    {
        for (ADominionUnitActor* Cart : FriendlyOxCarts)
        {
            if (IsValid(Cart) && Cart->Health > 0.0f)
            {
                float Dist = FVector::Dist2D(Cart->GetActorLocation(), CohortCentroid);
                if (Dist < MinCartDist)
                {
                    MinCartDist = Dist;
                    NearestOxCart = Cart;
                }
            }
        }

        if (NearestOxCart && MinCartDist <= 2800.0f)
        {
            bSupplyLineActive = true;
        }
    }

    // 3. Apply Starvation / Sustenance state to all combat units
    for (ADominionUnitActor* Unit : FriendlyCombatUnits)
    {
        if (IsValid(Unit))
        {
            Unit->SetStarvingState(!bSupplyLineActive);
        }
    }

    // 4. Update Ox-Cart visual overhead status
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
            else if (bSupplyLineActive)
            {
                if (Cart->OverheadStatusText)
                {
                    Cart->OverheadStatusText->SetText(FText::FromString(TEXT("[GRAIN SUPPLY CART: 100% - FEEDING COHORT]")));
                    Cart->OverheadStatusText->SetTextRenderColor(FColor(50, 255, 120));
                }
            }
            else
            {
                if (Cart->OverheadStatusText)
                {
                    Cart->OverheadStatusText->SetText(FText::FromString(TEXT("[!] CART OUT OF RANGE - COHORT STARVING!")));
                    Cart->OverheadStatusText->SetTextRenderColor(FColor(255, 160, 30));
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
