#include "DominionGeopoliticalAISubsystem.h"

void UDominionGeopoliticalAISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ActiveRulers.Empty();
    PlayerGlobalInfamy = 0.0f;
    PlayerGlobalSavagery = 0.0f;
}

void UDominionGeopoliticalAISubsystem::Deinitialize()
{
    ActiveRulers.Empty();
    Super::Deinitialize();
}

void UDominionGeopoliticalAISubsystem::RegisterRuler(FName RulerName, EAIRulerArchetype Archetype)
{
    if (!ActiveRulers.Contains(RulerName))
    {
        FRulerState NewRuler;
        NewRuler.RulerName = RulerName;
        NewRuler.Archetype = Archetype;
        ActiveRulers.Add(RulerName, NewRuler);
    }
}

void UDominionGeopoliticalAISubsystem::UpdatePlayerThreat(float AddedInfamy, float AddedSavagery)
{
    PlayerGlobalInfamy += AddedInfamy;
    PlayerGlobalSavagery += AddedSavagery;
    ProcessCoalitions();
}

void UDominionGeopoliticalAISubsystem::ProcessCoalitions()
{
    if (PlayerGlobalSavagery > CoalitionThreshold)
    {
        for (auto& Elem : ActiveRulers)
        {
            FRulerState& Ruler = Elem.Value;
            if (Ruler.Archetype == EAIRulerArchetype::MerchantPrince || Ruler.Archetype == EAIRulerArchetype::FanaticZealot)
            {
                if (!Ruler.ActiveCoalitions.Contains(FName("AntiPlayerCoalition")))
                {
                    Ruler.ActiveCoalitions.Add(FName("AntiPlayerCoalition"));
                    // Logic to establish joint military coalition
                }
            }
        }
    }
}

void UDominionGeopoliticalAISubsystem::GenerateCassusBelli(FName SourceRuler, FName TargetRuler)
{
    if (ActiveRulers.Contains(SourceRuler) && ActiveRulers.Contains(TargetRuler))
    {
        FRulerState& Source = ActiveRulers[SourceRuler];
        // Cassus Belli logic based on archetype
        if (Source.Archetype == EAIRulerArchetype::FanaticZealot)
        {
            // Holy war against secular states
        }
        else if (Source.Archetype == EAIRulerArchetype::NomadicWarlord)
        {
            // Opportunistic granary raids
        }
    }
}

void UDominionGeopoliticalAISubsystem::NegotiateHostage(FName RequestingRuler, FName TargetRuler)
{
    if (ActiveRulers.Contains(RequestingRuler) && ActiveRulers.Contains(TargetRuler))
    {
        FRulerState& Source = ActiveRulers[RequestingRuler];
        if (Source.Archetype == EAIRulerArchetype::GodKing)
        {
            // Dynastic dominance logic
        }
    }
}

void UDominionGeopoliticalAISubsystem::ApplyTradeEmbargo(FName SourceRuler, FName TargetRuler)
{
    if (ActiveRulers.Contains(SourceRuler) && ActiveRulers.Contains(TargetRuler))
    {
        FRulerState& Source = ActiveRulers[SourceRuler];
        if (Source.Archetype == EAIRulerArchetype::MerchantPrince)
        {
            // Embargo logic
        }
    }
}
