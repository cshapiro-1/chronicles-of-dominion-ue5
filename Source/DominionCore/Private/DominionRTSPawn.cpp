#include "DominionRTSPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"

ADominionRTSPawn::ADominionRTSPawn()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessPlayer = EAutoReceiveInput::Player0;

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    SetRootComponent(RootScene);

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(RootScene);
    SpringArmComponent->bDoCollisionTest = false;
    SpringArmComponent->bInheritPitch = false;
    SpringArmComponent->bInheritRoll = false;
    SpringArmComponent->bInheritYaw = true;
    SpringArmComponent->TargetArmLength = 4500.0f;
    SpringArmComponent->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f));

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("RTSCamera"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->bUsePawnControlRotation = false;

    TargetArmLength = 4500.0f;
    TargetYaw = 0.0f;
}

void ADominionRTSPawn::BeginPlay()
{
    Super::BeginPlay();
    TargetLocation = GetActorLocation();
}

void ADominionRTSPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 1. Smoothly interpolate position
    FVector CurrentLoc = GetActorLocation();
    FVector NewLoc = FMath::VInterpTo(CurrentLoc, TargetLocation, DeltaTime, DampingFactor);
    SetActorLocation(NewLoc);

    // 2. Smoothly interpolate zoom distance
    float CurrentArm = SpringArmComponent->TargetArmLength;
    float NewArm = FMath::FInterpTo(CurrentArm, TargetArmLength, DeltaTime, DampingFactor);
    SpringArmComponent->TargetArmLength = NewArm;

    // 3. Dynamic Exponential Pitch Tilt based on Zoom Level
    float ZoomRatio = GetCurrentZoomPercent();
    float TargetPitch = FMath::Lerp(MinPitchAngle, MaxPitchAngle, ZoomRatio);
    FRotator CurrentRot = SpringArmComponent->GetRelativeRotation();
    FRotator TargetRot = FRotator(TargetPitch, TargetYaw, 0.0f);
    SpringArmComponent->SetRelativeRotation(FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, DampingFactor));
}

void ADominionRTSPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (PlayerInputComponent)
    {
        // WASD & Arrow Key Bindings
        PlayerInputComponent->BindKey(EKeys::W, IE_Pressed, this, &ADominionRTSPawn::InputZoomIn); // Fallback hook
        PlayerInputComponent->BindKey(EKeys::MouseScrollUp, IE_Pressed, this, &ADominionRTSPawn::InputZoomIn);
        PlayerInputComponent->BindKey(EKeys::MouseScrollDown, IE_Pressed, this, &ADominionRTSPawn::InputZoomOut);
        PlayerInputComponent->BindKey(EKeys::Q, IE_Pressed, this, &ADominionRTSPawn::InputRotateLeft);
        PlayerInputComponent->BindKey(EKeys::E, IE_Pressed, this, &ADominionRTSPawn::InputRotateRight);
    }
}

void ADominionRTSPawn::InputMoveForward(float Value)
{
    if (FMath::Abs(Value) > 0.01f)
    {
        MoveCamera(FVector2D(0.0f, Value));
    }
}

void ADominionRTSPawn::InputMoveRight(float Value)
{
    if (FMath::Abs(Value) > 0.01f)
    {
        MoveCamera(FVector2D(Value, 0.0f));
    }
}

void ADominionRTSPawn::InputZoomIn()
{
    ZoomCamera(1.0f);
}

void ADominionRTSPawn::InputZoomOut()
{
    ZoomCamera(-1.0f);
}

void ADominionRTSPawn::InputRotateLeft()
{
    RotateCamera(-15.0f);
}

void ADominionRTSPawn::InputRotateRight()
{
    RotateCamera(15.0f);
}

void ADominionRTSPawn::MoveCamera(FVector2D Axis)
{
    FRotator YawRot = FRotator(0.0f, TargetYaw, 0.0f);
    FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

    float CurrentZoomMultiplier = FMath::Lerp(0.3f, 2.5f, GetCurrentZoomPercent());
    float Dt = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.016f;
    TargetLocation += (Forward * Axis.Y + Right * Axis.X) * PanSpeed * CurrentZoomMultiplier * Dt;
}

void ADominionRTSPawn::ZoomCamera(float Delta)
{
    TargetArmLength = FMath::Clamp(TargetArmLength - (Delta * ZoomSpeed), MinTargetArmLength, MaxTargetArmLength);
}

void ADominionRTSPawn::RotateCamera(float DeltaDegrees)
{
    TargetYaw = FMath::Fmod(TargetYaw + DeltaDegrees, 360.0f);
}

float ADominionRTSPawn::GetCurrentZoomPercent() const
{
    return FMath::Clamp((TargetArmLength - MinTargetArmLength) / (MaxTargetArmLength - MinTargetArmLength), 0.0f, 1.0f);
}
