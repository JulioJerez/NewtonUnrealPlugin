// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ndTree/NewtonModelPhysicsTreeItem.h"


class FNewtonModelPhysicsTreeItemShapeSphere : public FNewtonModelPhysicsTreeItemShape
{
	public:
	NEWTON_ADD_RTTI(FNewtonModelPhysicsTreeItemShapeSphere, FNewtonModelPhysicsTreeItemShape)

	FNewtonModelPhysicsTreeItemShapeSphere(const FNewtonModelPhysicsTreeItemShapeSphere& src);
	FNewtonModelPhysicsTreeItemShapeSphere(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode);

	virtual FNewtonModelPhysicsTreeItem* Clone() const override;

};
