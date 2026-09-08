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

	// Automatically destroy default template playground obstacles (SM_Ramp, SM_QuarterCylinder)
	TArray<AActor*> LevelActors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), LevelActors);
	for (AActor* Act : LevelActors)
	{
		if (Act && Act != this)
		{
			FString ActorName = Act->GetName();
			if (ActorName.Contains(TEXT("SM_Ramp")) || ActorName.Contains(TEXT("SM_QuarterCylinder")) ||
			    ActorName.Contains(TEXT("TopDownCharacter")) || ActorName.Contains(TEXT("BP_TopDown")))
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
			RTSPawn = World->SpawnActor<ADominionRTSPawn>(ADominionRTSPawn::StaticClass(), FVector(-1800.0f, 0.0f, 1200.0f), FRotator::ZeroRotator, SpawnParams);
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

	// Generate Procedural Mesopotamian PBR Textures
	UTexture2D* SandstoneAlbedo = UDominionTextureFactory::CreateSandstoneSiltAlbedo(512, 512);
	UTexture2D* SandstoneNormal = UDominionTextureFactory::CreateSandstoneSiltNormal(512, 512);
	UTexture2D* WaterNormal = UDominionTextureFactory::CreateWaterWaveNormal(512, 512);
	UTexture2D* MudbrickAlbedo = UDominionTextureFactory::CreateMudbrickAlbedo(512, 512);
	UTexture2D* MudbrickNormal = UDominionTextureFactory::CreateMudbrickNormal(512, 512);
	UTexture2D* CedarAlbedo = UDominionTextureFactory::CreateCedarWoodAlbedo(512, 512);

	// --- 1. Euphrates Valley River Water Plane & Silt Floodplain ---
	if (PlaneMesh)
	{
		// Shimmering Euphrates River Plane
		AStaticMeshActor* RiverActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(1800.0f, 0.0f, 25.0f), FRotator::ZeroRotator, SpawnParams);
		if (RiverActor && RiverActor->GetStaticMeshComponent())
		{
			RiverActor->GetStaticMeshComponent()->SetStaticMesh(PlaneMesh);
			RiverActor->SetActorScale3D(FVector(180.0f, 600.0f, 1.0f));
			UMaterialInstanceDynamic* RiverMat = RiverActor->GetStaticMeshComponent()->CreateAndSetMaterialInstanceDynamic(0);
			if (RiverMat)
			{
				RiverMat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.04f, 0.28f, 0.46f, 0.92f));
				RiverMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.04f, 0.28f, 0.46f, 0.92f));
				RiverMat->SetScalarParameterValue(TEXT("Roughness"), 0.04f);
				RiverMat->SetScalarParameterValue(TEXT("Metallic"), 0.25f);
				RiverMat->SetScalarParameterValue(TEXT("Specular"), 0.95f);
				if (WaterNormal)
				{
					RiverMat->SetTextureParameterValue(TEXT("NormalMap"), WaterNormal);
					RiverMat->SetTextureParameterValue(TEXT("Normal"), WaterNormal);
				}
			}
		}

		// Fertile Oasis Floodplain Ground
		AStaticMeshActor* OasisActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(1200.0f, 0.0f, 22.0f), FRotator::ZeroRotator, SpawnParams);
		if (OasisActor && OasisActor->GetStaticMeshComponent())
		{
			OasisActor->GetStaticMeshComponent()->SetStaticMesh(PlaneMesh);
			OasisActor->SetActorScale3D(FVector(80.0f, 500.0f, 1.0f));
			UMaterialInstanceDynamic* OasisMat = OasisActor->GetStaticMeshComponent()->CreateAndSetMaterialInstanceDynamic(0);
			if (OasisMat)
			{
				OasisMat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.22f, 0.44f, 0.16f)); // Lush floodplain green
				OasisMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.22f, 0.44f, 0.16f));
				OasisMat->SetScalarParameterValue(TEXT("Roughness"), 0.85f);
				if (SandstoneNormal)
				{
					OasisMat->SetTextureParameterValue(TEXT("NormalMap"), SandstoneNormal);
					OasisMat->SetTextureParameterValue(TEXT("Normal"), SandstoneNormal);
				}
			}
		}

		// City Foundation Plaza (Sandstone Silt Ground)
		AStaticMeshActor* PlazaActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(-300.0f, 0.0f, 20.0f), FRotator::ZeroRotator, SpawnParams);
		if (PlazaActor && PlazaActor->GetStaticMeshComponent())
		{
			PlazaActor->GetStaticMeshComponent()->SetStaticMesh(PlaneMesh);
			PlazaActor->SetActorScale3D(FVector(250.0f, 300.0f, 1.0f));
			UMaterialInstanceDynamic* PlazaMat = PlazaActor->GetStaticMeshComponent()->CreateAndSetMaterialInstanceDynamic(0);
			if (PlazaMat)
			{
				PlazaMat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.74f, 0.60f, 0.42f));
				PlazaMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.74f, 0.60f, 0.42f));
				PlazaMat->SetScalarParameterValue(TEXT("Roughness"), 0.90f);
				if (SandstoneAlbedo)
				{
					PlazaMat->SetTextureParameterValue(TEXT("BaseColorMap"), SandstoneAlbedo);
					PlazaMat->SetTextureParameterValue(TEXT("AlbedoMap"), SandstoneAlbedo);
					PlazaMat->SetTextureParameterValue(TEXT("Texture"), SandstoneAlbedo);
				}
				if (SandstoneNormal)
				{
					PlazaMat->SetTextureParameterValue(TEXT("NormalMap"), SandstoneNormal);
					PlazaMat->SetTextureParameterValue(TEXT("Normal"), SandstoneNormal);
				}
			}
		}

		// Marching Road Silt Ribbon
		AStaticMeshActor* RoadActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(400.0f, 0.0f, 23.0f), FRotator::ZeroRotator, SpawnParams);
		if (RoadActor && RoadActor->GetStaticMeshComponent())
		{
			RoadActor->GetStaticMeshComponent()->SetStaticMesh(PlaneMesh);
			RoadActor->SetActorScale3D(FVector(140.0f, 40.0f, 1.0f));
			UMaterialInstanceDynamic* RoadMat = RoadActor->GetStaticMeshComponent()->CreateAndSetMaterialInstanceDynamic(0);
			if (RoadMat)
			{
				RoadMat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.55f, 0.42f, 0.28f));
				RoadMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.55f, 0.42f, 0.28f));
				RoadMat->SetScalarParameterValue(TEXT("Roughness"), 0.85f);
				if (MudbrickNormal)
				{
					RoadMat->SetTextureParameterValue(TEXT("NormalMap"), MudbrickNormal);
					RoadMat->SetTextureParameterValue(TEXT("Normal"), MudbrickNormal);
				}
			}
		}
	}

	// --- 2. Procedural Date Palm Groves ---
	if (CylinderMesh && ConeMesh)
	{
		const FVector PalmLocations[] = {
			FVector(1100.0f, -400.0f, 30.0f),
			FVector(1150.0f, -600.0f, 30.0f),
			FVector(1080.0f, 500.0f, 30.0f),
			FVector(1220.0f, 750.0f, 30.0f),
			FVector(-600.0f, 700.0f, 30.0f),
			FVector(-750.0f, 850.0f, 30.0f),
			FVector(1300.0f, -200.0f, 30.0f),
			FVector(1350.0f, 300.0f, 30.0f)
		};

		for (const FVector& PalmLoc : PalmLocations)
		{
			// Palm Trunk
			AStaticMeshActor* Trunk = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), PalmLoc, FRotator::ZeroRotator, SpawnParams);
			if (Trunk && Trunk->GetStaticMeshComponent())
			{
				Trunk->GetStaticMeshComponent()->SetStaticMesh(CylinderMesh);
				Trunk->SetActorScale3D(FVector(0.35f, 0.35f, 3.2f));
				UMaterialInstanceDynamic* TrunkMat = Trunk->GetStaticMeshComponent()->CreateAndSetMaterialInstanceDynamic(0);
				if (TrunkMat)
				{
					TrunkMat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.36f, 0.22f, 0.12f));
					TrunkMat->SetScalarParameterValue(TEXT("Roughness"), 0.90f);
				}
			}

			// Palm Leaf Canopy Crown Layer 1
			AStaticMeshActor* Crown1 = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), PalmLoc + FVector(0.0f, 0.0f, 170.0f), FRotator(180.0f, 0.0f, 0.0f), SpawnParams);
			if (Crown1 && Crown1->GetStaticMeshComponent())
			{
				Crown1->GetStaticMeshComponent()->SetStaticMesh(ConeMesh);
				Crown1->SetActorScale3D(FVector(2.8f, 2.8f, 1.1f));
				UMaterialInstanceDynamic* CrownMat = Crown1->GetStaticMeshComponent()->CreateAndSetMaterialInstanceDynamic(0);
				if (CrownMat)
				{
					CrownMat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.16f, 0.46f, 0.12f));
					CrownMat->SetScalarParameterValue(TEXT("Roughness"), 0.70f);
				}
			}

			// Palm Leaf Canopy Crown Layer 2 (Top Tier)
			AStaticMeshActor* Crown2 = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), PalmLoc + FVector(0.0f, 0.0f, 210.0f), FRotator(180.0f, 45.0f, 0.0f), SpawnParams);
			if (Crown2 && Crown2->GetStaticMeshComponent())
			{
				Crown2->GetStaticMeshComponent()->SetStaticMesh(ConeMesh);
				Crown2->SetActorScale3D(FVector(1.9f, 1.9f, 0.9f));
				UMaterialInstanceDynamic* CrownMat = Crown2->GetStaticMeshComponent()->CreateAndSetMaterialInstanceDynamic(0);
				if (CrownMat)
				{
					CrownMat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.24f, 0.58f, 0.18f));
					CrownMat->SetScalarParameterValue(TEXT("Roughness"), 0.65f);
				}
			}
		}
	}

	// --- 3. Colossal Unimpregnable Fortification Walls & Bastions (80m Tall) ---
	if (CubeMesh)
	{
		const FVector WallLocations[] = {
			FVector(-2800.0f, 0.0f, 260.0f),    // Colossal Rear Keep Wall
			FVector(-1200.0f, -2200.0f, 260.0f), // Left Flank Wall
			FVector(-1200.0f, 2200.0f, 260.0f)   // Right Flank Wall
		};

		const FVector WallScales[] = {
			FVector(2.5f, 48.0f, 6.0f),
			FVector(36.0f, 2.5f, 6.0f),
			FVector(36.0f, 2.5f, 6.0f)
		};

		for (int32 w = 0; w < 3; ++w)
		{
			AStaticMeshActor* Wall = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), WallLocations[w], FRotator::ZeroRotator, SpawnParams);
			if (Wall && Wall->GetStaticMeshComponent())
			{
				Wall->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
				Wall->SetActorScale3D(WallScales[w]);
				UMaterialInstanceDynamic* WallMat = Wall->GetStaticMeshComponent()->CreateAndSetMaterialInstanceDynamic(0);
				if (WallMat)
				{
					WallMat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.68f, 0.52f, 0.36f));
					WallMat->SetScalarParameterValue(TEXT("Roughness"), 0.88f);
				}
			}
		}

		// Monumental Corner Citadel Bastions (Towering 90m)
		const FVector BastionLocations[] = {
			FVector(-2800.0f, -2200.0f, 380.0f),
			FVector(-2800.0f, 2200.0f, 380.0f),
			FVector(400.0f, -2200.0f, 380.0f),
			FVector(400.0f, 2200.0f, 380.0f)
		};

		for (const FVector& BastionLoc : BastionLocations)
		{
			AStaticMeshActor* Bastion = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), BastionLoc, FRotator::ZeroRotator, SpawnParams);
			if (Bastion && Bastion->GetStaticMeshComponent())
			{
				Bastion->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
				Bastion->SetActorScale3D(FVector(4.5f, 4.5f, 9.5f));
				UMaterialInstanceDynamic* BastionMat = Bastion->GetStaticMeshComponent()->CreateAndSetMaterialInstanceDynamic(0);
				if (BastionMat)
				{
					BastionMat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.58f, 0.44f, 0.30f));
					BastionMat->SetScalarParameterValue(TEXT("Roughness"), 0.85f);
				}
			}
		}
	}

	// --- 4. Spawn Colossal Citadel & Settlements ---
	// 1. Colossal Citadel Town Center
	ADominionBuildingActor* Citadel = World->SpawnActor<ADominionBuildingActor>(
		ADominionBuildingActor::StaticClass(),
		FVector(-1200.0f, 0.0f, 100.0f),
		FRotator::ZeroRotator,
		SpawnParams
	);
	if (Citadel)
	{
		Citadel->BuildingName = TEXT("Grand Monumental Citadel of Dominion");
		Citadel->TeamID = 0;
		Citadel->SetArchitecturalStyle(EDominionArchitecturalStyle::AngkorWatHewnStone);
	}

	// 2. Colossal Granary Vault
	ADominionBuildingActor* Granary = World->SpawnActor<ADominionBuildingActor>(
		ADominionBuildingActor::StaticClass(),
		FVector(-2000.0f, 950.0f, 100.0f),
		FRotator::ZeroRotator,
		SpawnParams
	);
	if (Granary)
	{
		Granary->BuildingName = TEXT("Megalithic Granary Vault");
		Granary->BuildingType = EDominionBuildingType::MudbrickGranary;
		Granary->TeamID = 0;
	}

	// 3. Colossal War Forge & Barracks
	ADominionBuildingActor* Barracks = World->SpawnActor<ADominionBuildingActor>(
		ADominionBuildingActor::StaticClass(),
		FVector(-2000.0f, -950.0f, 100.0f),
		FRotator::ZeroRotator,
		SpawnParams
	);
	if (Barracks)
	{
		Barracks->BuildingName = TEXT("Colossal War Forge & Barracks");
		Barracks->BuildingType = EDominionBuildingType::BronzeBarracks;
		Barracks->TeamID = 0;
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
