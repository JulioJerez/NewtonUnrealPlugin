// Copyright Epic Games, Inc. All Rights Reserved.

#include "ndTree/NewtonModelPhysicsTreeItemJointHinge.h"

FNewtonModelPhysicsTreeItemJointHinge::FNewtonModelPhysicsTreeItemJointHinge(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const UNewtonModelNodeJoint* const node)
	:FNewtonModelPhysicsTreeItemJoint(parentNode, node->Name)
{
}

FNewtonModelPhysicsTreeItemJointHinge::FNewtonModelPhysicsTreeItemJointHinge(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const FName& displayName)
	:FNewtonModelPhysicsTreeItemJoint(parentNode, displayName)
{
}