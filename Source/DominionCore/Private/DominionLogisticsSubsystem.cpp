#include "DominionLogisticsSubsystem.h"

void UDominionLogisticsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // Initialization logic for the logistics network
}

void UDominionLogisticsSubsystem::Deinitialize()
{
    Super::Deinitialize();
    // Cleanup logic
}

void UDominionLogisticsSubsystem::Tick(float DeltaTime)
{
    UpdateCongestionHeatmap(DeltaTime);
    // Process transport, quota updates, etc.
}

void UDominionLogisticsSubsystem::RegisterNode(const FLogisticsNode& NewNode)
{
    Nodes.Add(NewNode.NodeID, NewNode);
}

void UDominionLogisticsSubsystem::RegisterRoute(const FLogisticsRoute& NewRoute)
{
    Routes.Add(NewRoute.RouteID, NewRoute);
}

void UDominionLogisticsSubsystem::SetProvincialQuota(FName ProvinceID, int32 TargetDays)
{
    FProvincialQuota& Quota = Quotas.FindOrAdd(ProvinceID);
    Quota.ProvinceID = ProvinceID;
    Quota.TargetDaysOfFood = TargetDays;
}

void UDominionLogisticsSubsystem::UpdateCongestionHeatmap(float DeltaTime)
{
    for (auto& Elem : Routes)
    {
        FLogisticsRoute& Route = Elem.Value;
        // Congestion decays over time to simulate traffic clearing up
        Route.CongestionLevel = FMath::FInterpTo(Route.CongestionLevel, 0.0f, DeltaTime, 0.1f);
    }
}

float UDominionLogisticsSubsystem::CalculateRouteThroughput(FName RouteID) const
{
    const FLogisticsRoute* Route = Routes.Find(RouteID);
    if (Route)
    {
        float BaseThroughput = Route->bIsRiverBarge ? 100.0f : 50.0f;
        float FrictionMultiplier = 1.0f / FMath::Max(1.0f, Route->BaseFriction + Route->CongestionLevel);
        return BaseThroughput * FrictionMultiplier;
    }
    return 0.0f;
}
