using UnrealBuildTool;
using System.Collections.Generic;

public class ChroniclesOfDominionTarget : TargetRules
{
	public ChroniclesOfDominionTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("DominionCore");
	}
}
