// Copyright Epic Games, Inc. All Rights Reserved.

#include "ndTree/NewtonModelPhysicsTreeItemShapeSphere.h"

FNewtonModelPhysicsTreeItemShapeSphere::FNewtonModelPhysicsTreeItemShapeSphere(const FNewtonModelPhysicsTreeItemShapeSphere& src)
	:FNewtonModelPhysicsTreeItemShape(src)
{
	check(0);
}

FNewtonModelPhysicsTreeItemShapeSphere::FNewtonModelPhysicsTreeItemShapeSphere(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode)
	:FNewtonModelPhysicsTreeItemShape(parentNode)
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemShapeSphere::Clone() const
{
	return new FNewtonModelPhysicsTreeItemShapeSphere(*this);
}