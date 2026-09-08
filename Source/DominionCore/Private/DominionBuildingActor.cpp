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
	static UTexture2D* MudbrickAlbedo = UDominionTextureFactory::CreateMudbrickAlbedo(512, 512);
	static UTexture2D* MudbrickNormal = UDominionTextureFactory::CreateMudbrickNormal(512, 512);
	static UTexture2D* SandstoneAlbedo = UDominionTextureFactory::CreateSandstoneSiltAlbedo(512, 512);
	static UTexture2D* SandstoneNormal = UDominionTextureFactory::CreateSandstoneSiltNormal(512, 512);
	static UTexture2D* BronzeAlbedo = UDominionTextureFactory::CreateHammeredBronzeAlbedo(512, 512);
	static UTexture2D* BronzeNormal = UDominionTextureFactory::CreateHammeredBronzeNormal(512, 512);

	auto ApplyDynMaterial = [](UStaticMeshComponent* Comp, const FLinearColor& Color, float Metallic = 0.0f, float Roughness = 0.7f, UTexture2D* AlbedoMap = nullptr, UTexture2D* NormalMap = nullptr)
	{
		if (Comp)
		{
			UMaterialInstanceDynamic* DynMat = Comp->CreateAndSetMaterialInstanceDynamic(0);
			if (DynMat)
			{
				DynMat->SetVectorParameterValue(TEXT("BaseColor"), Color);
				DynMat->SetVectorParameterValue(TEXT("Color"), Color);
				DynMat->SetScalarParameterValue(TEXT("Metallic"), Metallic);
				DynMat->SetScalarParameterValue(TEXT("Roughness"), Roughness);
				if (AlbedoMap)
				{
					DynMat->SetTextureParameterValue(TEXT("BaseColorMap"), AlbedoMap);
					DynMat->SetTextureParameterValue(TEXT("AlbedoMap"), AlbedoMap);
					DynMat->SetTextureParameterValue(TEXT("Texture"), AlbedoMap);
				}
				if (NormalMap)
				{
					DynMat->SetTextureParameterValue(TEXT("NormalMap"), NormalMap);
					DynMat->SetTextureParameterValue(TEXT("Normal"), NormalMap);
				}
			}
		}
	};

	const FLinearColor GoldSelection(1.0f, 0.84f, 0.0f);

	if (ArchitecturalStyle == EDominionArchitecturalStyle::PrimitiveEarthAndMud)
	{
		// Era 1: Primitive Mud & Megalithic Earthwork
		const FLinearColor MudClay(0.72f, 0.54f, 0.36f);
		const FLinearColor RawTimber(0.36f, 0.24f, 0.14f);
		const FLinearColor RoughStone(0.55f, 0.50f, 0.44f);
		const FLinearColor StrawHide(0.78f, 0.68f, 0.48f);

		TowerLeftSpire->SetVisibility(false);
		TowerRightSpire->SetVisibility(false);
		FlyingButtressLeft->SetVisibility(false);
		FlyingButtressRight->SetVisibility(false);
		SpikeRidgeMesh1->SetVisibility(true); // Timber palisade spikes
		SpikeRidgeMesh2->SetVisibility(true);

		ApplyDynMaterial(BuildingMesh, MudClay, 0.0f, 0.90f, MudbrickAlbedo, MudbrickNormal);
		ApplyDynMaterial(Tier2Mesh, RoughStone, 0.0f, 0.85f, MudbrickAlbedo, MudbrickNormal);
		ApplyDynMaterial(TempleShrineMesh, MudClay, 0.0f, 0.90f, MudbrickAlbedo, MudbrickNormal);
		ApplyDynMaterial(ShrineRoofMesh, StrawHide, 0.0f, 0.95f, SandstoneAlbedo, SandstoneNormal);
		ApplyDynMaterial(RampMesh, MudClay, 0.0f, 0.90f, MudbrickAlbedo, MudbrickNormal);
		ApplyDynMaterial(LeftRampMesh, RoughStone, 0.0f, 0.88f, MudbrickAlbedo, MudbrickNormal);
		ApplyDynMaterial(RightRampMesh, RoughStone, 0.0f, 0.88f, MudbrickAlbedo, MudbrickNormal);
		ApplyDynMaterial(GateArchMesh, RawTimber, 0.0f, 0.80f);
		ApplyDynMaterial(SpikeRidgeMesh1, RawTimber, 0.0f, 0.85f);
		ApplyDynMaterial(SpikeRidgeMesh2, RawTimber, 0.0f, 0.85f);
		ApplyDynMaterial(BannerLeftMesh, StrawHide, 0.0f, 0.9f);
		ApplyDynMaterial(BannerRightMesh, StrawHide, 0.0f, 0.9f);
		ApplyDynMaterial(BrazierMesh, RoughStone, 0.0f, 0.85f, BronzeAlbedo, BronzeNormal);

		if (BrazierLight)
		{
			BrazierLight->SetLightColor(FLinearColor(1.0f, 0.45f, 0.10f));
			BrazierLight->SetIntensity(10000.0f);
		}
	}
	else if (ArchitecturalStyle == EDominionArchitecturalStyle::AngkorWatHewnStone)
	{
		// Era 2: Angkor Wat Hewn Sandstone & Lotus-Bud Prang Spires
		const FLinearColor WeatheredSandstone(0.68f, 0.60f, 0.48f);
		const FLinearColor MossyGreenStone(0.48f, 0.52f, 0.40f);
		const FLinearColor SacredGold(0.95f, 0.78f, 0.22f);
		const FLinearColor DarkBasalt(0.32f, 0.30f, 0.28f);

		TowerLeftSpire->SetVisibility(true);
		TowerRightSpire->SetVisibility(true);
		FlyingButtressLeft->SetVisibility(true); // Carved stone balustrades
		FlyingButtressRight->SetVisibility(true);
		SpikeRidgeMesh1->SetVisibility(false);
		SpikeRidgeMesh2->SetVisibility(false);

		ApplyDynMaterial(BuildingMesh, WeatheredSandstone, 0.0f, 0.78f);
		ApplyDynMaterial(Tier2Mesh, MossyGreenStone, 0.0f, 0.80f);
		ApplyDynMaterial(TempleShrineMesh, WeatheredSandstone, 0.0f, 0.75f);
		ApplyDynMaterial(ShrineRoofMesh, SacredGold, 0.7f, 0.35f);
		ApplyDynMaterial(TowerLeftSpire, WeatheredSandstone, 0.0f, 0.75f);
		ApplyDynMaterial(TowerRightSpire, WeatheredSandstone, 0.0f, 0.75f);
		ApplyDynMaterial(FlyingButtressLeft, MossyGreenStone, 0.0f, 0.80f);
		ApplyDynMaterial(FlyingButtressRight, MossyGreenStone, 0.0f, 0.80f);
		ApplyDynMaterial(RampMesh, WeatheredSandstone, 0.0f, 0.78f);
		ApplyDynMaterial(LeftRampMesh, DarkBasalt, 0.0f, 0.82f);
		ApplyDynMaterial(RightRampMesh, DarkBasalt, 0.0f, 0.82f);
		ApplyDynMaterial(GateArchMesh, WeatheredSandstone, 0.0f, 0.75f);
		ApplyDynMaterial(BannerLeftMesh, SacredGold, 0.5f, 0.4f);
		ApplyDynMaterial(BannerRightMesh, SacredGold, 0.5f, 0.4f);
		ApplyDynMaterial(BrazierMesh, DarkBasalt, 0.1f, 0.75f);

		if (BrazierLight)
		{
			BrazierLight->SetLightColor(FLinearColor(1.0f, 0.72f, 0.25f));
			BrazierLight->SetIntensity(15000.0f);
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

		ApplyDynMaterial(BuildingMesh, ObsidianBlack, 0.25f, 0.45f);
		ApplyDynMaterial(Tier2Mesh, ObsidianBlack, 0.30f, 0.40f);
		ApplyDynMaterial(TempleShrineMesh, ObsidianBlack, 0.35f, 0.35f);
		ApplyDynMaterial(ShrineRoofMesh, SpikyIron, 0.95f, 0.20f);
		ApplyDynMaterial(TowerLeftSpire, SpikyIron, 0.95f, 0.18f);
		ApplyDynMaterial(TowerRightSpire, SpikyIron, 0.95f, 0.18f);
		ApplyDynMaterial(FlyingButtressLeft, ObsidianBlack, 0.20f, 0.50f);
		ApplyDynMaterial(FlyingButtressRight, ObsidianBlack, 0.20f, 0.50f);
		ApplyDynMaterial(SpikeRidgeMesh1, SpikyIron, 0.95f, 0.15f);
		ApplyDynMaterial(SpikeRidgeMesh2, SpikyIron, 0.95f, 0.15f);
		ApplyDynMaterial(RampMesh, ObsidianBlack, 0.20f, 0.55f);
		ApplyDynMaterial(LeftRampMesh, PaleMarbleGargoyle, 0.10f, 0.60f);
		ApplyDynMaterial(RightRampMesh, PaleMarbleGargoyle, 0.10f, 0.60f);
		ApplyDynMaterial(GateArchMesh, SpikyIron, 0.90f, 0.25f);
		ApplyDynMaterial(BannerLeftMesh, BloodCrimson, 0.0f, 0.6f);
		ApplyDynMaterial(BannerRightMesh, BloodCrimson, 0.0f, 0.6f);
		ApplyDynMaterial(BrazierMesh, SpikyIron, 0.95f, 0.20f);

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

		ApplyDynMaterial(BuildingMesh, SootBrick, 0.05f, 0.85f);
		ApplyDynMaterial(Tier2Mesh, CastIron, 0.92f, 0.30f);
		ApplyDynMaterial(TempleShrineMesh, CastIron, 0.95f, 0.25f);
		ApplyDynMaterial(ShrineRoofMesh, IndustrialBrass, 0.96f, 0.22f);
		ApplyDynMaterial(TowerLeftSpire, CastIron, 0.90f, 0.35f);
		ApplyDynMaterial(TowerRightSpire, CastIron, 0.90f, 0.35f);
		ApplyDynMaterial(FlyingButtressLeft, IndustrialBrass, 0.95f, 0.20f);
		ApplyDynMaterial(FlyingButtressRight, IndustrialBrass, 0.95f, 0.20f);
		ApplyDynMaterial(SpikeRidgeMesh1, CastIron, 0.92f, 0.30f);
		ApplyDynMaterial(SpikeRidgeMesh2, CastIron, 0.92f, 0.30f);
		ApplyDynMaterial(RampMesh, SootBrick, 0.05f, 0.85f);
		ApplyDynMaterial(LeftRampMesh, CastIron, 0.90f, 0.32f);
		ApplyDynMaterial(RightRampMesh, CastIron, 0.90f, 0.32f);
		ApplyDynMaterial(GateArchMesh, IndustrialBrass, 0.96f, 0.20f);
		ApplyDynMaterial(BannerLeftMesh, IndustrialBrass, 0.85f, 0.30f);
		ApplyDynMaterial(BannerRightMesh, IndustrialBrass, 0.85f, 0.30f);
		ApplyDynMaterial(BrazierMesh, CastIron, 0.95f, 0.25f);

		if (BrazierLight)
		{
			BrazierLight->SetLightColor(FLinearColor(1.0f, 0.65f, 0.20f)); // White-hot furnace blast
			BrazierLight->SetIntensity(25000.0f);
		}
	}

	ApplyDynMaterial(SelectionBoxMesh, GoldSelection, 0.5f, 0.3f);
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


