#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "DominionSiegePhysicsActor.generated.h"

class UGeometryCollectionComponent;
class UNiagaraComponent;
class UPointLightComponent;

/**
 * ADominionSiegePhysicsActor
 * Unreal Engine 5 Chaos Destruction & Dynamic Flame Propagation.
 * Physically shatters city walls, ziggurats, and timber roofs when struck
 * by catapult boulders, with volumetric Lumen fire illumination.
 */
UCLASS()
class DOMINIONCORE_API ADominionSiegePhysicsActor : public AActor
{
    GENERATED_BODY()

public:
    ADominionSiegePhysicsActor();

    /** Triggers physics fracture when siege projectile impacts */
    UFUNCTION(BlueprintCallable, Category = "Dominion|ChaosSiege")
    void ApplySiegeImpact(FVector ImpactLocation, float ImpactForce, float FireIgnitionChance);

    /** Spreads procedural fire to neighboring structures */
    UFUNCTION(BlueprintCallable, Category = "Dominion|ChaosSiege")
    void IgniteStructure();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UGeometryCollectionComponent> ChaosGeometryCollection;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UNiagaraComponent> FireEmbersVFX;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UPointLightComponent> DynamicLumenBrazierLight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Siege Physics")
    float StructuralIntegrity = 5000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Siege Physics")
    bool bIsOnFire = false;
};


