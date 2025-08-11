// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WashingMan : ModuleRules
{
	public WashingMan(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
            "Niagara"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"WashingMan",
			"WashingMan/Variant_Platforming",
			"WashingMan/Variant_Combat",
			"WashingMan/Variant_Combat/AI",
			"WashingMan/Variant_SideScrolling",
			"WashingMan/Variant_SideScrolling/Gameplay",
			"WashingMan/Variant_SideScrolling/AI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
