// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FighterEngineEditor : ModuleRules
{
	public FighterEngineEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		if (Target.bBuildEditor)
		{
			OptimizeCode = CodeOptimization.Never;
		}
		
		PublicDependencyModuleNames.AddRange(new string[] {
			"FighterEngine",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
