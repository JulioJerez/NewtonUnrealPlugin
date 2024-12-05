// Fill out your copyright notice in the Description page of Project Settings.

#include "ndTree/NewtonModelPhysicsTreeItemAcyclicGraphs.h"
#include "ndTree/NewtonModelPhysicsTreeItem.h"


FNewtonModelPhysicsTreeItemAcyclicGraph::FNewtonModelPhysicsTreeItemAcyclicGraph(TSharedPtr<FNewtonModelPhysicsTreeItem> item, FNewtonModelPhysicsTreeItemAcyclicGraph* const parent)
	:m_item(item)
	,m_parent(parent)
	,m_children()
{
	m_item->m_acyclicGraph = this;
	if (m_parent)
	{
		m_parent->m_children.Push(this);
	}
}

FNewtonModelPhysicsTreeItemAcyclicGraph::~FNewtonModelPhysicsTreeItemAcyclicGraph()
{
	m_item->m_acyclicGraph = nullptr;
	for (int i = m_children.Num() - 1; i >= 0; --i)
	{
		delete m_children[i];
	}
}

void FNewtonModelPhysicsTreeItemAcyclicGraph::RemoveChild(FNewtonModelPhysicsTreeItemAcyclicGraph* const child)
{
	for (int i = m_children.Num() - 1; i >= 0; --i)
	{
		if (child == m_children[i])
		{
			m_children[i] = m_children[m_children.Num() - 1];
			m_children.SetNum(m_children.Num() - 1);
			break;
		}
	}
}
