// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectSneak : ModuleRules
{
	public ProjectSneak(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(new string[] 
		{ 
			"VariantManagerContent", 
			"GameplayAbilities", 
			"GameplayTasks", 
			"GameplayTags",
			"AdvancedSessions"
		});
		
		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay", 
			"EnhancedInput",
			"AdvancedSessions",
			"OnlineSubsystem",
			"OnlineSubsystemUtils" 
		});
		
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
	}
}

