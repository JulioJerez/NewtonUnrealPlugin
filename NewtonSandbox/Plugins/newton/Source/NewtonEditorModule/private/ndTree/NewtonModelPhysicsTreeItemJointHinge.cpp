// Copyright Epic Games, Inc. All Rights Reserved.

#include "ndTree/NewtonModelPhysicsTreeItemJointHinge.h"

FNewtonModelPhysicsTreeItemJointHinge::FNewtonModelPhysicsTreeItemJointHinge(const FNewtonModelPhysicsTreeItemJointHinge& src)
	:FNewtonModelPhysicsTreeItemJoint(src)
{
}

FNewtonModelPhysicsTreeItemJointHinge::FNewtonModelPhysicsTreeItemJointHinge(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode)
	:FNewtonModelPhysicsTreeItemJoint(parentNode)
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemJointHinge::Clone() const
{
	return new FNewtonModelPhysicsTreeItemJointHinge(*this);
}