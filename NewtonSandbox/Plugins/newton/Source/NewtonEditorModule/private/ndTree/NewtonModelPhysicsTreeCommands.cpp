// Copyright Epic Games, Inc. All Rights Reserved.

#include "ndTree/NewtonModelPhysicsTreeCommands.h"

#include "Framework/Commands/InputChord.h"
#include "Framework/Commands/UICommandInfo.h"
#include "GenericPlatform/GenericApplication.h"
#include "InputCoreTypes.h"

#define LOCTEXT_NAMESPACE "NewtonModelPhysicsTreeCommands"

FNewtonModelPhysicsTreeCommands::FNewtonModelPhysicsTreeCommands()
	: TCommands<FNewtonModelPhysicsTreeCommands>
	(
		TEXT("PhysicsTree"), // Context name for fast lookup
		NSLOCTEXT("Contexts", "PhysicsTree", "Physics Tree"), // Localized context name for displaying
		NAME_None, // Parent context name.  
		FAppStyle::GetAppStyleSetName() // Icon Style Set
	)
{
}

void FNewtonModelPhysicsTreeCommands::RegisterCommands()
{
	//UI_COMMAND(HideJoints, "Hide Joints", "hide all joints", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(JointsVisibility, "Joints Visilibilty", "filter joints visible state", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(CollisionsVisibility, "Collisions Visilibilty", "filter shapes visible state", EUserInterfaceActionType::ToggleButton, FInputChord());


	UI_COMMAND(AddJointHinge, "Add Hinge Joint", "Joint must be child of a rigid body ", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(AddShapeBox, "Add Box Shape", "collision shape must be a child of a rigid body", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddShapeSphere, "Add Sphere Shape", "collision shape must be a child of a rigid body", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(DeleteSelectedRow, "Delete", "delete this item and all it children", EUserInterfaceActionType::Button, FInputChord(EKeys::Platform_Delete));
}

#undef LOCTEXT_NAMESPACE
