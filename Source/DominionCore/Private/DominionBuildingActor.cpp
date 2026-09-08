#include "DominionBuildingActor.h"
#include "DominionTextureFactory.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

ADominionBuildingActor::ADominionBuildingActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.03f; // ~30 Hz animation tick

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeFinder(TEXT("/Engine/BasicShapes/Cone.Cone"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	UStaticMesh* CubeMesh = CubeFinder.Succeeded() ? CubeFinder.Object : nullptr;
	UStaticMesh* ConeMesh = ConeFinder.Succeeded() ? ConeFinder.Object : nullptr;
	UStaticMesh* CylinderMesh = CylinderFinder.Succeeded() ? CylinderFinder.Object : nullptr;
	UStaticMesh* SphereMesh = SphereFinder.Succeeded() ? SphereFinder.Object : nullptr;

	// Colossal Base Foundation
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
	RootComponent = BuildingMesh;
	BuildingMesh->SetCollisionProfileName(TEXT("BlockAll"));
	if (CubeMesh)
	{
		BuildingMesh->SetStaticMesh(CubeMesh);
		BuildingMesh->SetRelativeScale3D(FVector(12.0f, 12.0f, 4.0f)); // Colossal 120m base
	}

	// Tier 2 (Middle Colossal Terrace / Grand Nave)
	Tier2Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tier2Mesh"));
	Tier2Mesh->SetupAttachment(RootComponent);
	Tier2Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	Tier2Mesh->SetRelativeScale3D(FVector(0.75f, 0.75f, 1.2f));
	Tier2Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh)
	{
		Tier2Mesh->SetStaticMesh(CubeMesh);
	}

	// Tier 3 Summit / High Sanctuary Keep
	TempleShrineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TempleShrineMesh"));
	TempleShrineMesh->SetupAttachment(RootComponent);
	TempleShrineMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 320.0f));
	TempleShrineMesh->SetRelativeScale3D(FVector(0.48f, 0.48f, 1.4f));
	TempleShrineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh)
	{
		TempleShrineMesh->SetStaticMesh(CubeMesh);
	}

	// Shrine Peak / Central Grand Spire
	ShrineRoofMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShrineRoofMesh"));
	ShrineRoofMesh->SetupAttachment(TempleShrineMesh);
	ShrineRoofMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
	ShrineRoofMesh->SetRelativeScale3D(FVector(1.1f, 1.1f, 1.6f));
	ShrineRoofMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (ConeMesh)
	{
		ShrineRoofMesh->SetStaticMesh(ConeMesh);
	}

	// Soaring Flanking Spires (Gothic Needles / Angkor Wat Prang Towers / Chimneys)
	TowerLeftSpire = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerLeftSpire"));
	TowerLeftSpire->SetupAttachment(RootComponent);
	TowerLeftSpire->SetRelativeLocation(FVector(0.0f, -480.0f, 220.0f));
	TowerLeftSpire->SetRelativeScale3D(FVector(0.35f, 0.35f, 4.2f)); // Soaring 100m spire
	TowerLeftSpire->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (ConeMesh)
	{
		TowerLeftSpire->SetStaticMesh(ConeMesh);
	}

	TowerRightSpire = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerRightSpire"));
	TowerRightSpire->SetupAttachment(RootComponent);
	TowerRightSpire->SetRelativeLocation(FVector(0.0f, 480.0f, 220.0f));
	TowerRightSpire->SetRelativeScale3D(FVector(0.35f, 0.35f, 4.2f));
	TowerRightSpire->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (ConeMesh)
	{
		TowerRightSpire->SetStaticMesh(ConeMesh);
	}

	// Flying Buttresses / Stone Balustrades
	FlyingButtressLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlyingButtressLeft"));
	FlyingButtressLeft->SetupAttachment(RootComponent);
	FlyingButtressLeft->SetRelativeLocation(FVector(0.0f, -320.0f, 140.0f));
	FlyingButtressLeft->SetRelativeRotation(FRotator(0.0f, 0.0f, -35.0f));
	FlyingButtressLeft->SetRelativeScale3D(FVector(0.15f, 1.6f, 0.25f));
	FlyingButtressLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh)
	{
		FlyingButtressLeft->SetStaticMesh(CubeMesh);
	}

	FlyingButtressRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlyingButtressRight"));
	FlyingButtressRight->SetupAttachment(RootComponent);
	FlyingButtressRight->SetRelativeLocation(FVector(0.0f, 320.0f, 140.0f));
	FlyingButtressRight->SetRelativeRotation(FRotator(0.0f, 0.0f, 35.0f));
	FlyingButtressRight->SetRelativeScale3D(FVector(0.15f, 1.6f, 0.25f));
	FlyingButtressRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh)
	{
		FlyingButtressRight->SetStaticMesh(CubeMesh);
	}

	// Evil Spiky Ridges / Defensive Palisades
	SpikeRidgeMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpikeRidgeMesh1"));
	SpikeRidgeMesh1->SetupAttachment(RootComponent);
	SpikeRidgeMesh1->SetRelativeLocation(FVector(520.0f, -180.0f, 160.0f));
	SpikeRidgeMesh1->SetRelativeScale3D(FVector(0.18f, 0.18f, 1.2f));
	SpikeRidgeMesh1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (ConeMesh)
	{
		SpikeRidgeMesh1->SetStaticMesh(ConeMesh);
	}

	SpikeRidgeMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpikeRidgeMesh2"));
	SpikeRidgeMesh2->SetupAttachment(RootComponent);
	SpikeRidgeMesh2->SetRelativeLocation(FVector(520.0f, 180.0f, 160.0f));
	SpikeRidgeMesh2->SetRelativeScale3D(FVector(0.18f, 0.18f, 1.2f));
	SpikeRidgeMesh2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (ConeMesh)
	{
		SpikeRidgeMesh2->SetStaticMesh(ConeMesh);
	}

	// Central Grand Approach Ramp
	RampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RampMesh"));
	RampMesh->SetupAttachment(RootComponent);
	RampMesh->SetRelativeLocation(FVector(580.0f, 0.0f, -30.0f));
	RampMesh->SetRelativeRotation(FRotator(-26.0f, 0.0f, 0.0f));
	RampMesh->SetRelativeScale3D(FVector(5.5f, 1.4f, 0.35f));
	RampMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh)
	{
		RampMesh->SetStaticMesh(CubeMesh);
	}

	// Flanking Grand Stairways
	LeftRampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftRampMesh"));
	LeftRampMesh->SetupAttachment(RootComponent);
	LeftRampMesh->SetRelativeLocation(FVector(200.0f, -420.0f, 0.0f));
	LeftRampMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, -24.0f));
	LeftRampMesh->SetRelativeScale3D(FVector(3.8f, 0.8f, 0.25f));
	LeftRampMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh)
	{
		LeftRampMesh->SetStaticMesh(CubeMesh);
	}

	RightRampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightRampMesh"));
	RightRampMesh->SetupAttachment(RootComponent);
	RightRampMesh->SetRelativeLocation(FVector(200.0f, 420.0f, 0.0f));
	RightRampMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, -24.0f));
	RightRampMesh->SetRelativeScale3D(FVector(3.8f, 0.8f, 0.25f));
	RightRampMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh)
	{
		RightRampMesh->SetStaticMesh(CubeMesh);
	}

	// Monumental Gate Archway
	GateArchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateArchMesh"));
	GateArchMesh->SetupAttachment(RootComponent);
	GateArchMesh->SetRelativeLocation(FVector(320.0f, 0.0f, 140.0f));
	GateArchMesh->SetRelativeScale3D(FVector(0.6f, 1.8f, 1.2f));
	GateArchMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh)
	{
		GateArchMesh->SetStaticMesh(CubeMesh);
	}

	// Royal Tapestry Banners (Left & Right)
	BannerLeftMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BannerLeftMesh"));
	BannerLeftMesh->SetupAttachment(RootComponent);
	BannerLeftMesh->SetRelativeLocation(FVector(340.0f, -160.0f, 110.0f));
	BannerLeftMesh->SetRelativeScale3D(FVector(0.08f, 0.5f, 1.8f));
	BannerLeftMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh)
	{
		BannerLeftMesh->SetStaticMesh(CubeMesh);
	}

	BannerRightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BannerRightMesh"));
	BannerRightMesh->SetupAttachment(RootComponent);
	BannerRightMesh->SetRelativeLocation(FVector(340.0f, 160.0f, 110.0f));
	BannerRightMesh->SetRelativeScale3D(FVector(0.08f, 0.5f, 1.8f));
	BannerRightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh)
	{
		BannerRightMesh->SetStaticMesh(CubeMesh);
	}

	// Colossal Brazier Basin
	BrazierMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BrazierMesh"));
	BrazierMesh->SetupAttachment(RootComponent);
	BrazierMesh->SetRelativeLocation(FVector(400.0f, 0.0f, 80.0f));
	BrazierMesh->SetRelativeScale3D(FVector(0.8f, 0.8f, 1.0f));
	BrazierMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CylinderMesh)
	{
		BrazierMesh->SetStaticMesh(CylinderMesh);
	}

	// Dynamic Flame Point Light (Towering Illumination)
	BrazierLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("BrazierLight"));
	BrazierLight->SetupAttachment(BrazierMesh);
	BrazierLight->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
	BrazierLight->SetLightColor(FLinearColor(1.0f, 0.45f, 0.12f));
	BrazierLight->SetIntensity(14000.0f); // Obscene beacon light
	BrazierLight->SetAttenuationRadius(3500.0f);
	BrazierLight->SetCastShadows(false);

	// Selection Box on ground
	SelectionBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionBoxMesh"));
	SelectionBoxMesh->SetupAttachment(RootComponent);
	SelectionBoxMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -190.0f));
	SelectionBoxMesh->SetRelativeScale3D(FVector(2.4f, 2.4f, 0.06f));
	SelectionBoxMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SelectionBoxMesh->SetVisibility(false);
	if (CubeMesh)
	{
		SelectionBoxMesh->SetStaticMesh(CubeMesh);
	}

	// Overhead Billboard Label
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
		if (Comp)
		{
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
		// Mythic Grimdark Bronze Age Citadel & Ziggurat (Matching Visual Target Mockup)
		const FLinearColor WeatheredMudbrick(0.24f, 0.18f, 0.14f);
		const FLinearColor DarkHewnStone(0.18f, 0.15f, 0.12f);
		const FLinearColor VerdigrisCopperRoof(0.18f, 0.38f, 0.32f); // Oxidized green bronze/copper temple roof
		const FLinearColor RawCedarTimber(0.15f, 0.10f, 0.06f);
		const FLinearColor DarkBronze(0.55f, 0.38f, 0.16f);
		const FLinearColor FireEmissive(35.0f, 10.0f, 1.0f);

		TowerLeftSpire->SetVisibility(true);
		TowerRightSpire->SetVisibility(true);
		FlyingButtressLeft->SetVisibility(true);
		FlyingButtressRight->SetVisibility(true);
		SpikeRidgeMesh1->SetVisibility(false);
		SpikeRidgeMesh2->SetVisibility(false);

		ApplyMaterial(BuildingMesh, WeatheredMudbrick, 0.0f, 0.88f);
		ApplyMaterial(Tier2Mesh, WeatheredMudbrick, 0.0f, 0.85f);
		ApplyMaterial(TempleShrineMesh, WeatheredMudbrick, 0.0f, 0.82f);
		ApplyMaterial(ShrineRoofMesh, VerdigrisCopperRoof, 0.75f, 0.32f); // Shining verdigris temple roof
		ApplyMaterial(TowerLeftSpire, DarkHewnStone, 0.0f, 0.85f);
		ApplyMaterial(TowerRightSpire, DarkHewnStone, 0.0f, 0.85f);
		ApplyMaterial(FlyingButtressLeft, DarkHewnStone, 0.0f, 0.85f);
		ApplyMaterial(FlyingButtressRight, DarkHewnStone, 0.0f, 0.85f);
		ApplyMaterial(RampMesh, DarkHewnStone, 0.0f, 0.80f);
		ApplyMaterial(LeftRampMesh, DarkHewnStone, 0.0f, 0.82f);
		ApplyMaterial(RightRampMesh, DarkHewnStone, 0.0f, 0.82f);
		ApplyMaterial(GateArchMesh, WeatheredMudbrick, 0.0f, 0.85f);
		ApplyMaterial(BannerLeftMesh, DarkBronze, 0.85f, 0.25f);
		ApplyMaterial(BannerRightMesh, DarkBronze, 0.85f, 0.25f);
		ApplyMaterial(BrazierMesh, DarkHewnStone, 0.1f, 0.75f, FireEmissive);

		if (BrazierLight)
		{
			BrazierLight->SetLightColor(FLinearColor(1.0f, 0.45f, 0.10f));
			BrazierLight->SetIntensity(18000.0f);
			BrazierLight->SetAttenuationRadius(2800.0f);
		}
	}
	else if (ArchitecturalStyle == EDominionArchitecturalStyle::DarkEvilGothic)
	{
		// Era 3: Dark Evil Gothic Fantasy (Elden Ring / Dark Souls Anor Londo / Harrenhal)
		const FLinearColor ObsidianBlack(0.08f, 0.08f, 0.10f);
		const FLinearColor BloodCrimson(0.65f, 0.04f, 0.08f);
		const FLinearColor SpikyIron(0.18f, 0.18f, 0.20f);
		const FLinearColor PaleMarbleGargoyle(0.24f, 0.24f, 0.28f);

		TowerLeftSpire->SetVisibility(true);
		TowerRightSpire->SetVisibility(true);
		FlyingButtressLeft->SetVisibility(true);
		FlyingButtressRight->SetVisibility(true);
		SpikeRidgeMesh1->SetVisibility(true); // Evil spiky gargoyle finials
		SpikeRidgeMesh2->SetVisibility(true);

		ApplyMaterial(BuildingMesh, ObsidianBlack, 0.25f, 0.45f);
		ApplyMaterial(Tier2Mesh, ObsidianBlack, 0.30f, 0.40f);
		ApplyMaterial(TempleShrineMesh, ObsidianBlack, 0.35f, 0.35f);
		ApplyMaterial(ShrineRoofMesh, SpikyIron, 0.95f, 0.20f);
		ApplyMaterial(TowerLeftSpire, SpikyIron, 0.95f, 0.18f);
		ApplyMaterial(TowerRightSpire, SpikyIron, 0.95f, 0.18f);
		ApplyMaterial(FlyingButtressLeft, ObsidianBlack, 0.20f, 0.50f);
		ApplyMaterial(FlyingButtressRight, ObsidianBlack, 0.20f, 0.50f);
		ApplyMaterial(SpikeRidgeMesh1, SpikyIron, 0.95f, 0.15f);
		ApplyMaterial(SpikeRidgeMesh2, SpikyIron, 0.95f, 0.15f);
		ApplyMaterial(RampMesh, ObsidianBlack, 0.20f, 0.55f);
		ApplyMaterial(LeftRampMesh, PaleMarbleGargoyle, 0.10f, 0.60f);
		ApplyMaterial(RightRampMesh, PaleMarbleGargoyle, 0.10f, 0.60f);
		ApplyMaterial(GateArchMesh, SpikyIron, 0.90f, 0.25f);
		ApplyMaterial(BannerLeftMesh, BloodCrimson, 0.0f, 0.6f);
		ApplyMaterial(BannerRightMesh, BloodCrimson, 0.0f, 0.6f);
		ApplyMaterial(BrazierMesh, SpikyIron, 0.95f, 0.20f);

		if (BrazierLight)
		{
			BrazierLight->SetLightColor(FLinearColor(1.0f, 0.20f, 0.08f)); // Sinister evil crimson-orange fire
			BrazierLight->SetIntensity(22000.0f);
		}
	}
	else
	{
		// Era 4: Victorian Industrial Grimdark (Smokestacks, Cast-Iron Plating, Steam Valves)
		const FLinearColor SootBrick(0.22f, 0.16f, 0.14f);
		const FLinearColor CastIron(0.14f, 0.15f, 0.16f);
		const FLinearColor IndustrialBrass(0.85f, 0.62f, 0.18f);
		const FLinearColor FurnaceEmber(1.0f, 0.50f, 0.05f);

		TowerLeftSpire->SetVisibility(true); // Smokestack Left
		TowerRightSpire->SetVisibility(true); // Smokestack Right
		FlyingButtressLeft->SetVisibility(true); // Overhead pipe conduit
		FlyingButtressRight->SetVisibility(true);
		SpikeRidgeMesh1->SetVisibility(true);
		SpikeRidgeMesh2->SetVisibility(true);

		ApplyMaterial(BuildingMesh, SootBrick, 0.05f, 0.85f);
		ApplyMaterial(Tier2Mesh, CastIron, 0.92f, 0.30f);
		ApplyMaterial(TempleShrineMesh, CastIron, 0.95f, 0.25f);
		ApplyMaterial(ShrineRoofMesh, IndustrialBrass, 0.96f, 0.22f);
		ApplyMaterial(TowerLeftSpire, CastIron, 0.90f, 0.35f);
		ApplyMaterial(TowerRightSpire, CastIron, 0.90f, 0.35f);
		ApplyMaterial(FlyingButtressLeft, IndustrialBrass, 0.95f, 0.20f);
		ApplyMaterial(FlyingButtressRight, IndustrialBrass, 0.95f, 0.20f);
		ApplyMaterial(SpikeRidgeMesh1, CastIron, 0.92f, 0.30f);
		ApplyMaterial(SpikeRidgeMesh2, CastIron, 0.92f, 0.30f);
		ApplyMaterial(RampMesh, SootBrick, 0.05f, 0.85f);
		ApplyMaterial(LeftRampMesh, CastIron, 0.90f, 0.32f);
		ApplyMaterial(RightRampMesh, CastIron, 0.90f, 0.32f);
		ApplyMaterial(GateArchMesh, IndustrialBrass, 0.96f, 0.20f);
		ApplyMaterial(BannerLeftMesh, IndustrialBrass, 0.85f, 0.30f);
		ApplyMaterial(BannerRightMesh, IndustrialBrass, 0.85f, 0.30f);
		ApplyMaterial(BrazierMesh, CastIron, 0.95f, 0.25f);

		if (BrazierLight)
		{
			BrazierLight->SetLightColor(FLinearColor(1.0f, 0.65f, 0.20f)); // White-hot furnace blast
			BrazierLight->SetIntensity(25000.0f);
		}
	}

	ApplyMaterial(SelectionBoxMesh, GoldSelection, 0.5f, 0.3f);
}

void ADominionBuildingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Dynamic Flame Flicker on Brazier Light
	if (BrazierLight && BrazierLight->IsVisible())
	{
		const float Time = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
		const float Flicker = FMath::Sin(Time * 14.0f) * 0.18f + FMath::Cos(Time * 27.0f) * 0.12f;
		const float BaseIntensity = (ArchitecturalStyle == EDominionArchitecturalStyle::DarkEvilGothic) ? 22000.0f :
		                            (ArchitecturalStyle == EDominionArchitecturalStyle::VictorianIndustrial) ? 25000.0f : 12000.0f;
		BrazierLight->SetIntensity(BaseIntensity * (1.0f + Flicker));
	}
}

void ADominionBuildingActor::SetSelected(bool bNewSelected)
{
	bIsSelected = bNewSelected;
	if (SelectionBoxMesh)
	{
		SelectionBoxMesh->SetVisibility(bNewSelected);
	}
}


