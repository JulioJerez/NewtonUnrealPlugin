// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FNewtonModelPhysicsTreeItem;

class FNewtonModelPhysicsTreeItemAcyclicGraph
{
	public:
	FNewtonModelPhysicsTreeItemAcyclicGraph(TSharedPtr<FNewtonModelPhysicsTreeItem> item, FNewtonModelPhysicsTreeItemAcyclicGraph* const parent);
	~FNewtonModelPhysicsTreeItemAcyclicGraph();

	void RemoveChild(FNewtonModelPhysicsTreeItemAcyclicGraph* child);

	TSharedPtr<FNewtonModelPhysicsTreeItem> m_item;
	FNewtonModelPhysicsTreeItemAcyclicGraph* m_parent;
	TArray<FNewtonModelPhysicsTreeItemAcyclicGraph*> m_children;
};