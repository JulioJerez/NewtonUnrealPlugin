// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "NewtonModelEditorCommon.h"

class FNewtonModelPhysicsTree;
class FNewtonModelPhysicsTreeItemAcyclicGraph;

class FNewtonModelPhysicsTreeItem: public TSharedFromThis<FNewtonModelPhysicsTreeItem>
{
	public:
	NEWTON_INIT_RTTI(FNewtonModelPhysicsTreeItem)

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
	FNewtonModelPhysicsTreeItemAcyclicGraph* m_acyclicGraph;

	bool m_isHidden;

	friend class FNewtonModelPhysicsTree;
	friend class FNewtonModelPhysicsTreeItemAcyclicGraph;
};

class FNewtonModelPhysicsTreeItemBody : public FNewtonModelPhysicsTreeItem
{
	public:
	NEWTON_ADD_RTTI(FNewtonModelPhysicsTreeItemBody, FNewtonModelPhysicsTreeItem)

	FNewtonModelPhysicsTreeItemBody(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const FName& displayName)
		:FNewtonModelPhysicsTreeItem(parentNode, displayName)
	{
	}

	virtual FName BrushName() const override
	{
		return TEXT("bodyIcon.png");
	}
};

class FNewtonModelPhysicsTreeItemBodyRoot : public FNewtonModelPhysicsTreeItemBody
{
	public:
	NEWTON_ADD_RTTI(FNewtonModelPhysicsTreeItemBodyRoot, FNewtonModelPhysicsTreeItemBody)

	FNewtonModelPhysicsTreeItemBodyRoot(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const FName& displayName)
		:FNewtonModelPhysicsTreeItemBody(parentNode, displayName)
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
	NEWTON_ADD_RTTI(FNewtonModelPhysicsTreeItemShape, FNewtonModelPhysicsTreeItem)

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
	NEWTON_ADD_RTTI(FNewtonModelPhysicsTreeItemJoint, FNewtonModelPhysicsTreeItem)
	FNewtonModelPhysicsTreeItemJoint(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const FName& displayName)
		:FNewtonModelPhysicsTreeItem(parentNode, displayName)
	{
	}

	virtual FName BrushName() const override
	{
		return TEXT("jointIcon.png");
	}
};
