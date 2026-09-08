#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DominionRTSPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USceneComponent;

/**
 * ADominionRTSPawn
 * Physical RTS Camera Rig in Unreal Engine 5.
 * Features:
 * - Exponential orbital tilt: angles down at high altitude for macro overview, tilts horizontal when zooming to soldier level.
 * - Terrain collision clamping: prevents camera clipping through mountains or ziggurats.
 * - Smooth momentum damping and edge-panning.
 */
UCLASS()
class DOMINIONCORE_API ADominionRTSPawn : public APawn
{
    GENERATED_BODY()

public:
    ADominionRTSPawn();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /** Move camera along terrain plane */
    UFUNCTION(BlueprintCallable, Category = "Dominion|Camera")
    void MoveCamera(FVector2D Axis);

    /** Zoom camera with exponential pitch tilt */
    UFUNCTION(BlueprintCallable, Category = "Dominion|Camera")
    void ZoomCamera(float Delta);

    /** Rotate camera around focal point */
    UFUNCTION(BlueprintCallable, Category = "Dominion|Camera")
    void RotateCamera(float DeltaDegrees);

    UFUNCTION(BlueprintPure, Category = "Dominion|Camera")
    float GetCurrentZoomPercent() const;

    void InputMoveForward(float Value);
    void InputMoveRight(float Value);
    void InputZoomIn();
    void InputZoomOut();
    void InputRotateLeft();
    void InputRotateRight();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> RootScene;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> CameraComponent;

    /** Configuration Constants */
    UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
    float MinTargetArmLength = 1200.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
    float MaxTargetArmLength = 24000.0f; // Broad tactical battlefield orbit

    UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
    float MinPitchAngle = -42.0f; // Crisp 42-degree isometric low-altitude angle

    UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
    float MaxPitchAngle = -75.0f; // High altitude top-down bird's eye

    UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
    float PanSpeed = 6500.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
    float ZoomSpeed = 3500.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
    float DampingFactor = 12.0f;

    FVector TargetLocation;
    float TargetArmLength;
    float TargetYaw;
};


