using UnrealBuildTool;

public class NewtonRuntimeModule: ModuleRules
{
    public NewtonRuntimeModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDefinitions.Add("_D_CORE_DLL");
        PrivateDefinitions.Add("_D_NEWTON_DLL");
        PrivateDefinitions.Add("_D_COLLISION_DLL");

        PrivateIncludePaths.AddRange(
            new string[] 
            {
                "ThirdParty/newtonLibrary/Public/dCore/",
                "ThirdParty/newtonLibrary/Public/dNewton/",
                "ThirdParty/newtonLibrary/Public/dCollision/",
                "ThirdParty/newtonLibrary/Public/thirdParty/",
                "ThirdParty/newtonLibrary/Public/dNewton/dJoints/",
                "ThirdParty/newtonLibrary/Public/dNewton/dModels/",
                "ThirdParty/newtonLibrary/Public/dNewton/dIkSolver/",
                "ThirdParty/newtonLibrary/Public/dNewton/dModels/dVehicle/"
            }
        );


        PrivateDependencyModuleNames.AddRange(new string[] 
        {
            "Core",
            "Slate",
            "Engine",
            "UnrealEd",
            "Projects",
            "SlateCore",
            "Landscape",
            "CoreUObject",
            "PhysicsCore",
            "GeometryCore",
            "EditorFramework",
            "GeometryFramework",

			// ... add other public dependencies that you statically link with here ...
			"newtonLibrary"
        });
    }
}
