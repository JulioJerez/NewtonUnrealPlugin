/* Copyright (c) <2024-2024> <Julio Jerez, Newton Game Dynamics>
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
*
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any source distribution.
*/


#include "NewtonModelPhysicsTreeCommands.h"

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
	UI_COMMAND(JointsVisibility, "hide joints", "filter joints visible state", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(CollisionsVisibility, "hide collisions", "filter shapes visible state", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(ShowDebug, "Show all debug info", "Show all debug info", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ClearDebug, "Hide all debug info", "Hide all debug info", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ResetSelectedBone, "Reset mapped bone", "clear the selected bone", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(AddStaticMesh, "static mesh", "static mesh must be child of a rigid body ", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(AddJointHinge, "hinge", "Joint must be child of a rigid body ", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddJointSlider, "slider", "Joint must be child of a rigid body ", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddJointRoller, "roller", "Joint must be child of a rigid body ", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddJointWheel, "wheel", "Joint must be child of a rigid body ", EUserInterfaceActionType::Button, FInputChord());
	
	UI_COMMAND(AddJointLoop6dofEffector, "6 dof effector", "loop must be child of a rigid body", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(AddJointVehicleTire, "vehicle tire", "Joint must be child of the vehicle chassis body", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddJointVehicleMotor, "vehicle motor", "Joint must be child of the vehicle chassis body", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddJointVehicleDifferential, "vehicle differential", "Joint must be child of the vehicle chassis body", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddJointLoopVehicleGearBox, "vehicle gear box", "Joint must be child of the vehicle head differential", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddJointLoopVehicleTireAxle, "vehicle tire axle", "Joint must be child of the vehicle differential body", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddJointLoopVehicleDifferentialAxle, "vehicle differential axle", "Joint must be child of the vehicle differential body", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(AddShapeBox, "box", "collision shape must be a child of a rigid body", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddShapeSphere, "sphere", "collision shape must be a child of a rigid body", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddShapeCapsule, "capsule", "collision shape must be a child of a rigid body", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddShapeCylinder, "cylinder", "collision shape must be a child of a rigid body", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddShapeWheel, "wheel", "collision shape must be a child of a rigid body", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddShapeConvexhull, "convexhull", "collision shape must be a child of a rigid body", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddShapeConvexAggragate, "aggragate of convex shapes", "collision shape must be a child of a rigid body", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(DeleteSelectedRow, "Delete", "delete this item and all it children", EUserInterfaceActionType::Button, FInputChord(EKeys::Platform_Delete));
}

#undef LOCTEXT_NAMESPACE
