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


#include "NewtonModelPhysicsTreeItemJointLoop6dofEffector.h"

#include "NewtonModelEditor.h"
#include "NewtonModelPhysicsTree.h"
#include "NewtonModelPhysicsTreeItemBody.h"
#include "NewtonModelPhysicsTreeItemJoint.h"
#include "NewtonModelPhysicsTreeItemAcyclicGraphs.h"

FNewtonModelPhysicsTreeItemJointLoop6dofEffector::FNewtonModelPhysicsTreeItemJointLoop6dofEffector(const FNewtonModelPhysicsTreeItemJointLoop6dofEffector& src)
	:FNewtonModelPhysicsTreeItemJointLoop(src)
{
}

FNewtonModelPhysicsTreeItemJointLoop6dofEffector::FNewtonModelPhysicsTreeItemJointLoop6dofEffector(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode, FNewtonModelEditor* const editor)
	:FNewtonModelPhysicsTreeItemJointLoop(parentNode, modelNode, editor)
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemJointLoop6dofEffector::Clone() const
{
	return new FNewtonModelPhysicsTreeItemJointLoop6dofEffector(*this);
}


void FNewtonModelPhysicsTreeItemJointLoop6dofEffector::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	const UNewtonLinkJointLoop6dofEffector* const jointNode = Cast<UNewtonLinkJointLoop6dofEffector>(m_node);
	check(jointNode);

	if (jointNode->m_hidden || !jointNode->ShowDebug)
	{
		return;
	}

	float scale = jointNode->DebugScale * 5.0f;
	float thickness = NEWTON_EDITOR_DEBUG_THICKENESS * 5.0f;

	const FTransform parentTransform(CalculateGlobalTransform());
	const FVector positionParent(parentTransform.GetLocation());
	const FVector xAxisParent(parentTransform.GetUnitAxis(EAxis::X));
	const FVector yAxisParent(parentTransform.GetUnitAxis(EAxis::Y));
	const FVector zAxisParent(parentTransform.GetUnitAxis(EAxis::Z));
	pdi->DrawLine(positionParent, positionParent + scale * xAxisParent, FColor::Red, SDPG_Foreground, thickness);
	pdi->DrawLine(positionParent, positionParent + scale * yAxisParent, FColor::Green, SDPG_Foreground, thickness);
	pdi->DrawLine(positionParent, positionParent + scale * zAxisParent, FColor::Blue, SDPG_Foreground, thickness);

	const FTransform childTransform(jointNode->TargetFrame * parentTransform);
	const FVector positionChild(childTransform.GetLocation());
	const FVector xAxisChild(childTransform.GetUnitAxis(EAxis::X));
	const FVector yAxisChild(childTransform.GetUnitAxis(EAxis::Y));
	const FVector zAxisChild(childTransform.GetUnitAxis(EAxis::Z));
	pdi->DrawLine(positionChild, positionChild + scale * xAxisChild, FColor::Red, SDPG_Foreground, thickness);
	pdi->DrawLine(positionChild, positionChild + scale * yAxisChild, FColor::Green, SDPG_Foreground, thickness);
	pdi->DrawLine(positionChild, positionChild + scale * zAxisChild, FColor::Blue, SDPG_Foreground, thickness);
}

void FNewtonModelPhysicsTreeItemJointLoop6dofEffector::PrepareNode() const
{
	FNewtonModelPhysicsTreeItemJointLoop::PrepareNode();

	UNewtonLinkJointLoop* const jointNode = Cast<UNewtonLinkJointLoop>(m_node);
	jointNode->m_selectionNames.Empty();

	int stack = 1;
	int dofSum[TREE_STACK_DEPTH];
	FNewtonModelPhysicsTreeItemAcyclicGraph* nodeStack[TREE_STACK_DEPTH];

	dofSum[0] = 0;
	nodeStack[0] = GetParent()->GetAcyclicGraph();
	while (stack)
	{
		stack--;
		int sum = dofSum[stack];
		FNewtonModelPhysicsTreeItemAcyclicGraph* const node = nodeStack[stack];

		sum += node->m_item->GetFreeDof();
		if (sum >= 6)
		{
			const UNewtonLinkRigidBody* const body = Cast<UNewtonLinkRigidBody>(node->m_item->GetNode());
			if (body)
			{
				jointNode->m_selectionNames.Push(body->Name);
			}
		}
		
		for (int i = node->m_children.Num() - 1; i >= 0; --i)
		{
			dofSum[stack] = sum;
			nodeStack[stack] = node->m_children[i];
			stack++;
		}
	}
}

void FNewtonModelPhysicsTreeItemJointLoop6dofEffector::OnPropertyChange(const FPropertyChangedEvent& event)
{
	FNewtonModelPhysicsTreeItemJointLoop::OnPropertyChange(event);
} 