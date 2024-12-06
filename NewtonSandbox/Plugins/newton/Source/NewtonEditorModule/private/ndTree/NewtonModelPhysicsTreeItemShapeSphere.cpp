// Copyright Epic Games, Inc. All Rights Reserved.

#include "ndTree/NewtonModelPhysicsTreeItemShapeSphere.h"

FNewtonModelPhysicsTreeItemShapeSphere::FNewtonModelPhysicsTreeItemShapeSphere(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const UNewtonModelNodeCollisionSphere* const node)
	:FNewtonModelPhysicsTreeItemShape(parentNode, node->Name)
{
}

FNewtonModelPhysicsTreeItemShapeSphere::FNewtonModelPhysicsTreeItemShapeSphere(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const FName& displayName)
	:FNewtonModelPhysicsTreeItemShape(parentNode, displayName)
{
}