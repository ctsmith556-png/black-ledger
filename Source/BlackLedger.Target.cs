// Black Ledger - game target

using UnrealBuildTool;
using System.Collections.Generic;

public class BlackLedgerTarget : TargetRules
{
	public BlackLedgerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("BlackLedger");
	}
}
