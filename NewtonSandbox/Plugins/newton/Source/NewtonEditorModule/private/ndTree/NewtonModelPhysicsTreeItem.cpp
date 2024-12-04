// Copyright Epic Games, Inc. All Rights Reserved.

#include "ndTree/NewtonModelPhysicsTreeItem.h"

#include "ndTree/NewtonModelPhysicsTree.h"

FNewtonModelPhysicsTreeItem::FNewtonModelPhysicsTreeItem(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const FName& displayName)
{
	m_parent = parentNode;
	m_displayName = displayName;
}

const FName& FNewtonModelPhysicsTreeItem::GetDisplayName() const
{
	return m_displayName;
}

