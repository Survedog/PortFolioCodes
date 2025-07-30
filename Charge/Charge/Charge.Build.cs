// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Charge : ModuleRules
{
	public Charge(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { "Charge" });
	
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"EnhancedInput",
				"UMG",
				"ModularGameplay",
                "ModularGameplayActors",
                "CommonUI",
                "CommonInput",
                "CommonGame",
                "CommonUser",
                "NavigationSystem",
				"AIModule",
                "GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
				"AnimGraphRuntime",
				"OnlineSubsystemNull"
			});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
