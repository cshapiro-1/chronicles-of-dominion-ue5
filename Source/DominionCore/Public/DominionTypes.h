#pragma once

#include "CoreMinimal.h"
#include "DominionTypes.generated.h"

/** The 5 Grand Historical Epochs of Civilization */
UENUM(BlueprintType)
enum class EDominionEpoch : uint8
{
	StoneAge UMETA(DisplayName = "Epoch I: Stone & Bone (10,000 BCE)"),
	BronzeAge UMETA(DisplayName = "Epoch II: Bronze & River Valleys (3,000 BCE)"),
	ClassicalIron UMETA(DisplayName = "Epoch III: Classical Iron Empires (500 BCE)"),
	FeudalMedieval UMETA(DisplayName = "Epoch IV: Feudal High Middle Ages (1100 CE)"),
	IndustrialRevolution UMETA(DisplayName = "Epoch V: Industrial Revolution (1850 CE)")
};

/** Seasonal Climate Cycles */
UENUM(BlueprintType)
enum class EDominionSeason : uint8
{
	Spring UMETA(DisplayName = "Spring (Planting)"),
	Summer UMETA(DisplayName = "Summer (Maturation)"),
	Autumn UMETA(DisplayName = "Autumn (Great Harvest)"),
	Winter UMETA(DisplayName = "Winter (Frost & Freeze)")
};

/** Social Classes evolving by Era */
UENUM(BlueprintType)
enum class EDominionSocialClass : uint8
{
	TribalElder,
	HunterGatherer,
	PatricianNoble,
	PlebeianCitizen,
	SlavesAndCaptives,
	LandedAristocracy,
	ClergyAndMonks,
	GuildArtisans,
	PeasantSerfs,
	IndustrialCapitalists,
	MiddleClassBurghers,
	FactoryProletariat
};

/** Social Class Population Cohort */
USTRUCT(BlueprintType)
struct DOMINIONCORE_API FDominionPopCohort
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demographics")
	EDominionSocialClass SocialClass = EDominionSocialClass::PlebeianCitizen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demographics")
	int32 HeadCount = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demographics")
	float Happiness = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demographics")
	float FoodRationFulfillment = 1.0f;
};

/** The 4 Demographic Age Cohorts */
USTRUCT(BlueprintType)
struct DOMINIONCORE_API FDominionAgePyramid
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demographics")
	int64 Infants0To4 = 3500; // High mortality (30-50%)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demographics")
	int64 Children5To14 = 4200; // Apprentices & light labor (10-15% mortality)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demographics")
	int64 Adults15To50 = 6500; // Prime workforce, soldiers & reproduction (2-4% mortality)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demographics")
	int64 Elders50Plus = 800; // High wisdom, tribal elders & scholars (8-15% mortality)

	int64 GetTotalPopulation() const
	{
		return Infants0To4 + Children5To14 + Adults15To50 + Elders50Plus;
	}
};

/** Key Commodities spanning the 5 Epochs */
UENUM(BlueprintType)
enum class EDominionResource : uint8
{
	FlintAndObsidian,
	RawTimber,
	QuarriedStone,
	Clay,
	CopperOre,
	TinOre,
	IronOre,
	CoalAndCoke,
	RawCottonAndFlax,
	RawWool,
	WildGameAndMammothMeat,
	RawGrain,
	BakingFlour,
	BreadLoaves,
	PreservedSaltedMeat,
	VintageWineAndAle,
	BronzeIngots,
	WroughtIronIngots,
	RefinedSteel,
	IronPlowTools,
	TextilesAndClothing,
	SteamBoilers,
	MachineParts,
	Locomotives,
	GoldCrownsAndCoinage
};

/** Savagery, Annihilation, and Societal Blowback Metrics */
USTRUCT(BlueprintType)
struct DOMINIONCORE_API FDominionSavageryMetrics
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Savagery")
	float SavageryIndex = 0.0f; // 0 (Chivalric/Righteous) to 100 (Abyssal Monster)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Savagery")
	float WorldInfamy = 0.0f; // 0 (Respected) to 100 (Existential Coalition Target)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Savagery")
	float MoralDecay = 0.0f; // Priesthood horror & doomsday cult intensity

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Savagery")
	float CorpsePlagueRisk = 0.0f; // Miasma from unburied slaughtered multitudes

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Savagery")
	float LegionDisloyalty = 0.0f; // Brutalized soldiers demanding Donativum extortion

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Savagery")
	int32 TotalExtinguishedEmpires = 0; // Count of annihilated civilizations
};

