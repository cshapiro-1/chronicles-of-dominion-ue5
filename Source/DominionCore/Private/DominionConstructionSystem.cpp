#include "DominionConstructionSystem.h"

void UDominionConstructionSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ActiveSites.Empty();
	NextSiteId = 1;
	WindDirection = FVector2D(1.0f, 0.2f).GetSafeNormal();
}

void UDominionConstructionSystem::ProcessConstructionTick(float DeltaTime)
{
	for (int32 i = ActiveSites.Num() - 1; i >= 0; --i)
	{
		FDominionConstructionSite& Site = ActiveSites[i];

		switch (Site.CurrentPhase)
		{
		case EDominionConstructionPhase::SurveyingEarthworks:
			Site.Progress += 20.0f * DeltaTime;
			if (Site.Progress >= 100.0f)
			{
				Site.Progress = 0.0f;
				Site.CurrentPhase = EDominionConstructionPhase::MaterialHauling;
			}
			break;

		case EDominionConstructionPhase::MaterialHauling:
			// Awaits material delivery from logistics carts
			if (Site.DeliveredTimber >= Site.RequiredTimber && Site.DeliveredStone >= Site.RequiredStone)
			{
				Site.Progress = 0.0f;
				Site.CurrentPhase = EDominionConstructionPhase::ScaffoldingAndFraming;
			}
			break;

		case EDominionConstructionPhase::ScaffoldingAndFraming:
			Site.Progress += 15.0f * DeltaTime;
			if (Site.Progress >= 100.0f)
			{
				Site.Progress = 0.0f;
				Site.CurrentPhase = EDominionConstructionPhase::NaniteFinishing;
			}
			break;

		case EDominionConstructionPhase::NaniteFinishing:
			Site.Progress += 25.0f * DeltaTime;
			if (Site.Progress >= 100.0f)
			{
				Site.CurrentPhase = EDominionConstructionPhase::CompletedOccupied;
				// Remove completed construction site from active queue
				ActiveSites.RemoveAt(i);
			}
			break;

		default:
			break;
		}
	}
}

int32 UDominionConstructionSystem::CreateConstructionSite(FVector Location, EDominionZoneType ZoneType, int32 TimberCost, int32 StoneCost)
{
	FDominionConstructionSite NewSite;
	NewSite.SiteId = NextSiteId++;
	NewSite.Location = Location;
	NewSite.ZoneType = ZoneType;
	NewSite.CurrentPhase = EDominionConstructionPhase::SurveyingEarthworks;
	NewSite.Progress = 0.0f;
	NewSite.RequiredTimber = TimberCost;
	NewSite.DeliveredTimber = 0;
	NewSite.RequiredStone = StoneCost;
	NewSite.DeliveredStone = 0;

	ActiveSites.Add(NewSite);
	return NewSite.SiteId;
}

void UDominionConstructionSystem::DeliverMaterials(int32 SiteId, int32 TimberAmount, int32 StoneAmount)
{
	for (FDominionConstructionSite& Site : ActiveSites)
	{
		if (Site.SiteId == SiteId)
		{
			Site.DeliveredTimber = FMath::Min(Site.RequiredTimber, Site.DeliveredTimber + TimberAmount);
			Site.DeliveredStone = FMath::Min(Site.RequiredStone, Site.DeliveredStone + StoneAmount);
			break;
		}
	}
}
