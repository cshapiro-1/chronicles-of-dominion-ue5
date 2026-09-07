#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DominionGameModeBase.generated.h"

class ADominionRTSPawn;
class ADominionRTSPlayerController;
class ADominionRTSHUD;

/**
 * ADominionGameModeBase
 * Master GameMode binding PlayerController, RTS Camera Rig, AoE2 HUD,
 * and Dominion Simulation Subsystems in Unreal Engine 5.
 */
UCLASS()
class DOMINIONCORE_API ADominionGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    ADominionGameModeBase();

    virtual void StartPlay() override;
};


