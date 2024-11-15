using UnrealBuildTool;

public class NewtonEditorModule: ModuleRules
{
    public NewtonEditorModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] 
        {
            "Core",
            "Slate",
            "Engine",
            "Projects",
            "UnrealEd",
            "SlateCore",
            "ToolMenus",
            "AssetTools",
            "CoreUObject",
            "GraphEditor",
            "GeometryCore",
            "PropertyEditor",
            "EditorFramework",
            "GeometryFramework",
            //"EditorWidgets",
            //"ContentBrowser",
            //"ApplicationCore",
            //"NavigationSystem",
            "NewtonRuntimeModule"
        });
    }
}
