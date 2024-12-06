// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ndTree/NewtonModelPhysicsTreeItem.h"


class FNewtonModelPhysicsTreeItemJointHinge : public FNewtonModelPhysicsTreeItemJoint
{
	public:
	NEWTON_ADD_RTTI(FNewtonModelPhysicsTreeItemJointHinge, FNewtonModelPhysicsTreeItemJoint)

	FNewtonModelPhysicsTreeItemJointHinge(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const FName& displayName);
	FNewtonModelPhysicsTreeItemJointHinge(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const UNewtonModelNodeJoint* const node);
};
