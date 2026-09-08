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

/** The 4 Asymmetric Dark-Fantasy Starting Civilizations */
UENUM(BlueprintType)
enum class EDominionFaction : uint8
{
	AshenHegemony      UMETA(DisplayName = "The Ashen Theocracy of Ashur-Kish"),
	IronDynasts        UMETA(DisplayName = "The Iron Slaver League of Karn-Drak"),
	MiasmaNomads       UMETA(DisplayName = "The Black-Salt Horde of the Dune Wastes"),
	SunkenSyndicate    UMETA(DisplayName = "The River-Lich Syndicate of Marad-Nun")
};

USTRUCT(BlueprintType)
struct DOMINIONCORE_API FDominionCivTrait
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Faction")
	EDominionFaction FactionID = EDominionFaction::AshenHegemony;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Faction")
	FString FactionName = TEXT("The Ashen Theocracy of Ashur-Kish");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Faction")
	FString Motto = TEXT("Through Ash We Cleanse, In Blood We Endure");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Faction")
	FString SigilIcon = TEXT("🔥");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Faction")
	FString PrimaryStrength = TEXT("+50% Tithe Income • Fanatical Morale (No Routing Panic)");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Faction")
	FString CriticalWeakness = TEXT("+25% Discontent during peacetime • Zero Foreign Trade Diplomacy");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Faction")
	FString UniqueUnit = TEXT("Blood-Sworn Zealot Hoplite (Ignited Bitumen Spears)");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Faction")
	FString UniqueEdict = TEXT("Edict of the Eternal Pyre (Sacrifice Grain for +30% Combat Attack)");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Faction")
	float MoraleBreakModifier = 0.0f; // Unbreakable morale

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Faction")
	float MiningMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Faction")
	float CavalrySpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Faction")
	float RiverTradeMultiplier = 1.0f;
};

/** Tactical Formations for mass cohorts */
UENUM(BlueprintType)
enum class EDominionFormation : uint8
{
	Line     UMETA(DisplayName = "Line"),
	Square   UMETA(DisplayName = "Square"),
	Phalanx  UMETA(DisplayName = "Phalanx"),
	Skirmish UMETA(DisplayName = "Skirmish")
};

/** Supply status and starvation metrics for military cohorts */
USTRUCT(BlueprintType)
struct DOMINIONCORE_API FCohortSupplyStatus
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Supply")
	FName CohortID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Supply")
	bool bIsTethered = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Supply")
	float RationsRemainingSeconds = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Supply")
	float MaxRationsSeconds = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dominion|Supply")
	float StarvationDecayRate = 0.015f; // Baseline 1.5% health loss per second
};


