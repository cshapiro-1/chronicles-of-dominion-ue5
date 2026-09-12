#include "DominionUnitActor.h"
#include "DominionTextureFactory.h"
#include "DominionFormationSystem.h"
#include "DominionPoliticalEstatesSystem.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Animation/Skeleton.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ADominionUnitActor::ADominionUnitActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.02f; // 50 Hz Logic Tick for high performance

	// Find standard basic shape meshes
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeFinder(TEXT("/Engine/BasicShapes/Cone.Cone"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SpearmanFinder(TEXT("/Game/Characters/Units/SM_Sumerian_Spearman.SM_Sumerian_Spearman"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SlingerFinder(TEXT("/Game/Characters/Units/SM_Sumerian_Slinger.SM_Sumerian_Slinger"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MannyFinder(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny"));

	UStaticMesh* CylinderMesh = CylinderFinder.Succeeded() ? CylinderFinder.Object : nullptr;
	UStaticMesh* SphereMesh = SphereFinder.Succeeded() ? SphereFinder.Object : nullptr;
	UStaticMesh* CubeMesh = CubeFinder.Succeeded() ? CubeFinder.Object : nullptr;
	UStaticMesh* ConeMesh = ConeFinder.Succeeded() ? ConeFinder.Object : nullptr;
	UStaticMesh* SpearmanMesh = SpearmanFinder.Succeeded() ? SpearmanFinder.Object : nullptr;
	UStaticMesh* SlingerMesh = SlingerFinder.Succeeded() ? SlingerFinder.Object : nullptr;
	USkeletalMesh* MannyMesh = MannyFinder.Succeeded() ? MannyFinder.Object : nullptr;

	// 1. Root Capsule Collider (Eliminates FindTeleportSpot intersection warnings)
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->InitCapsuleSize(42.0f, 96.0f);
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
	CapsuleComponent->SetMobility(EComponentMobility::Movable);
	CapsuleComponent->SetCanEverAffectNavigation(true);

	// 2. Unit Torso / Chassis Mesh (Modular Static Mesh Fallback)
	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));
	UnitMesh->SetupAttachment(RootComponent);
	UnitMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UnitMesh->SetMobility(EComponentMobility::Movable);
	if (CylinderMesh)
	{
		UnitMesh->SetStaticMesh(CylinderMesh);
		UnitMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		UnitMesh->SetRelativeScale3D(FVector(0.55f, 0.55f, 1.1f));
	}

	// 3. High-Fidelity 3D Skeletal Mesh Warrior (With Skeleton Asset Fallback Guard)
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	const bool bHasValidSkeleton = (MannyMesh != nullptr && MannyMesh->GetSkeleton() != nullptr);
	if (bHasValidSkeleton)
	{
		SkeletalMesh->SetSkeletalMesh(MannyMesh);
		SkeletalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		SkeletalMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		SkeletalMesh->SetRelativeScale3D(FVector(0.95f, 0.95f, 0.95f));
		UnitMesh->SetVisibility(false); // Hide the prototype geometric cylinder
	}
	else
	{
		// Safe fallback: disable skeletal component to avoid animation evaluator warnings
		SkeletalMesh->SetVisibility(false);
		SkeletalMesh->SetComponentTickEnabled(false);
		UnitMesh->SetVisibility(true);
	}

	// 4. Selection Ring on Ground
	SelectionRingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionRingMesh"));
	SelectionRingMesh->SetupAttachment(RootComponent);
	SelectionRingMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
	SelectionRingMesh->SetRelativeScale3D(FVector(1.6f, 1.6f, 0.03f));
	SelectionRingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SelectionRingMesh->SetMobility(EComponentMobility::Movable);
	SelectionRingMesh->SetVisibility(false);
	if (CylinderMesh)
	{
		SelectionRingMesh->SetStaticMesh(CylinderMesh);
	}

	// 5. Overhead Billboard Display
	OverheadStatusText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("OverheadStatusText"));
	OverheadStatusText->SetupAttachment(RootComponent);
	OverheadStatusText->SetVisibility(false);
}

void ADominionUnitActor::ConfigureUnitType(EDominionUnitType NewType, int32 NewTeamID)
{
	UnitType = NewType;
	TeamID = NewTeamID;

	const FLinearColor TeamColor = (TeamID == 0) ? FLinearColor(0.12f, 0.45f, 0.95f) : FLinearColor(0.88f, 0.15f, 0.15f);

	static UStaticMesh* LoadedSpearman = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Characters/Units/SM_Sumerian_Spearman.SM_Sumerian_Spearman")));
	static UStaticMesh* LoadedSlinger = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Characters/Units/SM_Sumerian_Slinger.SM_Sumerian_Slinger")));

	if (UnitType == EDominionUnitType::BronzeSpearman)
	{
		UnitName = (TeamID == 0) ? TEXT("Bronze Spearman") : TEXT("Enemy Spearman");
		MaxHealth = 100.0f;
		Health = 100.0f;
		AttackPower = 25.0f;
		Armor = 6.0f;
		MoveSpeed = 360.0f;
		BaseMoveSpeed = 360.0f;
		AttackRange = 220.0f;
		AttackInterval = 1.0f;
		bInShieldWall = true;

		if (LoadedSpearman && UnitMesh)
		{
			UnitMesh->SetStaticMesh(LoadedSpearman);
			UnitMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
			UnitMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
			UnitMesh->SetVisibility(true);
		}
	}
	else if (UnitType == EDominionUnitType::Slinger)
	{
		UnitName = (TeamID == 0) ? TEXT("Mesopotamian Slinger") : TEXT("Enemy Slinger");
		MaxHealth = 70.0f;
		Health = 70.0f;
		AttackPower = 18.0f;
		Armor = 2.0f;
		MoveSpeed = 400.0f;
		BaseMoveSpeed = 400.0f;
		AttackRange = 900.0f;
		AttackInterval = 1.2f;
		bInShieldWall = false;

		if (LoadedSlinger && UnitMesh)
		{
			UnitMesh->SetStaticMesh(LoadedSlinger);
			UnitMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
			UnitMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
			UnitMesh->SetVisibility(true);
		}
	}
	else if (UnitType == EDominionUnitType::DummyTarget)
	{
		UnitName = TEXT("Training Target Dummy");
		MaxHealth = 300.0f;
		Health = 300.0f;
		AttackPower = 0.0f;
		Armor = 0.0f;
		MoveSpeed = 0.0f;
		BaseMoveSpeed = 0.0f;
		AttackRange = 0.0f;
		AttackInterval = 10.0f;
		bInShieldWall = false;
	}

	if (UnitMesh && UnitMesh->GetStaticMesh() && UnitMesh->GetNumMaterials() > 0)
	{
		UMaterialInstanceDynamic* DynMat = UnitMesh->CreateAndSetMaterialInstanceDynamic(0);
		if (DynMat)
		{
			DynMat->SetVectorParameterValue(TEXT("BaseColor"), TeamColor);
			DynMat->SetVectorParameterValue(TEXT("Color"), TeamColor);
		}
	}

	if (OverheadStatusText)
	{
		OverheadStatusText->SetVisibility(false);
	}
}

void ADominionUnitActor::BeginPlay()
{
	Super::BeginPlay();

	TargetDestination = GetActorLocation();

	if (UnitType == EDominionUnitType::BronzeSpearman || UnitType == EDominionUnitType::Slinger || UnitType == EDominionUnitType::DummyTarget)
	{
		ConfigureUnitType(UnitType, TeamID);
	}
	else
	{
		ConfigureUnitType(EDominionUnitType::BronzeSpearman, TeamID);
	}
}

void ADominionUnitActor::MoveToFormationSlot(const FVector& SlotTarget, const FRotator& TargetHeading)
{
	FormationSlotTarget = SlotTarget;
	DesiredHeading = TargetHeading;
	MoveToLocation(SlotTarget);
}

void ADominionUnitActor::ReplenishFieldRations(float Amount)
{
	bIsResupplying = true;
	FieldRations = FMath::Min(MaxFieldRations, FieldRations + Amount);
	bIsStarving = false;
}

void ADominionUnitActor::ConsumeFieldRations(float DeltaTime)
{
	bIsResupplying = false;
	FieldRations = FMath::Max(0.0f, FieldRations - DeltaTime);
	bIsStarving = (FieldRations <= 0.0f);
}

void ADominionUnitActor::SetStarvingState(bool bNewStarving)
{
	bIsStarving = bNewStarving;
}

void ADominionUnitActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Health <= 0.0f)
	{
		Destroy();
		return;
	}

	const float WorldTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	// 1. Combat Loop & Targeting
	if (CurrentTargetUnit && IsValid(CurrentTargetUnit) && CurrentTargetUnit->Health > 0.0f)
	{
		const float DistToTarget = FVector::Dist2D(GetActorLocation(), CurrentTargetUnit->GetActorLocation());
		if (DistToTarget <= AttackRange)
		{
			bHasDestination = false;
			FVector DirToTarget = (CurrentTargetUnit->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
			if (!DirToTarget.IsNearlyZero())
			{
				SetActorRotation(FMath::RInterpTo(GetActorRotation(), DirToTarget.Rotation(), DeltaTime, 12.0f));
			}

			AttackCooldownTimer -= DeltaTime;

			if (AttackCooldownTimer <= 0.0f)
			{
				// Deliver Damage
				CurrentTargetUnit->TakeCombatDamage(AttackPower, 2.0f, this, DirToTarget);

				// Slinger Visual Tracer
				if (UnitType == EDominionUnitType::Slinger)
				{
					if (UWorld* World = GetWorld())
					{
						DrawDebugLine(World, GetActorLocation() + FVector(0.0f, 0.0f, 60.0f), CurrentTargetUnit->GetActorLocation() + FVector(0.0f, 0.0f, 60.0f), FColor::Yellow, false, 0.20f, 0, 3.5f);
					}
				}

				AttackCooldownTimer = AttackInterval;
				CombatState = EDominionCombatState::InCombat;
			}
		}
		else
		{
			// March into attack range
			MoveToLocation(CurrentTargetUnit->GetActorLocation());
		}
	}
	else
	{
		if (CurrentTargetUnit != nullptr)
		{
			CurrentTargetUnit = nullptr;
			CombatState = EDominionCombatState::Idle;
		}

		// Auto-Attack Search (only when idle, not moving, and no target)
		if (CombatState == EDominionCombatState::Idle && !bHasDestination && CurrentTargetUnit == nullptr && AttackPower > 0.0f)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				TArray<AActor*> AllUnits;
				UGameplayStatics::GetAllActorsOfClass(World, ADominionUnitActor::StaticClass(), AllUnits);

				float NearestDist = AutoAttackRadius;
				ADominionUnitActor* BestTarget = nullptr;

				for (AActor* Act : AllUnits)
				{
					if (ADominionUnitActor* Candidate = Cast<ADominionUnitActor>(Act))
					{
						if (Candidate != this && Candidate->TeamID != this->TeamID && Candidate->Health > 0.0f)
						{
							float D = FVector::Dist2D(GetActorLocation(), Candidate->GetActorLocation());
							if (D < NearestDist)
							{
								NearestDist = D;
								BestTarget = Candidate;
							}
						}
					}
				}

				if (BestTarget)
				{
					AttackTarget(BestTarget);
				}
			}
		}
	}

	// 2. Direct Movement Execution
	if (bHasDestination)
	{
		FVector CurrentLoc = GetActorLocation();
		FVector Direction = (TargetDestination - CurrentLoc);
		Direction.Z = 0.0f; // Stay on terrain plane

		const float Dist = Direction.Size();
		if (Dist > 25.0f)
		{
			Direction.Normalize();
			FVector NewLoc = CurrentLoc + Direction * MoveSpeed * DeltaTime;
			NewLoc.Z = CurrentLoc.Z; // Maintain elevation above terrain
			SetActorLocation(NewLoc, false);

			FRotator TargetRot = (!DesiredHeading.IsZero() && Dist < 80.0f) ? DesiredHeading : Direction.Rotation();
			SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 12.0f));
			CombatState = EDominionCombatState::Marching;
		}
		else
		{
			bHasDestination = false;
			if (CombatState == EDominionCombatState::Marching)
			{
				CombatState = EDominionCombatState::Idle;
			}
			if (!DesiredHeading.IsZero())
			{
				SetActorRotation(DesiredHeading);
			}
		}
	}

	// 3. Real-time Overhead Status Update & Morale Panic
	if (CombatState == EDominionCombatState::RoutingPanic)
	{
		RoutTimer -= DeltaTime;
		if (RoutTimer <= 0.0f)
		{
			CombatState = EDominionCombatState::Idle;
			Morale = 45.0f;
			MoveSpeed = BaseMoveSpeed;
		}
	}

	if (DamageFeedbackTimer > 0.0f)
	{
		DamageFeedbackTimer -= DeltaTime;
	}

	if (OverheadStatusText)
	{
		if (CombatState == EDominionCombatState::RoutingPanic)
		{
			OverheadStatusText->SetVisibility(true);
			OverheadStatusText->SetText(FText::FromString(TEXT(">>> ROUT! RETREATING <<<")));
			OverheadStatusText->SetTextRenderColor(FColor(255, 30, 30));
		}
		else if (DamageFeedbackTimer > 0.0f)
		{
			OverheadStatusText->SetVisibility(true);
			FString DmgStr = FString::Printf(TEXT("-%d HP  [%d/%d]"), FMath::RoundToInt(LastDamageTaken), FMath::RoundToInt(Health), FMath::RoundToInt(MaxHealth));
			OverheadStatusText->SetText(FText::FromString(DmgStr));
			OverheadStatusText->SetTextRenderColor(FColor(255, 220, 60));
		}
		else if (bIsSelected || bIsHovered)
		{
			OverheadStatusText->SetVisibility(true);
			FString StatusStr = FString::Printf(TEXT("%s | HP: %d/%d"), *UnitName, FMath::RoundToInt(Health), FMath::RoundToInt(MaxHealth));
			OverheadStatusText->SetText(FText::FromString(StatusStr));
			OverheadStatusText->SetTextRenderColor(bIsSelected ? ((TeamID == 0) ? FColor(255, 215, 0) : FColor(255, 60, 60)) : FColor(100, 220, 255));
		}
		else
		{
			OverheadStatusText->SetVisibility(false);
		}
	}
}

void ADominionUnitActor::SetSelected(bool bNewSelected)
{
	bIsSelected = bNewSelected;
	if (SelectionRingMesh)
	{
		SelectionRingMesh->SetVisibility(bIsSelected || bIsHovered);
		if (bIsSelected)
		{
			UMaterialInstanceDynamic* DynMat = UDominionTextureFactory::CreateDominionMaterial(this, FLinearColor(1.0f, 0.82f, 0.18f), 0.9f, 0.2f, FLinearColor(3.0f, 2.2f, 0.4f));
			if (DynMat) SelectionRingMesh->SetMaterial(0, DynMat);
		}
		else if (bIsHovered)
		{
			UMaterialInstanceDynamic* DynMat = UDominionTextureFactory::CreateDominionMaterial(this, FLinearColor(0.3f, 0.8f, 1.0f), 0.8f, 0.2f, FLinearColor(1.2f, 2.5f, 3.0f));
			if (DynMat) SelectionRingMesh->SetMaterial(0, DynMat);
		}
	}
	if (OverheadStatusText)
	{
		OverheadStatusText->SetVisibility(bIsSelected || bIsHovered || DamageFeedbackTimer > 0.0f || CombatState == EDominionCombatState::RoutingPanic);
	}
}

void ADominionUnitActor::SetHovered(bool bNewHovered)
{
	if (bIsHovered == bNewHovered) return;
	bIsHovered = bNewHovered;
	if (SelectionRingMesh)
	{
		SelectionRingMesh->SetVisibility(bIsSelected || bIsHovered);
		if (bIsSelected)
		{
			UMaterialInstanceDynamic* DynMat = UDominionTextureFactory::CreateDominionMaterial(this, FLinearColor(1.0f, 0.82f, 0.18f), 0.9f, 0.2f, FLinearColor(3.0f, 2.2f, 0.4f));
			if (DynMat) SelectionRingMesh->SetMaterial(0, DynMat);
		}
		else if (bIsHovered)
		{
			UMaterialInstanceDynamic* DynMat = UDominionTextureFactory::CreateDominionMaterial(this, FLinearColor(0.3f, 0.8f, 1.0f), 0.8f, 0.2f, FLinearColor(1.2f, 2.5f, 3.0f));
			if (DynMat) SelectionRingMesh->SetMaterial(0, DynMat);
		}
	}
	if (OverheadStatusText)
	{
		OverheadStatusText->SetVisibility(bIsSelected || bIsHovered || DamageFeedbackTimer > 0.0f || CombatState == EDominionCombatState::RoutingPanic);
	}
}

void ADominionUnitActor::MoveToLocation(const FVector& Destination)
{
	if (CombatState == EDominionCombatState::RoutingPanic)
	{
		return; // Ignore orders while broken
	}

	TargetDestination = FVector(Destination.X, Destination.Y, GetActorLocation().Z);
	bHasDestination = true;
	CurrentTargetUnit = nullptr;
	CombatState = EDominionCombatState::Marching;
}

void ADominionUnitActor::AttackTarget(ADominionUnitActor* Target)
{
	if (CombatState == EDominionCombatState::RoutingPanic)
	{
		return; // Ignore attack orders while broken
	}

	CurrentTargetUnit = Target;
	if (Target)
	{
		MoveToLocation(Target->GetActorLocation());
	}
}

void ADominionUnitActor::TakeCombatDamage(float DamageAmount, float ArmorPiercing, ADominionUnitActor* Attacker, FVector HitDirection)
{
	float EffectiveArmor = FMath::Max(0.0f, Armor - ArmorPiercing);
	float MitigatedDamage = FMath::Max(2.0f, DamageAmount - (EffectiveArmor * 0.5f));
	Health -= MitigatedDamage;

	LastDamageTaken = MitigatedDamage;
	DamageFeedbackTimer = 1.2f;

	// Visual Hit Spark VFX
	if (UWorld* World = GetWorld())
	{
		DrawDebugPoint(World, GetActorLocation() + FVector(0.0f, 0.0f, 60.0f), 12.0f, (TeamID == 0) ? FColor(80, 180, 255) : FColor(255, 70, 50), false, 0.35f);
		DrawDebugString(World, GetActorLocation() + FVector(0.0f, 0.0f, 100.0f), FString::Printf(TEXT("-%d"), FMath::RoundToInt(MitigatedDamage)), nullptr, FColor(255, 230, 80), 0.6f, true, 1.2f);
	}

	// US-3.4: Lightweight Morale & Rout Trigger
	if (Health > 0.0f && Health <= (MaxHealth * 0.30f) && CombatState != EDominionCombatState::RoutingPanic)
	{
		if (FMath::FRand() < 0.45f)
		{
			CombatState = EDominionCombatState::RoutingPanic;
			RoutTimer = 4.5f;
			Morale = 0.0f;
			MoveSpeed = BaseMoveSpeed * 1.30f; // Panic flee speed bonus
			CurrentTargetUnit = nullptr;

			// Flee away from attackers toward perimeter
			FVector FleeDir = (Attacker) ? (GetActorLocation() - Attacker->GetActorLocation()).GetSafeNormal2D() : FVector(0.0f, -1.0f, 0.0f);
			TargetDestination = GetActorLocation() + FleeDir * 900.0f;
			bHasDestination = true;

			if (UWorld* World = GetWorld())
			{
				DrawDebugString(World, GetActorLocation() + FVector(0.0f, 0.0f, 130.0f), TEXT("ROUT!"), nullptr, FColor(255, 40, 40), 1.2f, true, 1.6f);
			}
		}
	}

	if (Health <= 0.0f)
	{
		Health = 0.0f;
		Destroy();
	}
}

