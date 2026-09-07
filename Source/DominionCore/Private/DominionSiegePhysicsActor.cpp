#include "DominionSiegePhysicsActor.h"
#include "NiagaraComponent.h"
#include "Components/PointLightComponent.h"

ADominionSiegePhysicsActor::ADominionSiegePhysicsActor()
{
    PrimaryActorTick.bCanEverTick = true;

    DynamicLumenBrazierLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("LumenBrazierLight"));
    DynamicLumenBrazierLight->SetLightColor(FLinearColor(1.0f, 0.35f, 0.05f));
    DynamicLumenBrazierLight->SetIntensity(0.0f);
    SetRootComponent(DynamicLumenBrazierLight);

    FireEmbersVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireEmbersVFX"));
    FireEmbersVFX->SetupAttachment(RootComponent);
    FireEmbersVFX->SetAutoActivate(false);
}

void ADominionSiegePhysicsActor::ApplySiegeImpact(FVector ImpactLocation, float ImpactForce, float FireIgnitionChance)
{
    StructuralIntegrity -= ImpactForce;

    if (StructuralIntegrity <= 0.0f)
    {
        // Chaos Destruction: triggers physics fracture simulation in UE5
    }

    if (FMath::FRand() < FireIgnitionChance && !bIsOnFire)
    {
        IgniteStructure();
    }
}

void ADominionSiegePhysicsActor::IgniteStructure()
{
    bIsOnFire = true;
    if (FireEmbersVFX) FireEmbersVFX->Activate(true);
    if (DynamicLumenBrazierLight) DynamicLumenBrazierLight->SetIntensity(5000.0f); // Illuminates surrounding city with Lumen
}

