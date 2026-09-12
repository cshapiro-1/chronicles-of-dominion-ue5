#include "DominionModularBuildingComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"

UDominionModularBuildingComponent::UDominionModularBuildingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDominionModularBuildingComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Auto-discover child mesh components attached to owner actor
    AActor* Owner = GetOwner();
    if (Owner)
    {
        TArray<UStaticMeshComponent*> MeshComps;
        Owner->GetComponents<UStaticMeshComponent>(MeshComps);
        if (MeshComps.Num() > 0) MainStructureMeshComponent = MeshComps[0];
        if (MeshComps.Num() > 1) RoofMeshComponent = MeshComps[1];
        if (MeshComps.Num() > 2) FoundationMeshComponent = MeshComps[2];
    }
}

void UDominionModularBuildingComponent::ApplyVisualTheme(UDominionBuildingVisualAsset* NewVisualAsset)
{
    if (!NewVisualAsset) return;
    CurrentVisualAsset = NewVisualAsset;

    // Hot-swap static meshes asynchronously or synchronously
    if (MainStructureMeshComponent && NewVisualAsset->MainStructureMesh.IsValid())
    {
        MainStructureMeshComponent->SetMobility(EComponentMobility::Movable);
        MainStructureMeshComponent->SetStaticMesh(NewVisualAsset->MainStructureMesh.Get());
    }

    if (RoofMeshComponent && NewVisualAsset->RoofMesh.IsValid())
    {
        RoofMeshComponent->SetMobility(EComponentMobility::Movable);
        RoofMeshComponent->SetStaticMesh(NewVisualAsset->RoofMesh.Get());
    }

    if (FoundationMeshComponent && NewVisualAsset->BaseFoundationMesh.IsValid())
    {
        FoundationMeshComponent->SetMobility(EComponentMobility::Movable);
        FoundationMeshComponent->SetStaticMesh(NewVisualAsset->BaseFoundationMesh.Get());
    }

    // Apply culture material skin with slot bounds verification
    if (MainStructureMeshComponent && NewVisualAsset->PrimaryMaterialSkin.IsValid())
    {
        if (MainStructureMeshComponent->GetNumMaterials() > 0)
        {
            MainStructureMeshComponent->SetMaterial(0, NewVisualAsset->PrimaryMaterialSkin.Get());
        }
    }
}

void UDominionModularBuildingComponent::SwapRoofMesh(UStaticMesh* NewRoofMesh)
{
    if (RoofMeshComponent && NewRoofMesh)
    {
        RoofMeshComponent->SetMobility(EComponentMobility::Movable);
        RoofMeshComponent->SetStaticMesh(NewRoofMesh);
    }
}

void UDominionModularBuildingComponent::ApplyDamageBurnOverlay(float DamagePercent)
{
    if (!MainStructureMeshComponent || MainStructureMeshComponent->GetNumMaterials() == 0) return;

    // Apply procedural charred burn mask dynamically via Material Instance Dynamic
    UMaterialInstanceDynamic* DynMat = MainStructureMeshComponent->CreateAndSetMaterialInstanceDynamic(0);
    if (DynMat)
    {
        DynMat->SetScalarParameterValue(TEXT("BurnAmount"), FMath::Clamp(DamagePercent, 0.0f, 1.0f));
    }
}

