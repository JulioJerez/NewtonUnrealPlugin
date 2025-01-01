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


#pragma once

#include "Framework/Commands/Commands.h"
#include "HAL/Platform.h"
#include "Internationalization/Internationalization.h"
#include "Styling/AppStyle.h"
#include "Templates/SharedPointer.h"
#include "UObject/NameTypes.h"
#include "UObject/UnrealNames.h"

class FUICommandInfo;


class FNewtonModelPhysicsTreeCommands : public TCommands<FNewtonModelPhysicsTreeCommands>
{
	public:
	FNewtonModelPhysicsTreeCommands();

	//Initialize commands
	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> JointsVisibility;
	TSharedPtr<FUICommandInfo> CollisionsVisibility;

	TSharedPtr<FUICommandInfo> ShowDebug;
	TSharedPtr<FUICommandInfo> ClearDebug;
	TSharedPtr<FUICommandInfo> ResetSelectedBone;

	// create node commands
	TSharedPtr<FUICommandInfo> AddShapeBox;
	TSharedPtr<FUICommandInfo> AddShapeWheel;
	TSharedPtr<FUICommandInfo> AddShapeSphere;
	TSharedPtr<FUICommandInfo> AddShapeCapsule;
	TSharedPtr<FUICommandInfo> AddShapeCylinder;
	TSharedPtr<FUICommandInfo> AddShapeConvexhull;

	TSharedPtr<FUICommandInfo> AddJointHinge;
	TSharedPtr<FUICommandInfo> AddJointSlider;
	TSharedPtr<FUICommandInfo> AddJointRoller;

	TSharedPtr<FUICommandInfo> AddLoopEffector6dof;

	TSharedPtr<FUICommandInfo> DeleteSelectedRow;
};
