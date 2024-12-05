// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FNewtonModelPhysicsTree;

class FNewtonModelPhysicsTreeItem: public TSharedFromThis<FNewtonModelPhysicsTreeItem>
{
	public:
	FNewtonModelPhysicsTreeItem(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const FName& displayName);
	virtual ~FNewtonModelPhysicsTreeItem();
	
	const FSlateBrush* GetIcon() const;
	const FName& GetDisplayName() const;
	void GenerateWidgetForNameColumn(TSharedPtr< SHorizontalBox > box, FIsSelected InIsSelected);

	protected:
	virtual FName BrushName() const = 0;

	// Display name of the category
	FName m_displayName;
	TSharedPtr<FNewtonModelPhysicsTreeItem> m_parent;

	friend class FNewtonModelPhysicsTree;
};


class FNewtonModelPhysicsTreeItemBody : public FNewtonModelPhysicsTreeItem
{
	public:
	FNewtonModelPhysicsTreeItemBody(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const FName& displayName)
		:FNewtonModelPhysicsTreeItem(parentNode, displayName)
	{
	}

	virtual FName BrushName() const override
	{
		return TEXT("bodyIcon.png");
	}
};

class FNewtonModelPhysicsTreeItemShape : public FNewtonModelPhysicsTreeItem
{
	public:
	FNewtonModelPhysicsTreeItemShape(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const FName& displayName)
		:FNewtonModelPhysicsTreeItem(parentNode, displayName)
	{
	}

	virtual FName BrushName() const override
	{
		return TEXT("shapeIcon.png");
	}
};

class FNewtonModelPhysicsTreeItemJoint : public FNewtonModelPhysicsTreeItem
{
	public:
	FNewtonModelPhysicsTreeItemJoint(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const FName& displayName)
		:FNewtonModelPhysicsTreeItem(parentNode, displayName)
	{
	}

	virtual FName BrushName() const override
	{
		return TEXT("jointIcon.png");
	}
};
