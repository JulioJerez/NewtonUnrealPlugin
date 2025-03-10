using UnrealBuildTool;

public class NewtonModelModule: ModuleRules
{
    public NewtonModelModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicIncludePathModuleNames.AddRange(new string[]
        {
            "Persona",
            "UnrealEd"
        });

        PrivateDependencyModuleNames.AddRange(new string[] 
        {
            "RHI",
            "Core",
            "Slate",
            "Engine",
            "Kismet",
            "Persona",
            "Projects",
            "UnrealEd",
            "InputCore",
            "SlateCore",
            "AnimGraph",
            "ToolMenus",
            "AssetTools",
            "ToolWidgets",
            "GraphEditor",
            "CoreUObject",
            "GeometryCore",
            "AnimationCore",
            "EditorWidgets",
            "AssetRegistry",
            "PropertyEditor",
            "BlueprintGraph",
            "ContentBrowser",
            "ApplicationCore",
            "EditorFramework",
            "AnimGraphRuntime",
            "AnimationWidgets",
            "GeometryFramework",
            "PinnedCommandList",
            "SkeletalMeshUtilitiesCommon",

            "NewtonRuntimeModule"
        });
    }
}

