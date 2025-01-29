// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class NewtonSandboxTarget : TargetRules
{
	public NewtonSandboxTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
        //DefaultBuildSettings = BuildSettingsVersion.V5;
        //IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.Add("NewtonSandbox");
    }
}
