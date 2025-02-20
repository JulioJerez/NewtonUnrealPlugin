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

void FNewtonModelPhysicsTreeItemJointLoop6dofEffector::OnPropertyChange(const FPropertyChangedEvent& event)
{
	check(0);
	//FNewtonModelPhysicsTreeItemJointLoop::OnPropertyChange(event);
	//FProperty* const property = event.Property;
	//if (property->GetName() != TEXT("BoneIndex"))
	//{
	//	return;
	//}
	//
	//check(GetParent());
	//UNewtonLinkJointLoop* const loopNode = Cast<UNewtonLinkJointLoop>(m_node);
	//TSharedPtr<FNewtonModelPhysicsTreeItem> bodyItem0(GetParent());
	//const UNewtonLinkRigidBody* const body0 = Cast<UNewtonLinkRigidBody>(bodyItem0->GetNode());
	//if (!body0 || (body0->BoneIndex < 0))
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("LoopJoint parentBody is not mapped to a bone"));
	//	loopNode->BoneIndex = -1;
	//	loopNode->BoneName = TEXT("none");
	//	return;
	//}
	//
	//TSharedPtr<FNewtonModelPhysicsTreeItem> bodyItem1(nullptr);
	//TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>> items(m_editor->GetNewtonModelPhysicsTree()->GetItems());
	//for (TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>>::TConstIterator it(items); it; ++it)
	//{
	//	TSharedPtr<FNewtonModelPhysicsTreeItem> itemInSet(*it);
	//	if (itemInSet->IsOfRttiByName(FNewtonModelPhysicsTreeItemBody::GetRtti()))
	//	{
	//		UNewtonLinkRigidBody* const node = Cast<UNewtonLinkRigidBody>(itemInSet->GetNode());
	//		check(node);
	//		if (node->BoneIndex == loopNode->BoneIndex)
	//		{
	//			bodyItem1 = itemInSet;
	//			break;
	//		}
	//	}
	//}
	//
	//if (!bodyItem1)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("LoopJoint childBody not found"));
	//	loopNode->BoneIndex = -1;
	//	loopNode->BoneName = TEXT("none");
	//	return;
	//}
	//
	//auto IsChildOf = [](TSharedPtr<FNewtonModelPhysicsTreeItem> child, TSharedPtr<FNewtonModelPhysicsTreeItem> parent)
	//{
	//	for (TSharedPtr<FNewtonModelPhysicsTreeItem> node(child); node != nullptr; node = node->GetParent())
	//	{
	//		if (node == parent)
	//		{
	//			return true;
	//		}
	//	}
	//	return false;
	//};
	//
	//TSharedPtr<FNewtonModelPhysicsTreeItem> childItem(nullptr);
	//TSharedPtr<FNewtonModelPhysicsTreeItem> parentItem(nullptr);
	//if (IsChildOf(bodyItem0, bodyItem1))
	//{
	//	childItem = bodyItem0;
	//	parentItem = bodyItem1;
	//}
	//else if (IsChildOf(bodyItem1, bodyItem0))
	//{
	//	childItem = bodyItem1;
	//	parentItem = bodyItem0;
	//}
	//
	//if (!childItem || !parentItem)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("node are not part of the same ik chain"));
	//	loopNode->BoneIndex = -1;
	//	loopNode->BoneName = TEXT("none");
	//	return;
	//}
	//
	//int dof = 0;
	//TSharedPtr<FNewtonModelPhysicsTreeItem> node(childItem);
	//do
	//{
	//	if (node->IsOfRttiByName(FNewtonModelPhysicsTreeItemJoint::GetRtti()))
	//	{
	//		const FNewtonModelPhysicsTreeItemJoint* const jointItem = (FNewtonModelPhysicsTreeItemJoint*)node.Get();
	//		dof += jointItem->GetFreeDof();
	//	}
	//	node = node->GetParent();
	//} while (node && (node != parentItem));
	//
	//if (dof < 6)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("selected bone is in the ik chain but the chain is underdetermined"));
	//	loopNode->BoneIndex = -1;
	//	loopNode->BoneName = TEXT("none");
	//	return;
	//}
	//
	//const UNewtonLinkRigidBody* const body1 = Cast<UNewtonLinkRigidBody>(bodyItem1->GetNode());
	//UNewtonLinkJointLoop6dofEffector* const effectorNode = Cast<UNewtonLinkJointLoop6dofEffector>(loopNode);
	//check(body1);
	//check(effectorNode);
	//check(body1->BoneIndex == effectorNode->BoneIndex);
	//
	//effectorNode->BoneName = body1->BoneName;
	//effectorNode->TargetFrame = childItem->CalculateGlobalTransform() * parentItem->CalculateGlobalTransform().Inverse();
} 