using UnrealBuildTool;

public class NewtonEditorModule: ModuleRules
{
    public NewtonEditorModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core",
            "Slate",
            "Engine",
            "Projects",
            "UnrealEd",
            "SlateCore",
            "ToolMenus",
            "GeometryCore",
            "EditorFramework",
            "GeometryFramework",
            "NewtonRuntimeModule" });
    }
}
