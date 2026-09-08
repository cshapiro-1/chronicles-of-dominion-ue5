#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DominionTextureFactory.generated.h"

/**
 * Procedural PBR Texture and Dynamic Material Factory for Chronicles of Dominion.
 * Generates high-fidelity diffuse, normal, roughness, and detail maps at runtime
 * to ensure photorealistic Mesopotamian Bronze Age visual parity.
 */
UCLASS()
class DOMINIONCORE_API UDominionTextureFactory : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Generates a 512x512 procedural Sandstone Silt Albedo texture with natural mineral noise
	static UTexture2D* CreateSandstoneSiltAlbedo(int32 Width = 512, int32 Height = 512);

	// Generates a 512x512 procedural Sand Dune & Silt Normal bump map
	static UTexture2D* CreateSandstoneSiltNormal(int32 Width = 512, int32 Height = 512);

	// Generates a 512x512 procedural River Water Wave Normal map
	static UTexture2D* CreateWaterWaveNormal(int32 Width = 512, int32 Height = 512);

	// Generates a 512x512 procedural Mudbrick Masonry Albedo & Normal map
	static UTexture2D* CreateMudbrickAlbedo(int32 Width = 512, int32 Height = 512);
	static UTexture2D* CreateMudbrickNormal(int32 Width = 512, int32 Height = 512);

	// Generates a 512x512 procedural Hammered Bronze Metal texture with micro-scratches
	static UTexture2D* CreateHammeredBronzeAlbedo(int32 Width = 512, int32 Height = 512);
	static UTexture2D* CreateHammeredBronzeNormal(int32 Width = 512, int32 Height = 512);

	// Generates a 512x512 procedural Cedar Wood Grain texture
	static UTexture2D* CreateCedarWoodAlbedo(int32 Width = 512, int32 Height = 512);

	// Helper to create and configure dynamic material instance with M_DominionMaster
	static UMaterialInstanceDynamic* CreateDominionMaterial(UObject* Outer, const FLinearColor& BaseColor, float Metallic = 0.0f, float Roughness = 0.7f, const FLinearColor& EmissiveColor = FLinearColor::Black);

	// Helper to bind generated PBR textures to a dynamic material instance
	static void ApplyPBRMaps(UMaterialInstanceDynamic* DynMat, UTexture2D* AlbedoMap, UTexture2D* NormalMap = nullptr, float Metallic = 0.0f, float Roughness = 0.5f);

private:
	static UTexture2D* CreateTextureFromRGBA(int32 Width, int32 Height, const TArray<FColor>& Pixels, bool bIsNormalMap = false);
};
