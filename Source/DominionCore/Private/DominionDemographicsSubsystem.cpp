#include "DominionDemographicsSubsystem.h"

void UDominionDemographicsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	AgePyramid.Infants0To4 = 3500;
	AgePyramid.Children5To14 = 4200;
	AgePyramid.Adults15To50 = 6500;
	AgePyramid.Elders50Plus = 800;

	InfantMortalityRate = 0.38f; // 380 deaths per 1,000 births
	AnnualBirthRate = 0.075f;
	MigrationPolicy = EDominionMigrationPolicy::CityAirMakesYouFree;
	RuralPopulation = 12000;
	UrbanPopulation = 3000;
	SlumPopulation = 1200;
	UrbanGrainDaysBuffer = 30.0f;
}

void UDominionDemographicsSubsystem::ProcessDemographicsTick(float DeltaTime, float AvailableGrain, float WaterSanitation, float FirewoodStock, float DietaryDiversity, float MidwiferyTier)
{
	// 1. Calculate dynamic Infant Mortality Rate based on sanitation & healthcare
	float CalculatedIMR = 0.50f; // 50% baseline without any civil services
	CalculatedIMR -= (WaterSanitation * 0.15f);     // -15% for clean aqueducts/wells
	CalculatedIMR -= (FirewoodStock * 0.10f);       // -10% for warm winter hearths
	CalculatedIMR -= (DietaryDiversity * 0.10f);    // -10% for protein/dairy diversity
	CalculatedIMR -= (MidwiferyTier * 0.15f);       // -15% for midwife guilds / antiseptics
	InfantMortalityRate = FMath::Clamp(CalculatedIMR, 0.04f, 0.55f);

	// 2. New Births from Adult Cohort
	const int64 NewBirths = FMath::FloorToInt(AgePyramid.Adults15To50 * AnnualBirthRate * (DeltaTime / 12.0f));
	const int64 SurvivingInfants = FMath::FloorToInt(NewBirths * (1.0f - InfantMortalityRate));

	AgePyramid.Infants0To4 += SurvivingInfants;

	// 3. Cohort Aging Transitions (Annual tick)
	const float AgingRate = DeltaTime / 12.0f;
	const int64 InfantsGraduating = FMath::FloorToInt(AgePyramid.Infants0To4 * (AgingRate / 5.0f));
	const int64 ChildrenGraduating = FMath::FloorToInt(AgePyramid.Children5To14 * (AgingRate / 10.0f));
	const int64 AdultsGraduating = FMath::FloorToInt(AgePyramid.Adults15To50 * (AgingRate / 35.0f));

	AgePyramid.Infants0To4 = FMath::Max(static_cast<int64>(0), AgePyramid.Infants0To4 - InfantsGraduating);
	AgePyramid.Children5To14 += InfantsGraduating;

	AgePyramid.Children5To14 = FMath::Max(static_cast<int64>(0), AgePyramid.Children5To14 - ChildrenGraduating);
	AgePyramid.Adults15To50 += ChildrenGraduating;

	AgePyramid.Adults15To50 = FMath::Max(static_cast<int64>(0), AgePyramid.Adults15To50 - AdultsGraduating);
	AgePyramid.Elders50Plus += AdultsGraduating;

	// Elder mortality
	const int64 ElderDeaths = FMath::FloorToInt(AgePyramid.Elders50Plus * 0.10f * AgingRate);
	AgePyramid.Elders50Plus = FMath::Max(static_cast<int64>(0), AgePyramid.Elders50Plus - ElderDeaths);
}

void UDominionDemographicsSubsystem::SetMigrationPolicy(EDominionMigrationPolicy NewPolicy)
{
	MigrationPolicy = NewPolicy;
}
