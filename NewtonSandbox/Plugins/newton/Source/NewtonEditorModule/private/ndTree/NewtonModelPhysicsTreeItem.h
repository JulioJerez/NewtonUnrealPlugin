// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "NewtonModelEditorCommon.h"

class FNewtonModelPhysicsTree;
class FNewtonModelPhysicsTreeItemAcyclicGraph;

class FNewtonModelPhysicsTreeItem: public TSharedFromThis<FNewtonModelPhysicsTreeItem>, public FGCObject
{
	public:
	NEWTON_INIT_RTTI(FNewtonModelPhysicsTreeItem)

	FNewtonModelPhysicsTreeItem(const FNewtonModelPhysicsTreeItem& src);
	FNewtonModelPhysicsTreeItem(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode);
	virtual ~FNewtonModelPhysicsTreeItem();

	virtual FNewtonModelPhysicsTreeItem* Clone() const;
	
	FName GetDisplayName() const;
	const FSlateBrush* GetIcon() const;
	void GenerateWidgetForNameColumn(TSharedPtr< SHorizontalBox > box, FIsSelected InIsSelected);

	protected:
	virtual FName BrushName() const;
	virtual FString GetReferencerName() const override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	// Display name of the category
	TObjectPtr<UNewtonModelNode> Node;

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

	FNewtonModelPhysicsTreeItemBody(const FNewtonModelPhysicsTreeItemBody& src);
	FNewtonModelPhysicsTreeItemBody(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode);

	virtual FName BrushName() const override;
	virtual FNewtonModelPhysicsTreeItem* Clone() const override;
};

class FNewtonModelPhysicsTreeItemBodyRoot : public FNewtonModelPhysicsTreeItemBody
{
	public:
	NEWTON_ADD_RTTI(FNewtonModelPhysicsTreeItemBodyRoot, FNewtonModelPhysicsTreeItemBody)

	FNewtonModelPhysicsTreeItemBodyRoot(const FNewtonModelPhysicsTreeItemBodyRoot& src);
	FNewtonModelPhysicsTreeItemBodyRoot(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode);

	virtual FName BrushName() const override;
	virtual FNewtonModelPhysicsTreeItem* Clone() const override;
};

class FNewtonModelPhysicsTreeItemShape : public FNewtonModelPhysicsTreeItem
{
	public:
	NEWTON_ADD_RTTI(FNewtonModelPhysicsTreeItemShape, FNewtonModelPhysicsTreeItem)

	FNewtonModelPhysicsTreeItemShape(const FNewtonModelPhysicsTreeItemShape& src);
	FNewtonModelPhysicsTreeItemShape(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode);

	virtual FName BrushName() const override;
	virtual FNewtonModelPhysicsTreeItem* Clone() const override;
};

class FNewtonModelPhysicsTreeItemJoint : public FNewtonModelPhysicsTreeItem
{
	public:
	NEWTON_ADD_RTTI(FNewtonModelPhysicsTreeItemJoint, FNewtonModelPhysicsTreeItem)

	FNewtonModelPhysicsTreeItemJoint(const FNewtonModelPhysicsTreeItemJoint& src);
	FNewtonModelPhysicsTreeItemJoint(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode);

	virtual FName BrushName() const override;
	virtual FNewtonModelPhysicsTreeItem* Clone() const override;
};
