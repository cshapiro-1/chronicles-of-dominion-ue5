#include "DominionBuildingActor.h"
#include "DominionTextureFactory.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

ADominionBuildingActor::ADominionBuildingActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f; // 10 Hz lightweight tick

	// 1. Check for custom generative game-ready mesh / fallback
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CitadelMeshFinder(TEXT("/Game/Environment/Generated/SM_Citadel.SM_Citadel"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ZigguratMeshFinder(TEXT("/Game/Environment/Generated/SM_Ziggurat_Citadel.SM_Ziggurat_Citadel"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> CitadelMatFinder(TEXT("/Game/Environment/Generated/M_Citadel.M_Citadel"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MudbrickMatFinder(TEXT("/Game/Materials/MI_Mesopotamian_Mudbrick.MI_Mesopotamian_Mudbrick"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BronzeMatFinder(TEXT("/Game/Materials/MI_Patina_Bronze.MI_Patina_Bronze"));

	UStaticMesh* ChosenMesh = CitadelMeshFinder.Succeeded() ? CitadelMeshFinder.Object :
	                          (ZigguratMeshFinder.Succeeded() ? ZigguratMeshFinder.Object :
	                          (CubeFinder.Succeeded() ? CubeFinder.Object : nullptr));
	UStaticMesh* CubeMesh = CubeFinder.Succeeded() ? CubeFinder.Object : nullptr;

	// 2. Colossal Base Foundation / Unified Static Mesh
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
	RootComponent = BuildingMesh;
	BuildingMesh->SetCollisionProfileName(TEXT("BlockAll"));
	BuildingMesh->SetMobility(EComponentMobility::Movable);
	if (ChosenMesh)
	{
		BuildingMesh->SetStaticMesh(ChosenMesh);
		BuildingMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		BuildingMesh->SetRelativeRotation(FRotator::ZeroRotator);
		BuildingMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

		// Bind PBR Material instances
		if (MudbrickMatFinder.Succeeded())
		{
			BuildingMesh->SetMaterial(0, MudbrickMatFinder.Object);
		}
		else if (CitadelMatFinder.Succeeded())
		{
			BuildingMesh->SetMaterial(0, CitadelMatFinder.Object);
		}

		if (BronzeMatFinder.Succeeded() && BuildingMesh->GetNumMaterials() > 1)
		{
			BuildingMesh->SetMaterial(1, BronzeMatFinder.Object);
		}
	}

	// 3. Selection Box on ground
	SelectionBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionBoxMesh"));
	SelectionBoxMesh->SetupAttachment(RootComponent);
	SelectionBoxMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 5.0f));
	SelectionBoxMesh->SetRelativeScale3D(FVector(36.0f, 36.0f, 0.1f));
	SelectionBoxMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SelectionBoxMesh->SetMobility(EComponentMobility::Movable);
	SelectionBoxMesh->SetVisibility(false);
	if (CubeMesh)
	{
		SelectionBoxMesh->SetStaticMesh(CubeMesh);
	}

	// 4. Overhead Billboard Label (Disabled for clean AAA fidelity)
	OverheadNameText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("OverheadNameText"));
	OverheadNameText->SetupAttachment(RootComponent);
	OverheadNameText->SetVisibility(false);
}

void ADominionBuildingActor::BeginPlay()
{
	Super::BeginPlay();

	if (OverheadNameText)
	{
		OverheadNameText->SetText(FText::FromString(BuildingName));
	}

	ApplyCurrentStyleVisuals();
}

void ADominionBuildingActor::SetArchitecturalStyle(EDominionArchitecturalStyle NewStyle)
{
	ArchitecturalStyle = NewStyle;
	ApplyCurrentStyleVisuals();
}

void ADominionBuildingActor::ApplyCurrentStyleVisuals()
{
	auto ApplyMaterial = [this](UStaticMeshComponent* Comp, const FLinearColor& BaseColor, float Metallic = 0.0f, float Roughness = 0.7f, const FLinearColor& EmissiveColor = FLinearColor::Black)
	{
		if (Comp && Comp->GetStaticMesh() && Comp->GetNumMaterials() > 0)
		{
			UMaterialInterface* ExistingMat = Comp->GetMaterial(0);
			if (ExistingMat && ExistingMat->GetName().Contains(TEXT("M_Citadel")))
			{
				return; // Preserve custom textured PBR Citadel material
			}
			UMaterialInstanceDynamic* DynMat = UDominionTextureFactory::CreateDominionMaterial(this, BaseColor, Metallic, Roughness, EmissiveColor);
			if (DynMat)
			{
				Comp->SetMaterial(0, DynMat);
			}
		}
	};

	const FLinearColor GoldSelection(1.0f, 0.84f, 0.0f);

	if (ArchitecturalStyle == EDominionArchitecturalStyle::PrimitiveEarthAndMud || ArchitecturalStyle == EDominionArchitecturalStyle::AngkorWatHewnStone)
	{
		// Mythic Grimdark Mesopotamian Alluvial Architecture (PBR Standards)
		const FLinearColor MesopotamianMudbrick(0.48f, 0.40f, 0.32f); // #7A6652 Dry clay silt
		ApplyMaterial(BuildingMesh, MesopotamianMudbrick, 0.0f, 0.90f);
	}
	else if (ArchitecturalStyle == EDominionArchitecturalStyle::DarkEvilGothic)
	{
		// Era 3: Dark Evil Gothic Fantasy
		const FLinearColor ObsidianBlack(0.08f, 0.08f, 0.10f);
		ApplyMaterial(BuildingMesh, ObsidianBlack, 0.25f, 0.45f);
	}
	else
	{
		// Era 4: Victorian Industrial Grimdark
		const FLinearColor SootBrick(0.22f, 0.16f, 0.14f);
		ApplyMaterial(BuildingMesh, SootBrick, 0.05f, 0.85f);
	}

	ApplyMaterial(SelectionBoxMesh, GoldSelection, 0.5f, 0.3f);
}

void ADominionBuildingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADominionBuildingActor::SetSelected(bool bNewSelected)
{
	bIsSelected = bNewSelected;
	if (SelectionBoxMesh)
	{
		SelectionBoxMesh->SetVisibility(bNewSelected);
	}
}
