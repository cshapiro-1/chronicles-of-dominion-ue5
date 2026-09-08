#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DominionTypes.h"
#include "DominionUnitActor.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;
class UDecalComponent;
class UTextRenderComponent;

UENUM(BlueprintType)
enum class EDominionUnitType : uint8
{
	BronzeSpearman UMETA(DisplayName = "Bronze Spearman"),
	HeavyChariot   UMETA(DisplayName = "Heavy War Chariot"),
	OxCartSupply   UMETA(DisplayName = "Ox-Cart Baggage Train"),
	CatapultSiege  UMETA(DisplayName = "Siege Catapult"),
	WorkerPeasant  UMETA(DisplayName = "Canal Worker")
};

UENUM(BlueprintType)
enum class EDominionCombatState : uint8
{
	Idle,
	Marching,
	InCombat,
	RoutingPanic
};

/**
 * ADominionUnitActor
 * High-fidelity 3D RTS Unit Actor with Selection Decal, overhead status bars,
 * Formation physics momentum, and real-time path marching.
 */
UCLASS()
class DOMINIONCORE_API ADominionUnitActor : public AActor
{
	GENERATED_BODY()

public:
	ADominionUnitActor();

	virtual void Tick(float DeltaTime) override;

	/** Select/Deselect visual feedback */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Unit")
	void SetSelected(bool bNewSelected);

	/** Issue movement command to terrain target */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Unit")
	void MoveToLocation(const FVector& Destination);

	/** Move to specific slot within a marching cohort with orientation lock */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Unit")
	void MoveToFormationSlot(const FVector& SlotTarget, const FRotator& TargetHeading);

	/** Issue attack target */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Unit")
	void AttackTarget(ADominionUnitActor* Target);

	/** Apply damage with directional shield blocking, armor, and bracing reflection */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Unit")
	void TakeCombatDamage(float DamageAmount, float ArmorPiercing, ADominionUnitActor* Attacker = nullptr, FVector HitDirection = FVector::ZeroVector);

	/** Update Starvation & Attrition state */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Unit")
	void SetStarvingState(bool bNewStarving);

	// --- Unit Properties ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	FString UnitName = TEXT("Bronze Phalanx Spearman");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	EDominionUnitType UnitType = EDominionUnitType::BronzeSpearman;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	int32 TeamID = 0; // 0 = Player (Blue), 1 = Enemy AI (Red)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	float Health = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	float MaxHealth = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	float Morale = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	float AttackPower = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	float Armor = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	float MoveSpeed = 380.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	float BaseMoveSpeed = 380.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	bool bIsSelected = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	bool bInShieldWall = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	bool bIsStarving = false;

	/** Haversack Field Rations (Local buffer in seconds, baseline 180s = 3 minutes of tactical autonomy away from ox-carts) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	float FieldRations = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	float MaxFieldRations = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	bool bIsResupplying = false;

	/** Replenishes field haversack rations when within range of Ox-Cart or Marching Camp */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Unit")
	void ReplenishFieldRations(float Amount);

	/** Consumes field rations when detached from supply lines */
	UFUNCTION(BlueprintCallable, Category = "Dominion|Unit")
	void ConsumeFieldRations(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	FVector FormationSlotTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	FRotator DesiredHeading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	EDominionCombatState CombatState = EDominionCombatState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> UnitMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> HelmetMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PlumeMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ShieldMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ShieldBossMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> SpearheadMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MountLeftMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MountRightMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> CrewMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> SelectionRingMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> OverheadStatusText;

protected:
	virtual void BeginPlay() override;

private:
	FVector TargetDestination;
	bool bHasDestination = false;

	UPROPERTY()
	TObjectPtr<ADominionUnitActor> CurrentTargetUnit;

	float AttackCooldownTimer = 0.0f;
};
