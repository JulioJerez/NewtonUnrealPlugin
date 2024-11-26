using UnrealBuildTool;

public class NewtonEditorModule: ModuleRules
{
    public NewtonEditorModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicIncludePathModuleNames.AddRange(new string[]
        {
            "Persona",
            "UnrealEd"
        });

        PrivateDependencyModuleNames.AddRange(new string[] 
        {
            "Core",
            "Slate",
            "Engine",
            "Persona",
            "Projects",
            "UnrealEd",
            "SlateCore",
            "ToolMenus",
            "AssetTools",
            "CoreUObject",
            "ToolWidgets",
            "GraphEditor",
            "GeometryCore",
            "PropertyEditor",
            "EditorFramework",
            "GeometryFramework",
            "PinnedCommandList",
            "SkeletalMeshUtilitiesCommon",

            "NewtonRuntimeModule"
        });
    }
}
