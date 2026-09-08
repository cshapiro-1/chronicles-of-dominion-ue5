#include "DominionUnitActor.h"
#include "DominionTextureFactory.h"
#include "DominionFormationSystem.h"
#include "DominionPoliticalEstatesSystem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
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
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MannyFinder(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny"));

	UStaticMesh* CylinderMesh = CylinderFinder.Succeeded() ? CylinderFinder.Object : nullptr;
	UStaticMesh* SphereMesh = SphereFinder.Succeeded() ? SphereFinder.Object : nullptr;
	UStaticMesh* CubeMesh = CubeFinder.Succeeded() ? CubeFinder.Object : nullptr;
	UStaticMesh* ConeMesh = ConeFinder.Succeeded() ? ConeFinder.Object : nullptr;
	USkeletalMesh* MannyMesh = MannyFinder.Succeeded() ? MannyFinder.Object : nullptr;

	// Root Unit Torso / Chassis
	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));
	RootComponent = UnitMesh;
	UnitMesh->SetCollisionProfileName(TEXT("Pawn"));
	UnitMesh->SetMobility(EComponentMobility::Movable);
	if (CylinderMesh)
	{
		UnitMesh->SetStaticMesh(CylinderMesh);
		UnitMesh->SetRelativeScale3D(FVector(0.55f, 0.55f, 1.1f));
	}

	// High-Fidelity 3D Skeletal Mesh Warrior (Photorealistic UE5 Character)
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (MannyMesh)
	{
		SkeletalMesh->SetSkeletalMesh(MannyMesh);
		SkeletalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		SkeletalMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		SkeletalMesh->SetRelativeScale3D(FVector(0.95f, 0.95f, 0.95f));
		UnitMesh->SetVisibility(false); // Hide the prototype geometric cylinder
	}

	// 1. Bronze Helmet
	HelmetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HelmetMesh"));
	HelmetMesh->SetupAttachment(RootComponent);
	HelmetMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 62.0f));
	HelmetMesh->SetRelativeScale3D(FVector(0.48f, 0.48f, 0.42f));
	HelmetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (SphereMesh)
	{
		HelmetMesh->SetStaticMesh(SphereMesh);
	}

	// 2. Horsehair Helmet Crest / Plume
	PlumeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlumeMesh"));
	PlumeMesh->SetupAttachment(HelmetMesh);
	PlumeMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 22.0f));
	PlumeMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	PlumeMesh->SetRelativeScale3D(FVector(0.18f, 0.45f, 0.35f));
	PlumeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (ConeMesh)
	{
		PlumeMesh->SetStaticMesh(ConeMesh);
	}

	// 3. Greek/Mesopotamian Aspis Shield on Left Arm
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(RootComponent);
	ShieldMesh->SetRelativeLocation(FVector(0.0f, -32.0f, 0.0f));
	ShieldMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
	ShieldMesh->SetRelativeScale3D(FVector(0.58f, 0.58f, 0.06f));
	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CylinderMesh)
	{
		ShieldMesh->SetStaticMesh(CylinderMesh);
	}

	// 4. Golden Center Boss / Emblem on Shield
	ShieldBossMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldBossMesh"));
	ShieldBossMesh->SetupAttachment(ShieldMesh);
	ShieldBossMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 8.0f));
	ShieldBossMesh->SetRelativeScale3D(FVector(0.28f, 0.28f, 0.18f));
	ShieldBossMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (SphereMesh)
	{
		ShieldBossMesh->SetStaticMesh(SphereMesh);
	}

	// 5. Long Cedar Spear Shaft
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetRelativeLocation(FVector(20.0f, 28.0f, 25.0f));
	WeaponMesh->SetRelativeRotation(FRotator(20.0f, 0.0f, 0.0f));
	WeaponMesh->SetRelativeScale3D(FVector(0.04f, 0.04f, 3.2f));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CylinderMesh)
	{
		WeaponMesh->SetStaticMesh(CylinderMesh);
	}

	// 6. Shining Bronze Spearhead Tip
	SpearheadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpearheadMesh"));
	SpearheadMesh->SetupAttachment(WeaponMesh);
	SpearheadMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 52.0f));
	SpearheadMesh->SetRelativeScale3D(FVector(2.8f, 2.8f, 0.35f));
	SpearheadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (ConeMesh)
	{
		SpearheadMesh->SetStaticMesh(ConeMesh);
	}

	// 7. Mounts / Spoke Wheels (for Chariots and Carts)
	MountLeftMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MountLeftMesh"));
	MountLeftMesh->SetupAttachment(RootComponent);
	MountLeftMesh->SetRelativeLocation(FVector(0.0f, -48.0f, -25.0f));
	MountLeftMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	MountLeftMesh->SetRelativeScale3D(FVector(0.55f, 0.55f, 0.1f));
	MountLeftMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MountLeftMesh->SetVisibility(false);
	if (CylinderMesh)
	{
		MountLeftMesh->SetStaticMesh(CylinderMesh);
	}

	MountRightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MountRightMesh"));
	MountRightMesh->SetupAttachment(RootComponent);
	MountRightMesh->SetRelativeLocation(FVector(0.0f, 48.0f, -25.0f));
	MountRightMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	MountRightMesh->SetRelativeScale3D(FVector(0.55f, 0.55f, 0.1f));
	MountRightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MountRightMesh->SetVisibility(false);
	if (CylinderMesh)
	{
		MountRightMesh->SetStaticMesh(CylinderMesh);
	}

	// 8. Crew Figure / Cargo Load
	CrewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrewMesh"));
	CrewMesh->SetupAttachment(RootComponent);
	CrewMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
	CrewMesh->SetRelativeScale3D(FVector(0.45f, 0.45f, 0.8f));
	CrewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CrewMesh->SetVisibility(false);
	if (CylinderMesh)
	{
		CrewMesh->SetStaticMesh(CylinderMesh);
	}

	// 9. Selection Ring on Ground
	SelectionRingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionRingMesh"));
	SelectionRingMesh->SetupAttachment(RootComponent);
	SelectionRingMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -55.0f));
	SelectionRingMesh->SetRelativeScale3D(FVector(1.4f, 1.4f, 0.04f));
	SelectionRingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SelectionRingMesh->SetVisibility(false);
	if (CylinderMesh)
	{
		SelectionRingMesh->SetStaticMesh(CylinderMesh);
	}

	// 10. Overhead Billboard Display (Hidden by default for clean RTS visual fidelity)
	OverheadStatusText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("OverheadStatusText"));
	OverheadStatusText->SetupAttachment(RootComponent);
	OverheadStatusText->SetVisibility(false);
}

void ADominionUnitActor::BeginPlay()
{
	Super::BeginPlay();

	TargetDestination = GetActorLocation();

	// High-Fidelity Historical PBR Materials
	const FLinearColor TeamColor = (TeamID == 0) ? FLinearColor(0.12f, 0.45f, 0.95f) : FLinearColor(0.88f, 0.15f, 0.15f);
	const FLinearColor PolishedBronze(0.92f, 0.65f, 0.28f);
	const FLinearColor DarkCedarWood(0.38f, 0.22f, 0.12f);
	const FLinearColor SolarGold(1.0f, 0.82f, 0.18f);
	const FLinearColor HorsehairBlack(0.08f, 0.08f, 0.08f);
	const FLinearColor LinenWhite(0.92f, 0.88f, 0.82f);
	const FLinearColor PlumeColor = (TeamID == 0) ? SolarGold : FLinearColor(0.95f, 0.15f, 0.15f);

	if (UnitType == EDominionUnitType::HeavyChariot)
	{
		UnitMesh->SetRelativeScale3D(FVector(1.8f, 1.2f, 0.6f));
		MountLeftMesh->SetVisibility(true);
		MountRightMesh->SetVisibility(true);
		CrewMesh->SetVisibility(true);
		PlumeMesh->SetVisibility(true);
		ShieldMesh->SetRelativeLocation(FVector(20.0f, -44.0f, 10.0f));
		WeaponMesh->SetRelativeRotation(FRotator(10.0f, 0.0f, 0.0f));
		WeaponMesh->SetRelativeScale3D(FVector(0.06f, 0.06f, 3.4f));
	}
	else if (UnitType == EDominionUnitType::OxCartSupply)
	{
		UnitMesh->SetRelativeScale3D(FVector(2.0f, 1.3f, 0.7f));
		MountLeftMesh->SetVisibility(true);
		MountRightMesh->SetVisibility(true);
		CrewMesh->SetVisibility(true); // Grain sacks
		CrewMesh->SetRelativeScale3D(FVector(1.2f, 0.9f, 0.5f));
		ShieldMesh->SetVisibility(false);
		HelmetMesh->SetVisibility(false);
		PlumeMesh->SetVisibility(false);
		WeaponMesh->SetVisibility(false);
	}

	// Procedural PBR Textures
	static UTexture2D* BronzeAlbedo = UDominionTextureFactory::CreateHammeredBronzeAlbedo(512, 512);
	static UTexture2D* BronzeNormal = UDominionTextureFactory::CreateHammeredBronzeNormal(512, 512);
	static UTexture2D* CedarAlbedo = UDominionTextureFactory::CreateCedarWoodAlbedo(512, 512);

	// Dynamic PBR Setup with Metallic & Roughness & Texture Maps
	auto ApplyDynPBR = [this](UStaticMeshComponent* Comp, const FLinearColor& Color, float Metallic = 0.0f, float Roughness = 0.6f, const FLinearColor& EmissiveColor = FLinearColor::Black)
	{
		if (Comp)
		{
			UMaterialInstanceDynamic* DynMat = UDominionTextureFactory::CreateDominionMaterial(this, Color, Metallic, Roughness, EmissiveColor);
			if (DynMat)
			{
				Comp->SetMaterial(0, DynMat);
			}
		}
	};

	ApplyDynPBR(UnitMesh, TeamColor, 0.1f, 0.5f);
	ApplyDynPBR(HelmetMesh, PolishedBronze, 0.95f, 0.22f);
	ApplyDynPBR(PlumeMesh, PlumeColor, 0.0f, 0.85f);
	ApplyDynPBR(ShieldMesh, TeamColor, 0.3f, 0.4f);
	ApplyDynPBR(ShieldBossMesh, SolarGold, 0.98f, 0.18f);
	ApplyDynPBR(WeaponMesh, DarkCedarWood, 0.0f, 0.75f);
	ApplyDynPBR(SpearheadMesh, PolishedBronze, 0.95f, 0.20f);
	ApplyDynPBR(MountLeftMesh, DarkCedarWood, 0.0f, 0.80f);
	ApplyDynPBR(MountRightMesh, DarkCedarWood, 0.0f, 0.80f);
	ApplyDynPBR(CrewMesh, (UnitType == EDominionUnitType::OxCartSupply) ? LinenWhite : TeamColor, 0.0f, 0.8f);
	ApplyDynPBR(SelectionRingMesh, SolarGold, 0.8f, 0.2f, FLinearColor(2.0f, 1.6f, 0.2f));

	// Apply Dynamic Material Tint to 3D Skeletal Mesh Warrior
	if (SkeletalMesh)
	{
		for (int32 m = 0; m < SkeletalMesh->GetNumMaterials(); ++m)
		{
			UMaterialInstanceDynamic* DynMat = SkeletalMesh->CreateAndSetMaterialInstanceDynamic(m);
			if (DynMat)
			{
				DynMat->SetVectorParameterValue(TEXT("BaseColor"), TeamColor);
				DynMat->SetVectorParameterValue(TEXT("Color"), TeamColor);
				DynMat->SetVectorParameterValue(TEXT("BodyColor"), TeamColor);
				DynMat->SetScalarParameterValue(TEXT("Metallic"), 0.35f);
				DynMat->SetScalarParameterValue(TEXT("Roughness"), 0.45f);
			}
		}
	}
}

void ADominionUnitActor::MoveToFormationSlot(const FVector& SlotTarget, const FRotator& TargetHeading)
{
	FormationSlotTarget = SlotTarget;
	DesiredHeading = TargetHeading;
	MoveToLocation(SlotTarget);
}

void ADominionUnitActor::SetStarvingState(bool bNewStarving)
{
	bIsStarving = bNewStarving;
	if (bIsStarving)
	{
		if (OverheadStatusText)
		{
			OverheadStatusText->SetText(FText::FromString(TEXT("[!] STARVING - ATTRITION")));
			OverheadStatusText->SetTextRenderColor(FColor(255, 130, 40));
		}
	}
	else
	{
		if (OverheadStatusText && CombatState != EDominionCombatState::InCombat)
		{
			FString StatusStr = FString::Printf(TEXT("%s | HP: %d/%d"), *UnitName, FMath::RoundToInt(Health), FMath::RoundToInt(MaxHealth));
			OverheadStatusText->SetText(FText::FromString(StatusStr));
			OverheadStatusText->SetTextRenderColor((TeamID == 0) ? FColor(255, 215, 0) : FColor(255, 60, 60));
		}
	}
}

void ADominionUnitActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float WorldTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	// Starvation & Supply Line Attrition
	if (bIsStarving && TeamID == 0 && UnitType != EDominionUnitType::OxCartSupply)
	{
		// 1. Health attrition: lose 1.5% max health per second
		Health = FMath::Max(1.0f, Health - (MaxHealth * 0.015f * DeltaTime));

		// 2. Morale decay: lose 5 morale per second
		Morale = FMath::Clamp(Morale - (5.0f * DeltaTime), 0.0f, 100.0f);

		// 3. Mutiny / Routing Panic if starved below 20 morale
		if (Morale <= 20.0f && CombatState != EDominionCombatState::RoutingPanic)
		{
			CombatState = EDominionCombatState::RoutingPanic;
			MoveSpeed = BaseMoveSpeed * 1.35f; // Scatter in panic
		}

		if (OverheadStatusText)
		{
			FString StarveStr = FString::Printf(TEXT("[!] STARVING (%d%% MORALE) | HP: %d"), FMath::RoundToInt(Morale), FMath::RoundToInt(Health));
			OverheadStatusText->SetText(FText::FromString(StarveStr));
			OverheadStatusText->SetTextRenderColor(FColor(255, 80, 20));
		}
	}
	else if (TeamID == 0 && UnitType != EDominionUnitType::OxCartSupply)
	{
		// Hope & Estates Morale Modulation (Player Army)
		if (UDominionPoliticalEstatesSystem* Estates = GetWorld() ? GetWorld()->GetSubsystem<UDominionPoliticalEstatesSystem>() : nullptr)
		{
			if (Estates->GetHope() > 70.0f)
			{
				Morale = FMath::Min(100.0f, Morale + (1.2f * DeltaTime)); // High Hope Morale Boost
			}
			else if (Estates->GetHope() < 20.0f)
			{
				Morale = FMath::Max(15.0f, Morale - (2.0f * DeltaTime)); // Imperial Despair decay
			}
		}
	}

	// Movement Execution with Dynamic Formation Cohesion
	if (bHasDestination)
	{
		FVector CurrentLoc = GetActorLocation();
		FVector Direction = (TargetDestination - CurrentLoc);
		Direction.Z = 0.0f; // Stay on terrain plane

		const float Dist = Direction.Size();
		if (Dist > 25.0f)
		{
			Direction.Normalize();

			// Dynamic Formation Cohesion Speed Steering
			float DynamicSpeed = BaseMoveSpeed;
			if (Dist > 200.0f)
			{
				DynamicSpeed = BaseMoveSpeed * 1.25f; // Catch up to formation
			}
			else if (Dist < 60.0f)
			{
				DynamicSpeed = BaseMoveSpeed * 0.85f; // Settle into slot
			}
			MoveSpeed = DynamicSpeed;

			FVector NewLoc = CurrentLoc + Direction * MoveSpeed * DeltaTime;
			SetActorLocation(NewLoc, true);

			// Rotate smoothly to face movement direction or desired formation heading
			FRotator TargetRot = (Dist < 80.0f && !DesiredHeading.IsZero()) ? DesiredHeading : Direction.Rotation();
			SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 12.0f));
			CombatState = EDominionCombatState::Marching;

			// Procedural Marching Cadence (Stride Bobbing & Weapon Swing)
			const float Stride = FMath::Sin(WorldTime * 9.0f);
			if (HelmetMesh)
			{
				HelmetMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 62.0f + Stride * 3.0f));
			}
			if (WeaponMesh && UnitType == EDominionUnitType::BronzeSpearman)
			{
				WeaponMesh->SetRelativeRotation(FRotator(20.0f + Stride * 12.0f, 0.0f, 0.0f));
			}
			if (ShieldMesh)
			{
				ShieldMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f - Stride * 6.0f));
			}

			// Chariot / Cart Wheel Rolling Physics
			if (MountLeftMesh && MountLeftMesh->IsVisible())
			{
				const float WheelAngle = MoveSpeed * DeltaTime * 3.0f;
				MountLeftMesh->AddLocalRotation(FRotator(WheelAngle, 0.0f, 0.0f));
				MountRightMesh->AddLocalRotation(FRotator(WheelAngle, 0.0f, 0.0f));
			}
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
	else
	{
		// Idle Organic Breathing Motion
		if (CombatState == EDominionCombatState::Idle)
		{
			const float Respiration = FMath::Sin(WorldTime * 2.8f) * 1.5f;
			if (HelmetMesh)
			{
				HelmetMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 62.0f + Respiration));
			}
		}
	}

	// Combat Tick & Spear Thrust Attack Animation
	if (CurrentTargetUnit && IsValid(CurrentTargetUnit))
	{
		float DistToTarget = FVector::Dist(GetActorLocation(), CurrentTargetUnit->GetActorLocation());
		if (DistToTarget <= 260.0f)
		{
			AttackCooldownTimer -= DeltaTime;

			// Spear Thrusting Animation
			if (WeaponMesh && UnitType == EDominionUnitType::BronzeSpearman)
			{
				// Thrust forward during first 0.3s of cooldown
				float ThrustOffset = 0.0f;
				if (AttackCooldownTimer > 0.8f)
				{
					ThrustOffset = (1.2f - AttackCooldownTimer) / 0.4f * 45.0f;
				}
				else if (AttackCooldownTimer > 0.5f)
				{
					ThrustOffset = (AttackCooldownTimer - 0.5f) / 0.3f * 45.0f;
				}
				WeaponMesh->SetRelativeLocation(FVector(20.0f + ThrustOffset, 28.0f, 25.0f));
				WeaponMesh->SetRelativeRotation(FRotator(4.0f, 0.0f, 0.0f)); // Level thrust
			}

			if (AttackCooldownTimer <= 0.0f)
			{
				FVector AttackDir = (CurrentTargetUnit->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				CurrentTargetUnit->TakeCombatDamage(AttackPower, 5.0f, this, AttackDir);
				AttackCooldownTimer = 1.2f; // Attack interval
				CombatState = EDominionCombatState::InCombat;
			}
		}
		else
		{
			// March into melee range
			MoveToLocation(CurrentTargetUnit->GetActorLocation());
		}
	}
}

void ADominionUnitActor::SetSelected(bool bNewSelected)
{
	bIsSelected = bNewSelected;
	if (SelectionRingMesh)
	{
		SelectionRingMesh->SetVisibility(bNewSelected);
	}
}

void ADominionUnitActor::MoveToLocation(const FVector& Destination)
{
	TargetDestination = Destination;
	bHasDestination = true;
	CurrentTargetUnit = nullptr;
}

void ADominionUnitActor::AttackTarget(ADominionUnitActor* Target)
{
	CurrentTargetUnit = Target;
	if (Target)
	{
		MoveToLocation(Target->GetActorLocation());
	}
}

void ADominionUnitActor::TakeCombatDamage(float DamageAmount, float ArmorPiercing, ADominionUnitActor* Attacker, FVector HitDirection)
{
	if (HitDirection.IsNearlyZero() && Attacker && IsValid(Attacker))
	{
		HitDirection = (GetActorLocation() - Attacker->GetActorLocation()).GetSafeNormal();
	}

	// 1. Directional Shield Blocking Analysis
	float FrontDot = 0.0f;
	if (!HitDirection.IsNearlyZero())
	{
		FrontDot = FVector::DotProduct(GetActorForwardVector(), -HitDirection);
	}

	bool bBlocked = false;
	bool bRearFlankCrit = false;

	if (FrontDot > 0.35f)
	{
		// Frontal Attack into Shield
		if (bInShieldWall || UnitType == EDominionUnitType::BronzeSpearman)
		{
			DamageAmount *= 0.25f; // 75% Damage Reduction from Shield Wall!
			bBlocked = true;
		}
	}
	else if (FrontDot < -0.35f)
	{
		// Rear Flank Critical Attack (Shield bypassed)
		DamageAmount *= 1.50f; // +50% Critical Damage on exposed rear
		bRearFlankCrit = true;
	}

	// 2. Spear Bracing Reflection Physics vs Charging Cavalry / Chariots
	if (bInShieldWall && FrontDot > 0.40f && Attacker && IsValid(Attacker))
	{
		// If attacker is a Chariot or high-speed charging attacker
		if (Attacker->UnitType == EDominionUnitType::HeavyChariot || Attacker->MoveSpeed > 300.0f)
		{
			float ReflectedDmg = FMath::Clamp(Attacker->MoveSpeed * 0.18f + 40.0f, 40.0f, 150.0f);
			Attacker->TakeCombatDamage(ReflectedDmg, 15.0f, this, -HitDirection);

			if (OverheadStatusText)
			{
				OverheadStatusText->SetVisibility(true);
				OverheadStatusText->SetText(FText::FromString(FString::Printf(TEXT("[BRACED SPEAR REFLECT: %.0f DMG]"), ReflectedDmg)));
				OverheadStatusText->SetTextRenderColor(FColor(0, 255, 255));
			}

			if (UWorld* World = GetWorld())
			{
				DrawDebugLine(World, GetActorLocation() + FVector(0,0,60), Attacker->GetActorLocation() + FVector(0,0,60), FColor::Cyan, false, 0.6f, 0, 4.0f);
			}
		}
	}

	// 3. Armor Mitigation
	float EffectiveArmor = FMath::Max(0.0f, Armor - ArmorPiercing);
	if (bInShieldWall)
	{
		EffectiveArmor += 8.0f; // Phalanx Shield Wall defense bonus
	}

	float MitigatedDamage = FMath::Max(3.0f, DamageAmount - (EffectiveArmor * 0.75f));
	Health -= MitigatedDamage;

	// 4. Combat Feedback Traces & Overhead Text
	if (UWorld* World = GetWorld())
	{
		FColor SparkCol = bBlocked ? FColor::Yellow : (bRearFlankCrit ? FColor::Orange : FColor::Red);
		DrawDebugPoint(World, GetActorLocation() + FVector(0.0f, 0.0f, 60.0f), 12.0f, SparkCol, false, 0.4f);
	}

	if (OverheadStatusText && !bIsStarving)
	{
		OverheadStatusText->SetVisibility(true);
		FString StatusStr;
		if (bBlocked)
		{
			StatusStr = FString::Printf(TEXT("[BLOCKED! -75%%] -%.0f HP (%d/%d)"), MitigatedDamage, FMath::RoundToInt(Health), FMath::RoundToInt(MaxHealth));
			OverheadStatusText->SetTextRenderColor(FColor(255, 215, 0));
		}
		else if (bRearFlankCrit)
		{
			StatusStr = FString::Printf(TEXT("[FLANK CRIT! +50%%] -%.0f HP (%d/%d)"), MitigatedDamage, FMath::RoundToInt(Health), FMath::RoundToInt(MaxHealth));
			OverheadStatusText->SetTextRenderColor(FColor(255, 60, 20));
		}
		else
		{
			StatusStr = FString::Printf(TEXT("-%.0f HP (%d/%d)"), MitigatedDamage, FMath::RoundToInt(Health), FMath::RoundToInt(MaxHealth));
			OverheadStatusText->SetTextRenderColor(FColor(255, 80, 80));
		}
		OverheadStatusText->SetText(FText::FromString(StatusStr));
	}

	if (Health <= 0.0f)
	{
		Health = 0.0f;
		Destroy();
	}
}
