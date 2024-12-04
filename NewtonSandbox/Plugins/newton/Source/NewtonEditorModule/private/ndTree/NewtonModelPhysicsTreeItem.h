// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FNewtonModelPhysicsTree;

class FNewtonModelPhysicsTreeItem
{
	public:
	FNewtonModelPhysicsTreeItem(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const FName& displayName);
	
	const FName& GetDisplayName() const;

	// Display name of the category
	FName m_displayName;
	TSharedPtr<FNewtonModelPhysicsTreeItem> m_parent;
};

