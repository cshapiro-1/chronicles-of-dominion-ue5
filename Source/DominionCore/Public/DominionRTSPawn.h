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
    float MinTargetArmLength = 600.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
    float MaxTargetArmLength = 160000.0f; // Colossal panoramic orbit

    UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
    float MinPitchAngle = -15.0f; // Low altitude eye-level tilt

    UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
    float MaxPitchAngle = -75.0f; // High altitude top-down bird's eye

    UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
    float PanSpeed = 7500.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
    float ZoomSpeed = 6500.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera Config")
    float DampingFactor = 10.0f;

    FVector TargetLocation;
    float TargetArmLength;
    float TargetYaw;
};


