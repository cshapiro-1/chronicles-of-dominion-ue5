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
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f; // 10 Hz GameMode Tick

	DefaultPawnClass = ADominionRTSPawn::StaticClass();
	PlayerControllerClass = ADominionRTSPlayerController::StaticClass();
	HUDClass = ADominionRTSHUD::StaticClass();
}

void ADominionGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Coarse 1.0s Accumulator for Economy Simulation (Governance Rule 1)
	ResourceTickAccumulator += DeltaTime;
	if (ResourceTickAccumulator >= 1.0f)
	{
		Grain += GrainIncomeRate;
		Clay += ClayIncomeRate;
		Bronze += BronzeIncomeRate;
		ResourceTickAccumulator = 0.0f;
	}
}

bool ADominionGameModeBase::CanAfford(float InGrain, float InClay, float InBronze) const
{
	return (Grain >= InGrain && Clay >= InClay && Bronze >= InBronze);
}

bool ADominionGameModeBase::SpendResources(float InGrain, float InClay, float InBronze)
{
	if (CanAfford(InGrain, InClay, InBronze))
	{
		Grain -= InGrain;
		Clay -= InClay;
		Bronze -= InBronze;
		return true;
	}
	return false;
}

ADominionUnitActor* ADominionGameModeBase::SpawnSpearman(int32 TeamID, FVector Location)
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	if (TeamID == 0 && !SpendResources(50.0f, 0.0f, 20.0f))
	{
		return nullptr; // Cannot afford
	}

	if (Location.IsNearlyZero())
	{
		Location = FVector(-1200.0f + FMath::RandRange(-150.0f, 150.0f), -600.0f + FMath::RandRange(-150.0f, 150.0f), 100.0f);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADominionUnitActor* Spearman = World->SpawnActor<ADominionUnitActor>(ADominionUnitActor::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);
	if (Spearman)
	{
		Spearman->ConfigureUnitType(EDominionUnitType::BronzeSpearman, TeamID);
	}
	return Spearman;
}

ADominionUnitActor* ADominionGameModeBase::SpawnSlinger(int32 TeamID, FVector Location)
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	if (TeamID == 0 && !SpendResources(40.0f, 30.0f, 0.0f))
	{
		return nullptr; // Cannot afford
	}

	if (Location.IsNearlyZero())
	{
		Location = FVector(-1200.0f + FMath::RandRange(-150.0f, 150.0f), -400.0f + FMath::RandRange(-150.0f, 150.0f), 100.0f);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADominionUnitActor* Slinger = World->SpawnActor<ADominionUnitActor>(ADominionUnitActor::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);
	if (Slinger)
	{
		Slinger->ConfigureUnitType(EDominionUnitType::Slinger, TeamID);
	}
	return Slinger;
}

ADominionUnitActor* ADominionGameModeBase::SpawnChariot(int32 TeamID, FVector Location)
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	if (TeamID == 0 && !SpendResources(90.0f, 0.0f, 50.0f))
	{
		return nullptr;
	}

	if (Location.IsNearlyZero())
	{
		Location = FVector(-1200.0f + FMath::RandRange(-150.0f, 150.0f), -200.0f + FMath::RandRange(-150.0f, 150.0f), 100.0f);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADominionUnitActor* Chariot = World->SpawnActor<ADominionUnitActor>(ADominionUnitActor::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);
	if (Chariot)
	{
		Chariot->ConfigureUnitType(EDominionUnitType::HeavyChariot, TeamID);
	}
	return Chariot;
}

ADominionUnitActor* ADominionGameModeBase::SpawnBaggageTrain(int32 TeamID, FVector Location)
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	if (TeamID == 0 && !SpendResources(30.0f, 0.0f, 15.0f))
	{
		return nullptr;
	}

	if (Location.IsNearlyZero())
	{
		Location = FVector(-1400.0f + FMath::RandRange(-100.0f, 100.0f), -300.0f + FMath::RandRange(-100.0f, 100.0f), 100.0f);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADominionUnitActor* Wagon = World->SpawnActor<ADominionUnitActor>(ADominionUnitActor::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);
	if (Wagon)
	{
		Wagon->ConfigureUnitType(EDominionUnitType::OxCartSupply, TeamID);
	}
	return Wagon;
}

void ADominionGameModeBase::SpawnSpearmenBatch(int32 Count, int32 TeamID)
{
	for (int32 i = 0; i < Count; ++i)
	{
		FVector Loc = FVector(-1000.0f + (i * 140.0f), -500.0f, 100.0f);
		SpawnSpearman(TeamID, Loc);
	}
}

ADominionUnitActor* ADominionGameModeBase::SpawnEnemyDummy(FVector Location)
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADominionUnitActor* Dummy = World->SpawnActor<ADominionUnitActor>(ADominionUnitActor::StaticClass(), Location, FRotator(0.0f, 180.0f, 0.0f), SpawnParams);
	if (Dummy)
	{
		Dummy->ConfigureUnitType(EDominionUnitType::DummyTarget, 1); // Red Team AI
	}
	return Dummy;
}

void ADominionGameModeBase::SpawnEnemyWave(int32 SpearmenCount, int32 SlingersCount)
{
	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn invading wave outside the South Gatehouse
	const FVector GateSpawnOrigin(0.0f, -2200.0f, 10.0f);
	const FVector CourtyardTarget(-200.0f, -200.0f, 10.0f);

	for (int32 i = 0; i < SpearmenCount; ++i)
	{
		FVector Loc = GateSpawnOrigin + FVector(-200.0f + (i * 120.0f), -150.0f, 0.0f);
		ADominionUnitActor* Enemy = World->SpawnActor<ADominionUnitActor>(ADominionUnitActor::StaticClass(), Loc, FRotator(0.0f, 90.0f, 0.0f), SpawnParams);
		if (Enemy)
		{
			Enemy->ConfigureUnitType(EDominionUnitType::BronzeSpearman, 1); // Red Team Raider
			Enemy->MoveToLocation(CourtyardTarget + FVector(FMath::RandRange(-200.0f, 200.0f), FMath::RandRange(-200.0f, 200.0f), 0.0f));
		}
	}

	for (int32 j = 0; j < SlingersCount; ++j)
	{
		FVector Loc = GateSpawnOrigin + FVector(-150.0f + (j * 150.0f), -350.0f, 0.0f);
		ADominionUnitActor* EnemySlinger = World->SpawnActor<ADominionUnitActor>(ADominionUnitActor::StaticClass(), Loc, FRotator(0.0f, 90.0f, 0.0f), SpawnParams);
		if (EnemySlinger)
		{
			EnemySlinger->ConfigureUnitType(EDominionUnitType::Slinger, 1); // Red Team Raider Slinger
			EnemySlinger->MoveToLocation(CourtyardTarget + FVector(FMath::RandRange(-200.0f, 200.0f), FMath::RandRange(-200.0f, 200.0f), 0.0f));
		}
	}

	CurrentWave++;
}

void ADominionGameModeBase::StartPlay()
{
	Super::StartPlay();

	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Ensure PlayerController possesses dedicated ADominionRTSPawn
	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (PC)
	{
		ADominionRTSPawn* RTSPawn = Cast<ADominionRTSPawn>(PC->GetPawn());
		if (!RTSPawn)
		{
			RTSPawn = World->SpawnActor<ADominionRTSPawn>(ADominionRTSPawn::StaticClass(), FVector(-750.0f, -750.0f, 300.0f), FRotator(-36.0f, 38.0f, 0.0f), SpawnParams);
			if (RTSPawn)
			{
				PC->Possess(RTSPawn);
			}
		}
	}

	// Spawn Starting Player Cohort in the open Citadel Courtyard
	SpawnSpearman(0, FVector(-200.0f, -100.0f, 10.0f));
	SpawnSpearman(0, FVector(-200.0f, -250.0f, 10.0f));
	SpawnSpearman(0, FVector(-200.0f, -400.0f, 10.0f));
	SpawnSlinger(0, FVector(-350.0f, -250.0f, 10.0f));
	SpawnSlinger(0, FVector(-350.0f, -400.0f, 10.0f));

	// Spawn Starting Target Dummy for tactical combat verification
	SpawnEnemyDummy(FVector(350.0f, -250.0f, 10.0f));
}

