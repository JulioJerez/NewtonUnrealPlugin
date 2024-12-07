// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ndTree/NewtonModelPhysicsTreeItem.h"


class FNewtonModelPhysicsTreeItemJointHinge : public FNewtonModelPhysicsTreeItemJoint
{
	public:
	NEWTON_ADD_RTTI(FNewtonModelPhysicsTreeItemJointHinge, FNewtonModelPhysicsTreeItemJoint)

	FNewtonModelPhysicsTreeItemJointHinge(const FNewtonModelPhysicsTreeItemJointHinge& src);
	FNewtonModelPhysicsTreeItemJointHinge(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode);

	virtual FNewtonModelPhysicsTreeItem* Clone() const override;
};
