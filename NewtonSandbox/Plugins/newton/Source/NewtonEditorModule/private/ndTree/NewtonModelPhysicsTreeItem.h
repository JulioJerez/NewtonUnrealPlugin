/* Copyright (c) <2024-2024> <Julio Jerez, Newton Game Dynamics>
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
*
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any source distribution.
*/


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

	TObjectPtr<UNewtonModelNode> Node;
	TSharedPtr<FNewtonModelPhysicsTreeItem> m_parent;
	FNewtonModelPhysicsTreeItemAcyclicGraph* m_acyclicGraph;

	bool m_isHidden;

	friend class FNewtonModelPhysicsTree;
	friend class FNewtonModelPhysicsTreeItemAcyclicGraph;
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
