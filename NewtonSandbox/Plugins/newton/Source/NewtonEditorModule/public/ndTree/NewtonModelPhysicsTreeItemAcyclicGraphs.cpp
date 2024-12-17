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

