// Black Ledger - editor target

using UnrealBuildTool;
using System.Collections.Generic;

public class BlackLedgerEditorTarget : TargetRules
{
	public BlackLedgerEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("BlackLedger");
	}
}
