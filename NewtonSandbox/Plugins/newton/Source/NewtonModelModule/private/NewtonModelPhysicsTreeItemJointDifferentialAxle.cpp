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


#include "NewtonModelPhysicsTreeItemJointDifferentialAxle.h"

#include "NewtonModelEditor.h"
#include "NewtonModelPhysicsTree.h"
#include "NewtonModelPhysicsTreeItemBody.h"
#include "NewtonModelPhysicsTreeItemJoint.h"

FNewtonModelPhysicsTreeItemJointDifferentialAxle::FNewtonModelPhysicsTreeItemJointDifferentialAxle(const FNewtonModelPhysicsTreeItemJointDifferentialAxle& src)
	:FNewtonModelPhysicsTreeItemLoop(src)
{
}

FNewtonModelPhysicsTreeItemJointDifferentialAxle::FNewtonModelPhysicsTreeItemJointDifferentialAxle(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode, FNewtonModelEditor* const editor)
	:FNewtonModelPhysicsTreeItemLoop(parentNode, modelNode, editor)
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemJointDifferentialAxle::Clone() const
{
	return new FNewtonModelPhysicsTreeItemJointDifferentialAxle(*this);
}


void FNewtonModelPhysicsTreeItemJointDifferentialAxle::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	const UNewtonLinkJointLoopDifferentialAxle* const jointNode = Cast<UNewtonLinkJointLoopDifferentialAxle>(m_node);
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

void FNewtonModelPhysicsTreeItemJointDifferentialAxle::OnPropertyChange(const FPropertyChangedEvent& event)
{
	FNewtonModelPhysicsTreeItemLoop::OnPropertyChange(event);
	FProperty* const property = event.Property;
	if (property->GetName() != TEXT("BoneIndex"))
	{
		return;
	}

	check(GetParent());
	UNewtonLinkJointLoop* const loopNode = Cast<UNewtonLinkJointLoop>(m_node);
	TSharedPtr<FNewtonModelPhysicsTreeItem> parentBodyItem(GetParent());

	TSharedPtr<FNewtonModelPhysicsTreeItem> childBodyItem(nullptr);
	TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>> items(m_editor->GetNewtonModelPhysicsTree()->GetItems());
	for (TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>>::TConstIterator it(items); it; ++it)
	{
		TSharedPtr<FNewtonModelPhysicsTreeItem> itemInSet(*it);
		if (itemInSet->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")))
		{
			UNewtonLinkRigidBody* const node = Cast<UNewtonLinkRigidBody>(itemInSet->GetNode());
			check(node);
			if (node->BoneIndex == loopNode->BoneIndex)
			{
				childBodyItem = itemInSet;
				break;
			}
		}
	}
	
	if (!childBodyItem || (parentBodyItem == childBodyItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("LoopJoint childBody not found"));
		loopNode->BoneIndex = -1;
		loopNode->BoneName = TEXT("none");
		return;
	}

	const UNewtonLinkRigidBody* const childBody = Cast<UNewtonLinkRigidBody>(childBodyItem->GetNode());
	UNewtonLinkJointLoopDifferentialAxle* const effectorNode = Cast<UNewtonLinkJointLoopDifferentialAxle>(loopNode);
	check(childBody);
	check(effectorNode);
	check(childBody->BoneIndex == effectorNode->BoneIndex);
	
	effectorNode->BoneName = childBody->BoneName;
	effectorNode->TargetFrame = childBodyItem->CalculateGlobalTransform() * childBodyItem->CalculateGlobalTransform().Inverse();
} 