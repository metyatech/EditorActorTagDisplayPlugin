// Copyright 2026 Udon-Tobira. All Rights Reserved.

using UnrealBuildTool;

public class EditorActorTagDisplay : ModuleRules
{
    public EditorActorTagDisplay(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        PrecompileForTargets = PrecompileTargetsType.Editor;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "UnrealEd",
            "LevelEditor",
            "Slate",
            "SlateCore",
            "DeveloperSettings",
            "ToolMenus",
            "Settings"
        });
    }
}
