using UnrealBuildTool;

public class PSAssetSystem : ModuleRules
{
    public PSAssetSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "ProjectSneak", "Slate", "SlateCore" });
    }
}