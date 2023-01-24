// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FighterEngine : ModuleRules
{
	public FighterEngine(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		if (Target.bBuildEditor)
		{
			OptimizeCode = CodeOptimization.Never;
		}
		
		PublicDependencyModuleNames.AddRange(new string[] { "Core", 
			"CoreUObject",
			"Engine",
			"InputCore",
			"OnlineSubsystem",
			"OnlineSubsystemEOS", 
			"OnlineSubsystemSteam",
			"AdvancedSessions",
			"AdvancedSteamSessions",
			"Networking",
			"Sockets",
			"Niagara",
			"LevelSequence",
			"MovieScene",
			"UMG",
			"EnhancedInput",
			"GGPOUE4"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
