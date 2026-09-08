#include "DominionEnvironmentVolume.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/PostProcessComponent.h"
#include "Engine/World.h"

ADominionEnvironmentVolume::ADominionEnvironmentVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	// 1. Directional Sun Light (Warm 5200K Mesopotamian Golden Hour Sun)
	SunLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
	SunLight->SetupAttachment(RootComponent);
	SunLight->SetMobility(EComponentMobility::Movable);
	SunLight->SetRelativeRotation(FRotator(-48.0f, 135.0f, 0.0f)); // Low dramatic RTS sun angle
	SunLight->SetIntensity(65000.0f); // 65,000 Lux
	SunLight->SetLightColor(FLinearColor(1.0f, 0.88f, 0.72f)); // Warm Solar Ochre
	SunLight->bUseTemperature = true;
	SunLight->Temperature = 5200.0f;
	SunLight->bEnableLightShaftBloom = true;
	SunLight->bEnableLightShaftOcclusion = true;
	SunLight->CastShadows = true;
	SunLight->DynamicShadowDistanceMovableLight = 25000.0f;
	SunLight->DynamicShadowCascades = 4;
	SunLight->CascadeDistributionExponent = 3.0f;
	SunLight->bAtmosphereSunLight = true;
	SunLight->AtmosphereSunLightIndex = 0;

	// 2. Sky Light for Ambient Desert Bounces
	SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
	SkyLight->SetupAttachment(RootComponent);
	SkyLight->SetMobility(EComponentMobility::Movable);
	SkyLight->SetIntensity(2.2f);
	SkyLight->SetLightColor(FLinearColor(0.78f, 0.86f, 0.98f));
	SkyLight->bRealTimeCapture = true;
	SkyLight->CastShadows = true;

	// 3. Sky Atmosphere Component for Rayleigh & Mie Dust Scattering
	SkyAtmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
	SkyAtmosphere->SetupAttachment(RootComponent);
	SkyAtmosphere->RayleighScatteringScale = 0.045f;
	SkyAtmosphere->RayleighScattering = FLinearColor(0.14f, 0.35f, 0.75f);
	SkyAtmosphere->MieScatteringScale = 0.008f;
	SkyAtmosphere->MieAbsorptionScale = 0.002f;
	SkyAtmosphere->MieAnisotropy = 0.82f;

	// 4. Exponential Height Fog with Volumetric Dust God Rays
	HeightFog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("HeightFog"));
	HeightFog->SetupAttachment(RootComponent);
	HeightFog->SetMobility(EComponentMobility::Movable);
	HeightFog->SetFogDensity(0.012f);
	HeightFog->SetFogHeightFalloff(0.0025f);
	HeightFog->SetFogInscatteringColor(FLinearColor(0.88f, 0.72f, 0.48f));
	HeightFog->bEnableVolumetricFog = true;
	HeightFog->VolumetricFogScatteringDistribution = 0.48f; // Forward dust scattering
	HeightFog->VolumetricFogExtinctionScale = 0.04f;
	HeightFog->VolumetricFogDistance = 28000.0f;
	HeightFog->VolumetricFogAlbedo = FColor(235, 195, 140);

	// 5. Cinematic Post Process Volume (Lumen GI, Reflections & Color Grading)
	PostProcessComp = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComp"));
	PostProcessComp->SetupAttachment(RootComponent);
	PostProcessComp->bUnbound = true; // Infinite extent
	PostProcessComp->Priority = 100.0f;

	// Post Process Settings Configuration
	FPostProcessSettings& Settings = PostProcessComp->Settings;

	// Lumen Global Illumination & Reflections
	Settings.bOverride_DynamicGlobalIlluminationMethod = true;
	Settings.DynamicGlobalIlluminationMethod = EDynamicGlobalIlluminationMethod::Lumen;
	Settings.bOverride_LumenSurfaceCacheResolution = true;
	Settings.LumenSurfaceCacheResolution = 1.0f;

	Settings.bOverride_ReflectionMethod = true;
	Settings.ReflectionMethod = EReflectionMethod::Lumen;
	Settings.bOverride_LumenReflectionQuality = true;
	Settings.LumenReflectionQuality = 1.0f;

	// Bloom & Lens Flare for Solar Shimmer
	Settings.bOverride_BloomIntensity = true;
	Settings.BloomIntensity = 0.42f;
	Settings.bOverride_BloomThreshold = true;
	Settings.BloomThreshold = 1.1f;

	// Ambient Occlusion for Depth Crevices
	Settings.bOverride_AmbientOcclusionIntensity = true;
	Settings.AmbientOcclusionIntensity = 0.85f;
	Settings.bOverride_AmbientOcclusionRadius = true;
	Settings.AmbientOcclusionRadius = 120.0f;

	// Cinematic Color Grading (Warm Desert Sun Contrast)
	Settings.bOverride_WhiteTemp = true;
	Settings.WhiteTemp = 5500.0f;
	Settings.bOverride_ColorSaturation = true;
	Settings.ColorSaturation = FVector4(1.10f, 1.08f, 1.05f, 1.0f);
	Settings.bOverride_ColorContrast = true;
	Settings.ColorContrast = FVector4(1.14f, 1.12f, 1.08f, 1.0f);
	Settings.bOverride_ColorGamma = true;
	Settings.ColorGamma = FVector4(0.98f, 0.98f, 0.99f, 1.0f);

	// Subtle Vignette
	Settings.bOverride_VignetteIntensity = true;
	Settings.VignetteIntensity = 0.24f;

	// Auto Exposure / EV100 tuning for crisp RTS framing
	Settings.bOverride_AutoExposureMethod = true;
	Settings.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;
	Settings.bOverride_AutoExposureBias = true;
	Settings.AutoExposureBias = 0.2f;
}

void ADominionEnvironmentVolume::BeginPlay()
{
	Super::BeginPlay();
}
