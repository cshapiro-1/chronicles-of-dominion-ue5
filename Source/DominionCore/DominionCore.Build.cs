using UnrealBuildTool;

public class DominionCore : ModuleRules
{
	public DominionCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"MassCore",
			"MassEntity",
			"MassCommon",
			"MassActors",
			"MassMovement",
			"MassSpawner",
			"MassRepresentation",
			"MassSimulation",
			"MassCrowd",
			"MassSignals",
			"Niagara",
			"GeometryCollectionEngine",
			"AIModule",
			"NavigationSystem"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Slate",
			"SlateCore"
		});
	}
}
