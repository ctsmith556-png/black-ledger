// Black Ledger - primary game module rules

using UnrealBuildTool;

public class BlackLedger : ModuleRules
{
	public BlackLedger(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// flat layout (Core/, Vehicles/, ...) - make module-root-relative includes work
		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput"
		});

		// EngineCameras: UE 5.7 keeps the camera-shake patterns (PerlinNoise etc.)
		// in this engine plugin, not the Engine module.
		// AIModule + GameplayTasks: AAIController.
		PrivateDependencyModuleNames.AddRange(new string[] { "EngineCameras", "AIModule", "GameplayTasks" });
	}
}
