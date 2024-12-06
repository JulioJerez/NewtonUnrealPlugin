// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ndTree/NewtonModelPhysicsTreeItem.h"


class FNewtonModelPhysicsTreeItemShapeBox : public FNewtonModelPhysicsTreeItemShape
{
	public:
	NEWTON_ADD_RTTI(FNewtonModelPhysicsTreeItemShapeBox, FNewtonModelPhysicsTreeItemShape)

	FNewtonModelPhysicsTreeItemShapeBox(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const FName& displayName);
	FNewtonModelPhysicsTreeItemShapeBox(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const UNewtonModelNodeCollisionBox* const node);
};
