#include "DominionGameModeBase.h"
#include "DominionRTSPawn.h"
#include "DominionRTSPlayerController.h"
#include "DominionRTSHUD.h"
#include "DominionUnitActor.h"
#include "DominionBuildingActor.h"
#include "DominionEnvironmentVolume.h"
#include "DominionTextureFactory.h"
#include "Engine/StaticMeshActor.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ADominionGameModeBase::ADominionGameModeBase()
{
	DefaultPawnClass = ADominionRTSPawn::StaticClass();
	PlayerControllerClass = ADominionRTSPlayerController::StaticClass();
	HUDClass = ADominionRTSHUD::StaticClass();
}

void ADominionGameModeBase::StartPlay()
{
	Super::StartPlay();

	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Automatically destroy all default template actors that cause white out and flat lighting
	TArray<AActor*> LevelActors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), LevelActors);
	for (AActor* Act : LevelActors)
	{
		if (Act && Act != this)
		{
			FString ActorName = Act->GetName();
			if (ActorName.Contains(TEXT("SM_SkySphere")) || ActorName.Contains(TEXT("TopDownCharacter")) ||
			    ActorName.Contains(TEXT("BP_TopDown")) || ActorName.Contains(TEXT("SM_Ramp")) ||
			    ActorName.Contains(TEXT("SM_QuarterCylinder")))
			{
				Act->Destroy();
			}
		}
	}

	// Ensure PlayerController has possessed an ADominionRTSPawn
	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (PC)
	{
		ADominionRTSPawn* RTSPawn = Cast<ADominionRTSPawn>(PC->GetPawn());
		if (!RTSPawn)
		{
			RTSPawn = World->SpawnActor<ADominionRTSPawn>(ADominionRTSPawn::StaticClass(), FVector(-1800.0f, 0.0f, 1400.0f), FRotator::ZeroRotator, SpawnParams);
			if (RTSPawn)
			{
				PC->Possess(RTSPawn);
			}
		}
	}

	// --- 0. Spawn Atmospheric & Volumetric Lighting Environment Volume ---
	ADominionEnvironmentVolume* EnvVolume = World->SpawnActor<ADominionEnvironmentVolume>(
		ADominionEnvironmentVolume::StaticClass(),
		FVector(0.0f, 0.0f, 0.0f),
		FRotator::ZeroRotator,
		SpawnParams
	);

	// Load basic shape meshes for environmental terrain geometry
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	UStaticMesh* CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	UStaticMesh* ConeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cone.Cone"));
	UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
	UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	// Color Palette for Mythic Grimdark Bronze Age (Matching Mockup Image 2)
	const FLinearColor DarkFlagstoneGround(0.12f, 0.11f, 0.09f);
	const FLinearColor DarkMudSilt(0.16f, 0.13f, 0.10f);
	const FLinearColor DarkReflectiveRiver(0.06f, 0.10f, 0.16f);
	const FLinearColor WeatheredDarkMudbrick(0.22f, 0.17f, 0.13f);
	const FLinearColor DarkHewnStone(0.18f, 0.15f, 0.12f);
	const FLinearColor DarkWoodTimber(0.14f, 0.09f, 0.05f);
	const FLinearColor ForgeKilnBrick(0.19f, 0.14f, 0.11f);
	const FLinearColor FieryHearthEmissive(65.0f, 22.0f, 2.0f);
	const FLinearColor TorchFlameEmissive(40.0f, 14.0f, 1.5f);

	// Helper to spawn static mesh actor with M_DominionMaster PBR material
	auto SpawnProp = [World, &SpawnParams](UStaticMesh* Mesh, const FVector& Loc, const FRotator& Rot, const FVector& Scale, const FLinearColor& BaseColor, float Metallic = 0.0f, float Roughness = 0.7f, const FLinearColor& EmissiveColor = FLinearColor::Black) -> AStaticMeshActor*
	{
		if (!Mesh || !World) return nullptr;
		AStaticMeshActor* Prop = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Loc, Rot, SpawnParams);
		if (Prop && Prop->GetStaticMeshComponent())
		{
			Prop->GetStaticMeshComponent()->SetStaticMesh(Mesh);
			Prop->SetActorScale3D(Scale);
			UMaterialInstanceDynamic* DynMat = UDominionTextureFactory::CreateDominionMaterial(Prop, BaseColor, Metallic, Roughness, EmissiveColor);
			if (DynMat)
			{
				Prop->GetStaticMeshComponent()->SetMaterial(0, DynMat);
			}
		}
		return Prop;
	};

	// --- 1. Vast Dark Wet Flagstone Citadel Ground ---
	if (PlaneMesh)
	{
		// Main Citadel Flagstone Plaza Floor
		SpawnProp(PlaneMesh, FVector(0.0f, 0.0f, 20.0f), FRotator::ZeroRotator, FVector(450.0f, 550.0f, 1.0f), DarkFlagstoneGround, 0.15f, 0.52f);

		// Shimmering Euphrates River Channel (Right Flank)
		AStaticMeshActor* River = SpawnProp(PlaneMesh, FVector(1900.0f, 0.0f, 24.0f), FRotator::ZeroRotator, FVector(220.0f, 700.0f, 1.0f), DarkReflectiveRiver, 0.25f, 0.04f);
		if (River && River->GetStaticMeshComponent())
		{
			River->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
		}

		// River Silt Bank
		SpawnProp(PlaneMesh, FVector(1350.0f, 0.0f, 22.0f), FRotator::ZeroRotator, FVector(60.0f, 600.0f, 1.0f), DarkMudSilt, 0.05f, 0.75f);
	}

	// --- 2. River Docks & Moored Wooden Barges (Right Flank) ---
	if (CubeMesh)
	{
		// Main Wooden Dock Pier
		SpawnProp(CubeMesh, FVector(1450.0f, -400.0f, 32.0f), FRotator::ZeroRotator, FVector(4.5f, 12.0f, 0.35f), DarkWoodTimber, 0.0f, 0.85f);
		SpawnProp(CubeMesh, FVector(1450.0f, 350.0f, 32.0f), FRotator::ZeroRotator, FVector(4.5f, 12.0f, 0.35f), DarkWoodTimber, 0.0f, 0.85f);

		// Moored Wooden River Barges / Boats
		const FVector BoatLocations[] = {
			FVector(1720.0f, -380.0f, 28.0f),
			FVector(1750.0f, 400.0f, 28.0f),
			FVector(1820.0f, -650.0f, 28.0f)
		};
		for (const FVector& BoatLoc : BoatLocations)
		{
			SpawnProp(CubeMesh, BoatLoc, FRotator(0.0f, 15.0f, 0.0f), FVector(3.2f, 1.2f, 0.6f), DarkWoodTimber, 0.0f, 0.80f);
		}
	}

	// --- 3. Bronze Smelting Forges & Glowing Beehive Kilns (Left Flank) ---
	if (SphereMesh && CylinderMesh && CubeMesh)
	{
		const FVector KilnLocations[] = {
			FVector(-600.0f, -1100.0f, 60.0f),
			FVector(-1100.0f, -1350.0f, 60.0f)
		};

		for (const FVector& KilnLoc : KilnLocations)
		{
			// Beehive Kiln Dome Body
			SpawnProp(SphereMesh, KilnLoc, FRotator::ZeroRotator, FVector(2.8f, 2.8f, 2.5f), ForgeKilnBrick, 0.05f, 0.88f);

			// Chimney Smoke Stack
			SpawnProp(CylinderMesh, KilnLoc + FVector(0.0f, 0.0f, 160.0f), FRotator::ZeroRotator, FVector(0.7f, 0.7f, 1.8f), ForgeKilnBrick, 0.05f, 0.90f);

			// Glowing Fiery Hearth Mouth (Open Fire Interior)
			SpawnProp(CubeMesh, KilnLoc + FVector(110.0f, 0.0f, -15.0f), FRotator::ZeroRotator, FVector(0.6f, 1.2f, 0.9f), FLinearColor(1.0f, 0.35f, 0.05f), 0.0f, 0.2f, FieryHearthEmissive);

			// Dynamic Radiant Fire Light from Forge Crucible
			AStaticMeshActor* LightAnchor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), KilnLoc + FVector(140.0f, 0.0f, 10.0f), FRotator::ZeroRotator, SpawnParams);
			if (LightAnchor)
			{
				UPointLightComponent* ForgeLight = NewObject<UPointLightComponent>(LightAnchor);
				ForgeLight->SetupAttachment(LightAnchor->GetRootComponent());
				ForgeLight->SetMobility(EComponentMobility::Movable);
				ForgeLight->SetLightColor(FLinearColor(1.0f, 0.48f, 0.12f));
				ForgeLight->SetIntensity(24000.0f);
				ForgeLight->SetAttenuationRadius(2500.0f);
				ForgeLight->SetCastShadows(true);
				ForgeLight->RegisterComponent();
			}

			// Blacksmith Anvil & Smelting Tables
			SpawnProp(CubeMesh, KilnLoc + FVector(180.0f, 80.0f, 0.0f), FRotator::ZeroRotator, FVector(0.8f, 1.4f, 0.6f), DarkWoodTimber, 0.0f, 0.80f);
			SpawnProp(CubeMesh, KilnLoc + FVector(180.0f, -80.0f, 0.0f), FRotator::ZeroRotator, FVector(0.6f, 0.6f, 0.8f), FLinearColor(0.25f, 0.22f, 0.20f), 0.95f, 0.30f); // Bronze anvil
		}
	}

	// --- 4. Fortification Bastion Walls & Flaming Perimeter Braziers ---
	if (CubeMesh && CylinderMesh)
	{
		const FVector WallLocations[] = {
			FVector(-2600.0f, 0.0f, 220.0f),    // Colossal Rear Keep Wall
			FVector(-1200.0f, -2000.0f, 220.0f), // Left Flank Wall
			FVector(-1200.0f, 2000.0f, 220.0f),  // Right Flank Wall
			FVector(1300.0f, -1200.0f, 180.0f),  // River Gate Bastion Left
			FVector(1300.0f, 1200.0f, 180.0f)   // River Gate Bastion Right
		};

		const FVector WallScales[] = {
			FVector(2.5f, 44.0f, 5.0f),
			FVector(32.0f, 2.5f, 5.0f),
			FVector(32.0f, 2.5f, 5.0f),
			FVector(2.0f, 16.0f, 4.0f),
			FVector(2.0f, 16.0f, 4.0f)
		};

		for (int32 w = 0; w < 5; ++w)
		{
			SpawnProp(CubeMesh, WallLocations[w], FRotator::ZeroRotator, WallScales[w], WeatheredDarkMudbrick, 0.0f, 0.88f);
		}

		// Monumental Corner Fortress Bastions (Towering 80m)
		const FVector BastionLocations[] = {
			FVector(-2600.0f, -2000.0f, 320.0f),
			FVector(-2600.0f, 2000.0f, 320.0f),
			FVector(600.0f, -2000.0f, 320.0f),
			FVector(600.0f, 2000.0f, 320.0f)
		};

		for (const FVector& BastionLoc : BastionLocations)
		{
			SpawnProp(CubeMesh, BastionLoc, FRotator::ZeroRotator, FVector(4.2f, 4.2f, 8.0f), DarkHewnStone, 0.0f, 0.85f);

			// Bastion Flame Brazier
			SpawnProp(CylinderMesh, BastionLoc + FVector(0.0f, 0.0f, 420.0f), FRotator::ZeroRotator, FVector(0.8f, 0.8f, 0.6f), DarkHewnStone, 0.1f, 0.7f, TorchFlameEmissive);

			AStaticMeshActor* BrazierLightActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), BastionLoc + FVector(0.0f, 0.0f, 460.0f), FRotator::ZeroRotator, SpawnParams);
			if (BrazierLightActor)
			{
				UPointLightComponent* BastionLight = NewObject<UPointLightComponent>(BrazierLightActor);
				BastionLight->SetupAttachment(BrazierLightActor->GetRootComponent());
				BastionLight->SetMobility(EComponentMobility::Movable);
				BastionLight->SetLightColor(FLinearColor(1.0f, 0.52f, 0.14f));
				BastionLight->SetIntensity(16000.0f);
				BastionLight->SetAttenuationRadius(2800.0f);
				BastionLight->SetCastShadows(false);
				BastionLight->RegisterComponent();
			}
		}
	}

	// --- 5. Spawn Colossal Stepped Ziggurat Citadel (Center-Rear) ---
	ADominionBuildingActor* Citadel = World->SpawnActor<ADominionBuildingActor>(
		ADominionBuildingActor::StaticClass(),
		FVector(-1200.0f, 0.0f, 80.0f),
		FRotator::ZeroRotator,
		SpawnParams
	);
	if (Citadel)
	{
		Citadel->BuildingName = TEXT("Grand Stepped Ziggurat of Dominion");
		Citadel->TeamID = 0;
		Citadel->SetArchitecturalStyle(EDominionArchitecturalStyle::AngkorWatHewnStone);
	}

	// Granary Vault
	ADominionBuildingActor* Granary = World->SpawnActor<ADominionBuildingActor>(
		ADominionBuildingActor::StaticClass(),
		FVector(-1900.0f, 900.0f, 80.0f),
		FRotator::ZeroRotator,
		SpawnParams
	);
	if (Granary)
	{
		Granary->BuildingName = TEXT("Megalithic Granary Vault");
		Granary->BuildingType = EDominionBuildingType::MudbrickGranary;
		Granary->TeamID = 0;
		Granary->SetArchitecturalStyle(EDominionArchitecturalStyle::PrimitiveEarthAndMud);
	}

	// War Forge & Barracks
	ADominionBuildingActor* Barracks = World->SpawnActor<ADominionBuildingActor>(
		ADominionBuildingActor::StaticClass(),
		FVector(-1900.0f, -900.0f, 80.0f),
		FRotator::ZeroRotator,
		SpawnParams
	);
	if (Barracks)
	{
		Barracks->BuildingName = TEXT("Colossal Bronze War Forge");
		Barracks->BuildingType = EDominionBuildingType::BronzeBarracks;
		Barracks->TeamID = 0;
		Barracks->SetArchitecturalStyle(EDominionArchitecturalStyle::PrimitiveEarthAndMud);
	}

	// --- 5. Spawn Player 100-Soldier Mass Legion (10x10 Phalanx Shield Wall) ---
	const float UnitSpacing = 135.0f;
	const int32 FormationRows = 10;
	const int32 FormationCols = 10; // 100 Soldiers!

	for (int32 Row = 0; Row < FormationRows; ++Row)
	{
		for (int32 Col = 0; Col < FormationCols; ++Col)
		{
			FVector Loc(100.0f + (Row * UnitSpacing), -600.0f + (Col * UnitSpacing), 100.0f);
			ADominionUnitActor* Spearman = World->SpawnActor<ADominionUnitActor>(
				ADominionUnitActor::StaticClass(),
				Loc,
				FRotator(0.0f, 0.0f, 0.0f),
				SpawnParams
			);
			if (Spearman)
			{
				Spearman->TeamID = 0;
				Spearman->UnitName = TEXT("Bronze Legionary Spearman");
				Spearman->bInShieldWall = true;
			}
		}
	}

	// --- 6. Spawn Player Heavy Chariot Vanguard (8 Heavy War Wagons) ---
	for (int32 i = 0; i < 8; ++i)
	{
		FVector Loc(100.0f + (i * 200.0f), 900.0f, 100.0f);
		ADominionUnitActor* Chariot = World->SpawnActor<ADominionUnitActor>(
			ADominionUnitActor::StaticClass(),
			Loc,
			FRotator::ZeroRotator,
			SpawnParams
		);
		if (Chariot)
		{
			Chariot->TeamID = 0;
			Chariot->UnitName = TEXT("Heavy War Chariot");
			Chariot->UnitType = EDominionUnitType::HeavyChariot;
			Chariot->MoveSpeed = 580.0f;
			Chariot->AttackPower = 75.0f;
			Chariot->Health = 700.0f;
			Chariot->MaxHealth = 700.0f;
		}
	}

	// --- 7. Spawn 4 Ox-Cart Baggage Train Supply Wagons ---
	for (int32 i = 0; i < 4; ++i)
	{
		FVector Loc(-800.0f + (i * 260.0f), -1400.0f, 100.0f);
		ADominionUnitActor* OxCart = World->SpawnActor<ADominionUnitActor>(
			ADominionUnitActor::StaticClass(),
			Loc,
			FRotator::ZeroRotator,
			SpawnParams
		);
		if (OxCart)
		{
			OxCart->TeamID = 0;
			OxCart->UnitName = TEXT("Imperial Baggage Train");
			OxCart->UnitType = EDominionUnitType::OxCartSupply;
			OxCart->MoveSpeed = 260.0f;
		}
	}

	// --- 8. Spawn Enemy 100-Soldier Raider Horde (10x10 Opposing Army) ---
	for (int32 Row = 0; Row < FormationRows; ++Row)
	{
		for (int32 Col = 0; Col < FormationCols; ++Col)
		{
			FVector Loc(3200.0f + (Row * UnitSpacing), -600.0f + (Col * UnitSpacing), 100.0f);
			ADominionUnitActor* EnemyUnit = World->SpawnActor<ADominionUnitActor>(
				ADominionUnitActor::StaticClass(),
				Loc,
				FRotator(0.0f, 180.0f, 0.0f), // Facing player army
				SpawnParams
			);
			if (EnemyUnit)
			{
				EnemyUnit->TeamID = 1; // Red Hostile Raider
				EnemyUnit->UnitName = TEXT("Nomadic Raider Vanguard");
			}
		}
	}
}
