// Copyright Epic Games, Inc. All Rights Reserved.

#include "ndTree/NewtonModelPhysicsTreeItemJointHinge.h"

FNewtonModelPhysicsTreeItemJointHinge::FNewtonModelPhysicsTreeItemJointHinge(const FNewtonModelPhysicsTreeItemJointHinge& src)
	:FNewtonModelPhysicsTreeItemJoint(src)
{
	check(0);
}

FNewtonModelPhysicsTreeItemJointHinge::FNewtonModelPhysicsTreeItemJointHinge(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode)
	:FNewtonModelPhysicsTreeItemJoint(parentNode)
{
	check(0);
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemJointHinge::Clone() const
{
	return new FNewtonModelPhysicsTreeItemJointHinge(*this);
}