// Copyright Epic Games, Inc. All Rights Reserved.

#include "ndTree/NewtonModelPhysicsTreeItemShapeBox.h"

FNewtonModelPhysicsTreeItemShapeBox::FNewtonModelPhysicsTreeItemShapeBox(const FNewtonModelPhysicsTreeItemShapeBox& src)
	:FNewtonModelPhysicsTreeItemShape(src)
{
	check(0);
}

FNewtonModelPhysicsTreeItemShapeBox::FNewtonModelPhysicsTreeItemShapeBox(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode)
	:FNewtonModelPhysicsTreeItemShape(parentNode)
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemShapeBox::Clone() const
{
	return new FNewtonModelPhysicsTreeItemShapeBox(*this);
}