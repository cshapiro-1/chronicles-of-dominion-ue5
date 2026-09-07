#include "DominionTutorialSubsystem.h"
#include "Engine/Engine.h"

void UDominionTutorialSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CurrentStep = EDominionTutorialStep::Step1_Selection;
	FormationsTestedCount = 0;
}

void UDominionTutorialSubsystem::NotifyArmySelected(int32 UnitCount)
{
	if (CurrentStep == EDominionTutorialStep::Step1_Selection && UnitCount >= 1)
	{
		AdvanceStep();
	}
}

void UDominionTutorialSubsystem::NotifyFormationTriggered(const FString& FormationName)
{
	if (CurrentStep == EDominionTutorialStep::Step2_Formations)
	{
		FormationsTestedCount++;
		if (FormationsTestedCount >= 2)
		{
			AdvanceStep();
		}
	}
}

void UDominionTutorialSubsystem::NotifyMovementCommand()
{
	if (CurrentStep == EDominionTutorialStep::Step3_Movement)
	{
		AdvanceStep();
	}
}

void UDominionTutorialSubsystem::NotifyCombatStarted()
{
	if (CurrentStep == EDominionTutorialStep::Step4_Combat)
	{
		AdvanceStep();
	}
}

void UDominionTutorialSubsystem::NotifyUnitRecruited(const FString& UnitName)
{
	if (CurrentStep == EDominionTutorialStep::Step5_Recruitment)
	{
		AdvanceStep();
	}
}

void UDominionTutorialSubsystem::AdvanceStep()
{
	int32 NextIdx = static_cast<int32>(CurrentStep) + 1;
	if (NextIdx <= static_cast<int32>(EDominionTutorialStep::Step6_MasteryCompleted))
	{
		CurrentStep = static_cast<EDominionTutorialStep>(NextIdx);
	}
}

FString UDominionTutorialSubsystem::GetObjectiveTitle() const
{
	switch (CurrentStep)
	{
	case EDominionTutorialStep::Step1_Selection:
		return TEXT("SELECT YOUR ARMY");
	case EDominionTutorialStep::Step2_Formations:
		return TEXT("DRILL BATTLE FORMATIONS");
	case EDominionTutorialStep::Step3_Movement:
		return TEXT("ISSUE MARCH ORDERS");
	case EDominionTutorialStep::Step4_Combat:
		return TEXT("ENGAGE NOMADIC RAIDERS");
	case EDominionTutorialStep::Step5_Recruitment:
		return TEXT("RECRUIT AT THE ZIGGURAT");
	case EDominionTutorialStep::Step6_MasteryCompleted:
		return TEXT("TUTORIAL COMPLETE: CONQUEST AWAITS");
	default:
		return TEXT("TACTICAL COMMAND");
	}
}

FString UDominionTutorialSubsystem::GetObjectiveDescription() const
{
	switch (CurrentStep)
	{
	case EDominionTutorialStep::Step1_Selection:
		return TEXT("Left-Click on a Spearman or Click-and-Drag a marquee box across your Bronze Phalanx army.");
	case EDominionTutorialStep::Step2_Formations:
		return TEXT("Press [Q] for Phalanx Shield Wall (+40% Armor) or [W] for Wedge Shock Charge.");
	case EDominionTutorialStep::Step3_Movement:
		return TEXT("Right-Click on the ground to command your selected regiments to march in organized formation.");
	case EDominionTutorialStep::Step4_Combat:
		return TEXT("Right-Click on the red Nomadic Raider Vanguard across the field to order an attack.");
	case EDominionTutorialStep::Step5_Recruitment:
		return TEXT("Click the Ziggurat Town Center and press [A] to recruit Spearmen or [S] for War Chariots.");
	case EDominionTutorialStep::Step6_MasteryCompleted:
		return TEXT("You have mastered army maneuvers, combat, and civic recruitment. Build your empire!");
	default:
		return TEXT("Command your forces.");
	}
}

FString UDominionTutorialSubsystem::GetAdvisorAdvice() const
{
	switch (CurrentStep)
	{
	case EDominionTutorialStep::Step1_Selection:
		return TEXT("High Priest of Ur: 'My Lord, our bronze regiments await your command. Select them now!'");
	case EDominionTutorialStep::Step2_Formations:
		return TEXT("Vizier: 'A Phalanx shield wall repels charges; a Wedge punch breaks enemy lines. Drill your men!'");
	case EDominionTutorialStep::Step3_Movement:
		return TEXT("Vizier: 'March disciplined through the valley. Guard our granary supply routes from ambush!'");
	case EDominionTutorialStep::Step4_Combat:
		return TEXT("Warlord: 'Raiders on our perimeter! Flank them with Heavy Chariots and pin them with Spearmen!'");
	case EDominionTutorialStep::Step5_Recruitment:
		return TEXT("High Priest: 'Our city granaries are stocked. Train new spearmen to expand our dominion!'");
	case EDominionTutorialStep::Step6_MasteryCompleted:
		return TEXT("Vizier: 'Ur stands victorious! Forge tools of bronze and advance toward the Iron Age!'");
	default:
		return TEXT("Command wisely, Ruler.");
	}
}
