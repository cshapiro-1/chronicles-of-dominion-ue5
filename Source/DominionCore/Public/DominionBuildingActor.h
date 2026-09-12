#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DominionBuildingActor.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;

UENUM(BlueprintType)
enum class EDominionArchitecturalStyle : uint8
{
	PrimitiveEarthAndMud   UMETA(DisplayName = "Primitive Earth & Mud"),
	AngkorWatHewnStone     UMETA(DisplayName = "Angkor Wat Hewn Stone"),
	DarkEvilGothic         UMETA(DisplayName = "Dark Evil Gothic"),
	VictorianIndustrial    UMETA(DisplayName = "Victorian Industrial Grimdark")
};

UENUM(BlueprintType)
enum class EDominionBuildingType : uint8
{
	ZigguratTownCenter UMETA(DisplayName = "Citadel Town Center"),
	MudbrickGranary    UMETA(DisplayName = "Resource Granary & Vault"),
	BronzeBarracks     UMETA(DisplayName = "War Forge & Barracks"),
	SiltIrrigation     UMETA(DisplayName = "Aqueduct & Canal"),
	TimberCamp         UMETA(DisplayName = "Timber & Quarry Camp")
};

/**
 * ADominionBuildingActor
 * Colossal Interactive RTS 3D Structure with 4-Era architectural style morphing:
 * Primitive Mud -> Angkor Wat Hewn Stone -> Dark Evil Gothic -> Victorian Industrial.
 */
UCLASS()
class DOMINIONCORE_API ADominionBuildingActor : public AActor
{
	GENERATED_BODY()

public:
	ADominionBuildingActor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Dominion|Building")
	void SetSelected(bool bNewSelected);

	UFUNCTION(BlueprintCallable, Category = "Dominion|Building")
	void SetArchitecturalStyle(EDominionArchitecturalStyle NewStyle);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Stats")
	FString BuildingName = TEXT("Citadel of Ur-Kish");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Stats")
	EDominionBuildingType BuildingType = EDominionBuildingType::ZigguratTownCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Stats")
	EDominionArchitecturalStyle ArchitecturalStyle = EDominionArchitecturalStyle::PrimitiveEarthAndMud;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Stats")
	int32 TeamID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Stats")
	float Health = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Stats")
	float MaxHealth = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Stats")
	bool bIsSelected = false;

	// --- 3D Architectural Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BuildingMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> SelectionBoxMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> OverheadNameText;

protected:
	virtual void BeginPlay() override;
	void ApplyCurrentStyleVisuals();
};
