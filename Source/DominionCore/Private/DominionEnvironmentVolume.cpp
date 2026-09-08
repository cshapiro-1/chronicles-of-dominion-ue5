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

	// 1. Directional Atmospheric Light (Mythic Grimdark Storm Light)
	SunLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
	SunLight->SetupAttachment(RootComponent);
	SunLight->SetMobility(EComponentMobility::Movable);
	SunLight->SetRelativeRotation(FRotator(-38.0f, 130.0f, 0.0f)); // Low dramatic storm angle
	SunLight->SetIntensity(14000.0f); // Balanced for deep night/storm shadows and radiant firelight
	SunLight->SetLightColor(FLinearColor(0.72f, 0.62f, 0.50f));
	SunLight->bUseTemperature = true;
	SunLight->Temperature = 4800.0f;
	SunLight->bEnableLightShaftBloom = true;
	SunLight->bEnableLightShaftOcclusion = true;
	SunLight->CastShadows = true;
	SunLight->DynamicShadowDistanceMovableLight = 30000.0f;
	SunLight->DynamicShadowCascades = 4;
	SunLight->CascadeDistributionExponent = 3.0f;
	SunLight->bAtmosphereSunLight = true;
	SunLight->AtmosphereSunLightIndex = 0;

	// 2. Sky Light for Ambient Storm Bounce
	SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
	SkyLight->SetupAttachment(RootComponent);
	SkyLight->SetMobility(EComponentMobility::Movable);
	SkyLight->SetIntensity(0.75f);
	SkyLight->SetLightColor(FLinearColor(0.10f, 0.14f, 0.22f));
	SkyLight->bRealTimeCapture = true;
	SkyLight->CastShadows = true;

	// 3. Sky Atmosphere Component for Storm Rayleigh & Smoke Scattering
	SkyAtmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
	SkyAtmosphere->SetupAttachment(RootComponent);
	SkyAtmosphere->RayleighScatteringScale = 0.08f;
	SkyAtmosphere->RayleighScattering = FLinearColor(0.08f, 0.18f, 0.45f);
	SkyAtmosphere->MieScatteringScale = 0.015f;
	SkyAtmosphere->MieAbsorptionScale = 0.005f;
	SkyAtmosphere->MieAnisotropy = 0.85f;

	// 4. Exponential Height Fog with Volumetric Storm Mist
	HeightFog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("HeightFog"));
	HeightFog->SetupAttachment(RootComponent);
	HeightFog->SetMobility(EComponentMobility::Movable);
	HeightFog->SetFogDensity(0.022f);
	HeightFog->SetFogHeightFalloff(0.0020f);
	HeightFog->SetFogInscatteringColor(FLinearColor(0.12f, 0.14f, 0.18f));
	HeightFog->bEnableVolumetricFog = true;
	HeightFog->VolumetricFogScatteringDistribution = 0.65f;
	HeightFog->VolumetricFogExtinctionScale = 0.06f;
	HeightFog->VolumetricFogDistance = 30000.0f;
	HeightFog->VolumetricFogAlbedo = FColor(70, 75, 85);

	// 5. Cinematic Post Process Volume (Lumen GI, Reflections & Dark Grimdark Tonemapping)
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

	// Bloom & Lens Flare for Radiant Flame Glow
	Settings.bOverride_BloomIntensity = true;
	Settings.BloomIntensity = 0.65f;
	Settings.bOverride_BloomThreshold = true;
	Settings.BloomThreshold = 0.85f;

	// Ambient Occlusion for Deep Stone Crevices
	Settings.bOverride_AmbientOcclusionIntensity = true;
	Settings.AmbientOcclusionIntensity = 1.15f;
	Settings.bOverride_AmbientOcclusionRadius = true;
	Settings.AmbientOcclusionRadius = 150.0f;

	// Cinematic Color Grading (Mythic Grimdark Bronze Contrast)
	Settings.bOverride_WhiteTemp = true;
	Settings.WhiteTemp = 5800.0f;
	Settings.bOverride_ColorSaturation = true;
	Settings.ColorSaturation = FVector4(1.05f, 1.02f, 0.98f, 1.0f);
	Settings.bOverride_ColorContrast = true;
	Settings.ColorContrast = FVector4(1.22f, 1.18f, 1.12f, 1.0f);
	Settings.bOverride_ColorGamma = true;
	Settings.ColorGamma = FVector4(0.95f, 0.95f, 0.96f, 1.0f);

	// Dramatic Vignette
	Settings.bOverride_VignetteIntensity = true;
	Settings.VignetteIntensity = 0.38f;

	// Auto Exposure tuning for grimdark clarity
	Settings.bOverride_AutoExposureMethod = true;
	Settings.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;
	Settings.bOverride_AutoExposureBias = true;
	Settings.AutoExposureBias = 0.4f;
}

void ADominionEnvironmentVolume::BeginPlay()
{
	Super::BeginPlay();
}
