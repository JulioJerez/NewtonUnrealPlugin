// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class NewtonSandboxEditorTarget : TargetRules
{
	public NewtonSandboxEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
        //DefaultBuildSettings = BuildSettingsVersion.V5;
        //IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        //IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("NewtonSandbox");
	}
}
