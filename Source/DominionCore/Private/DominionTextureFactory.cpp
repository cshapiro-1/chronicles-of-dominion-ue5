#include "DominionTextureFactory.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Math/UnrealMathUtility.h"

namespace
{
	// Lightweight 2D Continuous Value Noise with cosine interpolation
	float Hash2D(int32 X, int32 Y)
	{
		int32 N = X * 374761393 + Y * 668265263;
		N = (N ^ (N >> 13)) * 1274126177;
		return (float)(N & 0x7FFFFFFF) / (float)0x7FFFFFFF;
	}

	float SmoothNoise2D(float X, float Y)
	{
		int32 X0 = FMath::FloorToInt(X);
		int32 Y0 = FMath::FloorToInt(Y);
		int32 X1 = X0 + 1;
		int32 Y1 = Y0 + 1;

		float Fx = X - (float)X0;
		float Fy = Y - (float)Y0;

		// Cosine / Smoothstep interpolation
		float Sx = Fx * Fx * (3.0f - 2.0f * Fx);
		float Sy = Fy * Fy * (3.0f - 2.0f * Fy);

		float N00 = Hash2D(X0, Y0);
		float N10 = Hash2D(X1, Y0);
		float N01 = Hash2D(X0, Y1);
		float N11 = Hash2D(X1, Y1);

		float Nx0 = FMath::Lerp(N00, N10, Sx);
		float Nx1 = FMath::Lerp(N01, N11, Sx);

		return FMath::Lerp(Nx0, Nx1, Sy);
	}

	float FractalNoise2D(float X, float Y, int32 Octaves = 4, float Lacunarity = 2.0f, float Gain = 0.5f)
	{
		float Total = 0.0f;
		float Frequency = 1.0f;
		float Amplitude = 1.0f;
		float MaxValue = 0.0f;

		for (int32 i = 0; i < Octaves; ++i)
		{
			Total += SmoothNoise2D(X * Frequency, Y * Frequency) * Amplitude;
			MaxValue += Amplitude;
			Frequency *= Lacunarity;
			Amplitude *= Gain;
		}

		return (MaxValue > 0.0f) ? (Total / MaxValue) : 0.0f;
	}
}

UTexture2D* UDominionTextureFactory::CreateTextureFromRGBA(int32 Width, int32 Height, const TArray<FColor>& Pixels, bool bIsNormalMap)
{
	if (Width <= 0 || Height <= 0 || Pixels.Num() != Width * Height)
	{
		return nullptr;
	}

	UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	if (!NewTexture)
	{
		return nullptr;
	}

	if (bIsNormalMap)
	{
		NewTexture->CompressionSettings = TC_Normalmap;
		NewTexture->SRGB = false;
	}
	else
	{
		NewTexture->CompressionSettings = TC_Default;
		NewTexture->SRGB = true;
	}

#if WITH_EDITOR || !UE_SERVER
	FTexturePlatformData* PlatformData = NewTexture->GetPlatformData();
	if (PlatformData && PlatformData->Mips.Num() > 0)
	{
		FTexture2DMipMap& Mip = PlatformData->Mips[0];
		void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
		if (Data)
		{
			FMemory::Memcpy(Data, Pixels.GetData(), Width * Height * sizeof(FColor));
			Mip.BulkData.Unlock();
			NewTexture->UpdateResource();
		}
	}
#endif

	return NewTexture;
}

UTexture2D* UDominionTextureFactory::CreateSandstoneSiltAlbedo(int32 Width, int32 Height)
{
	TArray<FColor> Pixels;
	Pixels.SetNumUninitialized(Width * Height);

	// Desert sandstone palette: warm golden ochre base with mineral flecks
	const FColor SandBase(215, 178, 126);
	const FColor SandHighlight(238, 204, 155);
	const FColor SandShadow(170, 130, 88);

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			float N1 = FractalNoise2D((float)X * 0.04f, (float)Y * 0.04f, 4);
			float Dunes = FMath::Sin(((float)X * 0.05f + (float)Y * 0.02f) * 3.14159f + N1 * 3.0f) * 0.5f + 0.5f;
			float Blend = FMath::Clamp(N1 * 0.6f + Dunes * 0.4f, 0.0f, 1.0f);

			uint8 R = FMath::Clamp(FMath::RoundToInt(FMath::Lerp((float)SandShadow.R, (float)SandHighlight.R, Blend)), 0, 255);
			uint8 G = FMath::Clamp(FMath::RoundToInt(FMath::Lerp((float)SandShadow.G, (float)SandHighlight.G, Blend)), 0, 255);
			uint8 B = FMath::Clamp(FMath::RoundToInt(FMath::Lerp((float)SandShadow.B, (float)SandHighlight.B, Blend)), 0, 255);

			Pixels[Y * Width + X] = FColor(R, G, B, 255);
		}
	}

	return CreateTextureFromRGBA(Width, Height, Pixels, false);
}

UTexture2D* UDominionTextureFactory::CreateSandstoneSiltNormal(int32 Width, int32 Height)
{
	TArray<FColor> Pixels;
	Pixels.SetNumUninitialized(Width * Height);

	auto HeightFunc = [](float X, float Y) -> float
	{
		float N = FractalNoise2D(X * 0.04f, Y * 0.04f, 3);
		float Dunes = FMath::Sin((X * 0.05f + Y * 0.02f) * 3.14159f + N * 3.0f);
		return Dunes * 0.6f + N * 0.4f;
	};

	const float Strength = 2.5f;

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			float H_L = HeightFunc((float)(X - 1), (float)Y);
			float H_R = HeightFunc((float)(X + 1), (float)Y);
			float H_D = HeightFunc((float)X, (float)(Y - 1));
			float H_U = HeightFunc((float)X, (float)(Y + 1));

			FVector Normal((H_L - H_R) * Strength, (H_D - H_U) * Strength, 1.0f);
			Normal.Normalize();

			uint8 R = FMath::Clamp(FMath::RoundToInt((Normal.X * 0.5f + 0.5f) * 255.0f), 0, 255);
			uint8 G = FMath::Clamp(FMath::RoundToInt((Normal.Y * 0.5f + 0.5f) * 255.0f), 0, 255);
			uint8 B = FMath::Clamp(FMath::RoundToInt((Normal.Z * 0.5f + 0.5f) * 255.0f), 0, 255);

			Pixels[Y * Width + X] = FColor(R, G, B, 255);
		}
	}

	return CreateTextureFromRGBA(Width, Height, Pixels, true);
}

UTexture2D* UDominionTextureFactory::CreateWaterWaveNormal(int32 Width, int32 Height)
{
	TArray<FColor> Pixels;
	Pixels.SetNumUninitialized(Width * Height);

	auto WaveHeight = [](float X, float Y) -> float
	{
		float W1 = FMath::Sin(X * 0.08f + Y * 0.03f);
		float W2 = FMath::Cos(X * 0.04f - Y * 0.07f);
		float W3 = FMath::Sin(X * 0.12f + Y * 0.11f) * 0.5f;
		return (W1 + W2 + W3) * 0.4f;
	};

	const float Strength = 3.5f;

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			float H_L = WaveHeight((float)(X - 1), (float)Y);
			float H_R = WaveHeight((float)(X + 1), (float)Y);
			float H_D = WaveHeight((float)X, (float)(Y - 1));
			float H_U = WaveHeight((float)X, (float)(Y + 1));

			FVector Normal((H_L - H_R) * Strength, (H_D - H_U) * Strength, 1.0f);
			Normal.Normalize();

			uint8 R = FMath::Clamp(FMath::RoundToInt((Normal.X * 0.5f + 0.5f) * 255.0f), 0, 255);
			uint8 G = FMath::Clamp(FMath::RoundToInt((Normal.Y * 0.5f + 0.5f) * 255.0f), 0, 255);
			uint8 B = FMath::Clamp(FMath::RoundToInt((Normal.Z * 0.5f + 0.5f) * 255.0f), 0, 255);

			Pixels[Y * Width + X] = FColor(R, G, B, 255);
		}
	}

	return CreateTextureFromRGBA(Width, Height, Pixels, true);
}

UTexture2D* UDominionTextureFactory::CreateMudbrickAlbedo(int32 Width, int32 Height)
{
	TArray<FColor> Pixels;
	Pixels.SetNumUninitialized(Width * Height);

	const FColor BrickTan(195, 150, 105);
	const FColor BrickDark(150, 105, 68);
	const FColor MortarLine(110, 80, 50);

	const int32 BrickW = 64;
	const int32 BrickH = 32;

	for (int32 Y = 0; Y < Height; ++Y)
	{
		int32 Row = Y / BrickH;
		int32 LocalY = Y % BrickH;
		int32 OffsetX = (Row % 2 == 1) ? (BrickW / 2) : 0;

		for (int32 X = 0; X < Width; ++X)
		{
			int32 LocalX = (X + OffsetX) % BrickW;

			bool bIsMortar = (LocalX <= 2 || LocalY <= 2);

			if (bIsMortar)
			{
				Pixels[Y * Width + X] = MortarLine;
			}
			else
			{
				float Noise = FractalNoise2D((float)X * 0.08f, (float)Y * 0.08f, 3);
				uint8 R = FMath::Clamp(FMath::RoundToInt(FMath::Lerp((float)BrickDark.R, (float)BrickTan.R, Noise)), 0, 255);
				uint8 G = FMath::Clamp(FMath::RoundToInt(FMath::Lerp((float)BrickDark.G, (float)BrickTan.G, Noise)), 0, 255);
				uint8 B = FMath::Clamp(FMath::RoundToInt(FMath::Lerp((float)BrickDark.B, (float)BrickTan.B, Noise)), 0, 255);
				Pixels[Y * Width + X] = FColor(R, G, B, 255);
			}
		}
	}

	return CreateTextureFromRGBA(Width, Height, Pixels, false);
}

UTexture2D* UDominionTextureFactory::CreateMudbrickNormal(int32 Width, int32 Height)
{
	TArray<FColor> Pixels;
	Pixels.SetNumUninitialized(Width * Height);

	const int32 BrickW = 64;
	const int32 BrickH = 32;

	auto MudbrickH = [BrickW, BrickH](float X, float Y) -> float
	{
		int32 YInt = FMath::FloorToInt(Y);
		int32 XInt = FMath::FloorToInt(X);
		int32 Row = YInt / BrickH;
		int32 LocalY = YInt % BrickH;
		int32 OffsetX = (Row % 2 == 1) ? (BrickW / 2) : 0;
		int32 LocalX = (XInt + OffsetX) % BrickW;

		if (LocalX <= 2 || LocalY <= 2) return 0.0f; // Mortar groove
		return 1.0f + FractalNoise2D(X * 0.05f, Y * 0.05f, 2) * 0.25f;
	};

	const float Strength = 4.0f;

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			float H_L = MudbrickH((float)(X - 1), (float)Y);
			float H_R = MudbrickH((float)(X + 1), (float)Y);
			float H_D = MudbrickH((float)X, (float)(Y - 1));
			float H_U = MudbrickH((float)X, (float)(Y + 1));

			FVector Normal((H_L - H_R) * Strength, (H_D - H_U) * Strength, 1.0f);
			Normal.Normalize();

			uint8 R = FMath::Clamp(FMath::RoundToInt((Normal.X * 0.5f + 0.5f) * 255.0f), 0, 255);
			uint8 G = FMath::Clamp(FMath::RoundToInt((Normal.Y * 0.5f + 0.5f) * 255.0f), 0, 255);
			uint8 B = FMath::Clamp(FMath::RoundToInt((Normal.Z * 0.5f + 0.5f) * 255.0f), 0, 255);

			Pixels[Y * Width + X] = FColor(R, G, B, 255);
		}
	}

	return CreateTextureFromRGBA(Width, Height, Pixels, true);
}

UTexture2D* UDominionTextureFactory::CreateHammeredBronzeAlbedo(int32 Width, int32 Height)
{
	TArray<FColor> Pixels;
	Pixels.SetNumUninitialized(Width * Height);

	const FColor BronzeBase(225, 160, 65);
	const FColor BronzeSpecular(250, 195, 110);
	const FColor BronzePatina(145, 95, 35);

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			float Noise = FractalNoise2D((float)X * 0.12f, (float)Y * 0.12f, 4);
			uint8 R = FMath::Clamp(FMath::RoundToInt(FMath::Lerp((float)BronzePatina.R, (float)BronzeSpecular.R, Noise)), 0, 255);
			uint8 G = FMath::Clamp(FMath::RoundToInt(FMath::Lerp((float)BronzePatina.G, (float)BronzeSpecular.G, Noise)), 0, 255);
			uint8 B = FMath::Clamp(FMath::RoundToInt(FMath::Lerp((float)BronzePatina.B, (float)BronzeSpecular.B, Noise)), 0, 255);
			Pixels[Y * Width + X] = FColor(R, G, B, 255);
		}
	}

	return CreateTextureFromRGBA(Width, Height, Pixels, false);
}

UTexture2D* UDominionTextureFactory::CreateHammeredBronzeNormal(int32 Width, int32 Height)
{
	TArray<FColor> Pixels;
	Pixels.SetNumUninitialized(Width * Height);

	const float Strength = 2.0f;

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			float H_L = FractalNoise2D((float)(X - 1) * 0.12f, (float)Y * 0.12f, 3);
			float H_R = FractalNoise2D((float)(X + 1) * 0.12f, (float)Y * 0.12f, 3);
			float H_D = FractalNoise2D((float)X * 0.12f, (float)(Y - 1) * 0.12f, 3);
			float H_U = FractalNoise2D((float)X * 0.12f, (float)(Y + 1) * 0.12f, 3);

			FVector Normal((H_L - H_R) * Strength, (H_D - H_U) * Strength, 1.0f);
			Normal.Normalize();

			uint8 R = FMath::Clamp(FMath::RoundToInt((Normal.X * 0.5f + 0.5f) * 255.0f), 0, 255);
			uint8 G = FMath::Clamp(FMath::RoundToInt((Normal.Y * 0.5f + 0.5f) * 255.0f), 0, 255);
			uint8 B = FMath::Clamp(FMath::RoundToInt((Normal.Z * 0.5f + 0.5f) * 255.0f), 0, 255);

			Pixels[Y * Width + X] = FColor(R, G, B, 255);
		}
	}

	return CreateTextureFromRGBA(Width, Height, Pixels, true);
}

UTexture2D* UDominionTextureFactory::CreateCedarWoodAlbedo(int32 Width, int32 Height)
{
	TArray<FColor> Pixels;
	Pixels.SetNumUninitialized(Width * Height);

	const FColor WoodLight(115, 68, 38);
	const FColor WoodDark(65, 35, 18);

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			float Grain = FMath::Sin((float)Y * 0.25f + FractalNoise2D((float)X * 0.05f, (float)Y * 0.02f, 2) * 5.0f) * 0.5f + 0.5f;
			uint8 R = FMath::Clamp(FMath::RoundToInt(FMath::Lerp((float)WoodDark.R, (float)WoodLight.R, Grain)), 0, 255);
			uint8 G = FMath::Clamp(FMath::RoundToInt(FMath::Lerp((float)WoodDark.G, (float)WoodLight.G, Grain)), 0, 255);
			uint8 B = FMath::Clamp(FMath::RoundToInt(FMath::Lerp((float)WoodDark.B, (float)WoodLight.B, Grain)), 0, 255);
			Pixels[Y * Width + X] = FColor(R, G, B, 255);
		}
	}

	return CreateTextureFromRGBA(Width, Height, Pixels, false);
}

void UDominionTextureFactory::ApplyPBRMaps(UMaterialInstanceDynamic* DynMat, UTexture2D* AlbedoMap, UTexture2D* NormalMap, float Metallic, float Roughness)
{
	if (!DynMat) return;

	if (AlbedoMap)
	{
		DynMat->SetTextureParameterValue(TEXT("BaseColorMap"), AlbedoMap);
		DynMat->SetTextureParameterValue(TEXT("AlbedoMap"), AlbedoMap);
		DynMat->SetTextureParameterValue(TEXT("Texture"), AlbedoMap);
		DynMat->SetTextureParameterValue(TEXT("BaseColor"), AlbedoMap);
	}

	if (NormalMap)
	{
		DynMat->SetTextureParameterValue(TEXT("NormalMap"), NormalMap);
		DynMat->SetTextureParameterValue(TEXT("Normal"), NormalMap);
		DynMat->SetTextureParameterValue(TEXT("Normals"), NormalMap);
	}

	DynMat->SetScalarParameterValue(TEXT("Metallic"), Metallic);
	DynMat->SetScalarParameterValue(TEXT("Roughness"), Roughness);
	DynMat->SetScalarParameterValue(TEXT("Specular"), 0.5f);
}
