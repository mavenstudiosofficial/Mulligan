// Copyright Joram Wessels 2023

using UnrealBuildTool;

public class RooibosOutlinesEditor : ModuleRules
{
    public RooibosOutlinesEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "AssetTools", "RooibosOutlines", "UnrealEd"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine"
            }
        );
    }
}