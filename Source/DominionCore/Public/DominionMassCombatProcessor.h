#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MassEntityTypes.h"
#include "DominionMassCombatProcessor.generated.h"

struct FDominionPhalanxFragment : public FMassFragment
{
    float Health = 100.0f;
    float Morale = 100.0f;
    float ArmorPenetration = 15.0f;
    int32 FormationRank = 1;
    bool bInShieldWall = true;
};

/**
 * UDominionMassCombatProcessor
 * High-performance SIMD multi-threaded entity processor.
 * Calculates physics collision, shield wall pushback, archery deflection,
 * and routing mechanics for 20,000+ units concurrently in Unreal Engine 5.
 */
UCLASS()
class DOMINIONCORE_API UDominionMassCombatProcessor : public UMassProcessor
{
    GENERATED_BODY()

public:
    UDominionMassCombatProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery PhalanxCombatQuery;
};


