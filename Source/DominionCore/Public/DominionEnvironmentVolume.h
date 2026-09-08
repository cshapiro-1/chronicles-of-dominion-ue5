#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/PostProcessComponent.h"
#include "DominionEnvironmentVolume.generated.h"

/**
 * Photorealistic Lighting & Atmospheric Environment Actor for Chronicles of Dominion.
 * Sets up warm Mesopotamian golden-hour directional illumination, volumetric dust fog,
 * sky atmosphere scattering, and Lumen cinematic post-processing.
 */
UCLASS()
class DOMINIONCORE_API ADominionEnvironmentVolume : public AActor
{
	GENERATED_BODY()

public:
	ADominionEnvironmentVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
	UDirectionalLightComponent* SunLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
	USkyLightComponent* SkyLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
	USkyAtmosphereComponent* SkyAtmosphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Atmosphere")
	UExponentialHeightFogComponent* HeightFog;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PostProcess")
	UPostProcessComponent* PostProcessComp;

protected:
	virtual void BeginPlay() override;
};
