// Copyright Epic Games, Inc. All Rights Reserved.

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

	// create node commands
	TSharedPtr<FUICommandInfo> AddShapeBox;
	TSharedPtr<FUICommandInfo> AddShapeSphere;
	TSharedPtr<FUICommandInfo> AddJointHinge;

	TSharedPtr<FUICommandInfo> DeleteSelectedRow;
};
