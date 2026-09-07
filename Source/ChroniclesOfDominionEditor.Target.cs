using UnrealBuildTool;
using System.Collections.Generic;

public class ChroniclesOfDominionEditorTarget : TargetRules
{
	public ChroniclesOfDominionEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("DominionCore");
	}
}
