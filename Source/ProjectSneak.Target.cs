// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectSneakTarget : TargetRules
{
	public ProjectSneakTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		bOverrideBuildEnvironment = true;
		GlobalDefinitions.Add("UE_PROJECT_STEAMSHIPPINGID=4465440");
		ExtraModuleNames.Add("ProjectSneak");
	}
}
