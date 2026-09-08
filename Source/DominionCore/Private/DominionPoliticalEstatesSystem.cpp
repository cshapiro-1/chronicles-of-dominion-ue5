#include "DominionPoliticalEstatesSystem.h"
#include "DominionSupplyLineSubsystem.h"
#include "Engine/World.h"

void UDominionPoliticalEstatesSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	PriesthoodLoyalty = 65.0f;
	NobilityLoyalty = 60.0f;
	MassesLoyalty = 58.0f;
	Hope = 65.0f;
	Discontent = 22.0f;

	ActiveEdictName = TEXT("None");
	ActiveEdictTimer = 0.0f;
	CrisisDisplayTimer = 0.0f;
	CrisisCooldown = 0.0f;
}

void UDominionPoliticalEstatesSystem::Deinitialize()
{
	Super::Deinitialize();
}

void UDominionPoliticalEstatesSystem::Tick(float DeltaTime)
{
	// 1. Process Active Imperial Edict Timer
	if (ActiveEdictTimer > 0.0f)
	{
		ActiveEdictTimer -= DeltaTime;
		if (ActiveEdictTimer <= 0.0f)
		{
			ActiveEdictTimer = 0.0f;
			ActiveEdictName = TEXT("None");
		}
	}

	// 2. Dynamic 3-Estate Tug-of-War Drift towards Equilibrium
	// If one estate has immense loyalty (>80%), the other two feel neglected and slowly lose loyalty
	if (PriesthoodLoyalty > 75.0f)
	{
		MassesLoyalty = FMath::Max(5.0f, MassesLoyalty - (1.2f * DeltaTime));
		NobilityLoyalty = FMath::Max(5.0f, NobilityLoyalty - (0.8f * DeltaTime));
	}
	if (NobilityLoyalty > 75.0f)
	{
		MassesLoyalty = FMath::Max(5.0f, MassesLoyalty - (1.5f * DeltaTime)); // Feudal extraction hurts commoners
	}
	if (MassesLoyalty > 75.0f)
	{
		NobilityLoyalty = FMath::Max(5.0f, NobilityLoyalty - (1.0f * DeltaTime)); // Nobles resist commoner concessions
	}

	// 3. Supply Line & Starvation Impact on Hope and Discontent
	UDominionSupplyLineSubsystem* SupplySys = GetWorld() ? GetWorld()->GetSubsystem<UDominionSupplyLineSubsystem>() : nullptr;
	bool bSupplyActive = SupplySys ? SupplySys->IsSupplyLineActive() : true;

	if (!bSupplyActive)
	{
		// Severed supply lines cause rapid discontent accumulation and hope collapse
		Discontent = FMath::Clamp(Discontent + (3.5f * DeltaTime), 0.0f, 100.0f);
		Hope = FMath::Clamp(Hope - (2.5f * DeltaTime), 0.0f, 100.0f);
		MassesLoyalty = FMath::Max(5.0f, MassesLoyalty - (2.0f * DeltaTime));
	}
	else
	{
		// Gentle passive recovery toward baseline when well-supplied
		if (Discontent > 20.0f) Discontent = FMath::Max(20.0f, Discontent - (0.5f * DeltaTime));
		if (Hope < 60.0f) Hope = FMath::Min(60.0f, Hope + (0.5f * DeltaTime));
	}

	// 4. Estate Low Loyalty Blowback on Discontent
	if (PriesthoodLoyalty < 30.0f) Discontent = FMath::Clamp(Discontent + (1.2f * DeltaTime), 0.0f, 100.0f);
	if (NobilityLoyalty < 30.0f)   Discontent = FMath::Clamp(Discontent + (1.5f * DeltaTime), 0.0f, 100.0f);
	if (MassesLoyalty < 30.0f)     Discontent = FMath::Clamp(Discontent + (2.0f * DeltaTime), 0.0f, 100.0f);

	// 5. High Discontent Erode Hope
	if (Discontent > 60.0f)
	{
		Hope = FMath::Clamp(Hope - (1.8f * DeltaTime), 0.0f, 100.0f);
	}

	// 6. Crisis Timers and Periodic Check
	if (CrisisDisplayTimer > 0.0f)
	{
		CrisisDisplayTimer -= DeltaTime;
	}
	if (CrisisCooldown > 0.0f)
	{
		CrisisCooldown -= DeltaTime;
	}

	CheckCrises(DeltaTime);
}

bool UDominionPoliticalEstatesSystem::EnactEdict(EDominionEdictType EdictType)
{
	switch (EdictType)
	{
	case EDominionEdictType::SacredGrainTithe:
		PriesthoodLoyalty = FMath::Clamp(PriesthoodLoyalty + 22.0f, 0.0f, 100.0f);
		MassesLoyalty = FMath::Clamp(MassesLoyalty - 16.0f, 0.0f, 100.0f);
		Hope = FMath::Clamp(Hope + 10.0f, 0.0f, 100.0f);
		Discontent = FMath::Clamp(Discontent + 8.0f, 0.0f, 100.0f);
		ActiveEdictName = TEXT("Sacred Grain Tithe (Church +22, Masses -16)");
		ActiveEdictTimer = 30.0f;
		OnEdictEnacted.Broadcast(EdictType, ActiveEdictName);
		return true;

	case EDominionEdictType::FeudalConscription:
		NobilityLoyalty = FMath::Clamp(NobilityLoyalty + 24.0f, 0.0f, 100.0f);
		MassesLoyalty = FMath::Clamp(MassesLoyalty - 20.0f, 0.0f, 100.0f);
		Discontent = FMath::Clamp(Discontent + 15.0f, 0.0f, 100.0f);
		Hope = FMath::Clamp(Hope - 8.0f, 0.0f, 100.0f);
		ActiveEdictName = TEXT("Feudal Corvée Conscription (Nobles +24, Masses -20)");
		ActiveEdictTimer = 30.0f;
		OnEdictEnacted.Broadcast(EdictType, ActiveEdictName);
		return true;

	case EDominionEdictType::ImperialBreadDole:
		MassesLoyalty = FMath::Clamp(MassesLoyalty + 28.0f, 0.0f, 100.0f);
		NobilityLoyalty = FMath::Clamp(NobilityLoyalty - 14.0f, 0.0f, 100.0f);
		PriesthoodLoyalty = FMath::Clamp(PriesthoodLoyalty - 8.0f, 0.0f, 100.0f);
		Hope = FMath::Clamp(Hope + 22.0f, 0.0f, 100.0f);
		Discontent = FMath::Clamp(Discontent - 25.0f, 0.0f, 100.0f);
		ActiveEdictName = TEXT("Imperial Bread Dole (Masses +28, Hope +22, Discontent -25)");
		ActiveEdictTimer = 30.0f;
		OnEdictEnacted.Broadcast(EdictType, ActiveEdictName);
		return true;

	case EDominionEdictType::GladiatorCircus:
		Hope = FMath::Clamp(Hope + 26.0f, 0.0f, 100.0f);
		Discontent = FMath::Clamp(Discontent - 20.0f, 0.0f, 100.0f);
		NobilityLoyalty = FMath::Clamp(NobilityLoyalty + 12.0f, 0.0f, 100.0f);
		MassesLoyalty = FMath::Clamp(MassesLoyalty + 14.0f, 0.0f, 100.0f);
		PriesthoodLoyalty = FMath::Clamp(PriesthoodLoyalty - 10.0f, 0.0f, 100.0f); // Priesthood condemns secular blood sport
		ActiveEdictName = TEXT("Grand Gladiator & Chariot Games (Hope +26, Discontent -20)");
		ActiveEdictTimer = 30.0f;
		OnEdictEnacted.Broadcast(EdictType, ActiveEdictName);
		return true;

	default:
		break;
	}
	return false;
}

void UDominionPoliticalEstatesSystem::ModifyEstateLoyalty(EDominionEstateType Estate, float Delta)
{
	switch (Estate)
	{
	case EDominionEstateType::Priesthood:
		PriesthoodLoyalty = FMath::Clamp(PriesthoodLoyalty + Delta, 0.0f, 100.0f);
		break;
	case EDominionEstateType::Nobility:
		NobilityLoyalty = FMath::Clamp(NobilityLoyalty + Delta, 0.0f, 100.0f);
		break;
	case EDominionEstateType::Masses:
		MassesLoyalty = FMath::Clamp(MassesLoyalty + Delta, 0.0f, 100.0f);
		break;
	}
}

void UDominionPoliticalEstatesSystem::ModifyHope(float Delta)
{
	Hope = FMath::Clamp(Hope + Delta, 0.0f, 100.0f);
}

void UDominionPoliticalEstatesSystem::ModifyDiscontent(float Delta)
{
	Discontent = FMath::Clamp(Discontent + Delta, 0.0f, 100.0f);
}

void UDominionPoliticalEstatesSystem::CheckCrises(float DeltaTime)
{
	if (CrisisCooldown > 0.0f) return;

	// 1. High Discontent Critical Crisis (> 80%)
	if (Discontent >= 80.0f)
	{
		TriggerCrisis(
			TEXT("POPULAR INSURRECTION & URBAN RIOTS"),
			TEXT("Discontent has breached breaking point! Mobs are rioting in the capital and storming imperial granaries!")
		);
		return;
	}

	// 2. Hope Collapse Crisis (< 15%)
	if (Hope <= 15.0f)
	{
		TriggerCrisis(
			TEXT("TOTAL IMPERIAL DESPAIR & MUTINY"),
			TEXT("The people have lost all faith in the Crown! Legionnaires are throwing down their bronze spears and deserting!")
		);
		return;
	}

	// 3. Priesthood Alienation (< 20%)
	if (PriesthoodLoyalty <= 20.0f)
	{
		TriggerCrisis(
			TEXT("HOLY ANATHEMA & PRIESTHOOD EXCOMMUNICATION"),
			TEXT("The High Priesthood has cursed the Sovereign! Flagellant zealots preach open rebellion at temple steps!")
		);
		return;
	}

	// 4. Nobility Alienation (< 20%)
	if (NobilityLoyalty <= 20.0f)
	{
		TriggerCrisis(
			TEXT("BARONIAL LEVY REBELLION & COUP"),
			TEXT("The Warlord Aristocracy has defied the throne! Feudal lords withhold their war chariots and plot regicide!")
		);
		return;
	}

	// 5. Common Masses Alienation (< 20%)
	if (MassesLoyalty <= 20.0f)
	{
		TriggerCrisis(
			TEXT("GENERAL SERF STRIKE & CANAL REVOLT"),
			TEXT("The third estate refuses forced labor! Irrigation canals are left untended and food production is paralyzed!")
		);
		return;
	}
}

void UDominionPoliticalEstatesSystem::TriggerCrisis(const FString& Title, const FString& Desc)
{
	ActiveCrisisTitle = Title;
	ActiveCrisisDesc = Desc;
	CrisisDisplayTimer = 10.0f; // Show notification on HUD for 10 seconds
	CrisisCooldown = 35.0f;     // Cooldown between major crises

	OnCrisisTriggered.Broadcast(Title, Desc);
}

void UDominionPoliticalEstatesSystem::ApplySavageryBlowback(const FDominionSavageryMetrics& Metrics)
{
	if (Metrics.MoralDecay > 30.0f)
	{
		PriesthoodLoyalty = FMath::Max(5.0f, PriesthoodLoyalty - (Metrics.MoralDecay * 0.5f));
		Discontent = FMath::Min(100.0f, Discontent + 25.0f);
		TriggerCrisis(
			TEXT("HOLY ANATHEMA & EXCOMMUNICATION"),
			TEXT("The Priesthood has cursed the Crown for slaughtering whole nations! Blood pollution defiles the kingdom!")
		);
	}

	if (Metrics.LegionDisloyalty > 40.0f)
	{
		NobilityLoyalty = FMath::Max(5.0f, NobilityLoyalty - 30.0f);
		Discontent = FMath::Min(100.0f, Discontent + 20.0f);
		TriggerCrisis(
			TEXT("LEGION DONATIVUM COUP ATTEMPT"),
			TEXT("Veterans hardened by total massacre demand immediate gold extortion or will burn the imperial capital!")
		);
	}
}

