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


#include "NewtonModelPhysicsTreeItemJointLoopVehicleTireAxle.h"

#include "NewtonModelEditor.h"
#include "NewtonModelPhysicsTree.h"
#include "NewtonModelPhysicsTreeItemBody.h"
#include "NewtonModelPhysicsTreeItemJoint.h"

FNewtonModelPhysicsTreeItemJointLoopVehicleTireAxle::FNewtonModelPhysicsTreeItemJointLoopVehicleTireAxle(const FNewtonModelPhysicsTreeItemJointLoopVehicleTireAxle& src)
	:FNewtonModelPhysicsTreeItemJointLoop(src)
{
}

FNewtonModelPhysicsTreeItemJointLoopVehicleTireAxle::FNewtonModelPhysicsTreeItemJointLoopVehicleTireAxle(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode, FNewtonModelEditor* const editor)
	:FNewtonModelPhysicsTreeItemJointLoop(parentNode, modelNode, editor)
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemJointLoopVehicleTireAxle::Clone() const
{
	return new FNewtonModelPhysicsTreeItemJointLoopVehicleTireAxle(*this);
}

void FNewtonModelPhysicsTreeItemJointLoopVehicleTireAxle::PrepareNode() const
{
	FNewtonModelPhysicsTreeItemJointLoop::PrepareNode();
	
	UNewtonLinkJointLoop* const jointNode = Cast<UNewtonLinkJointLoop>(m_node);
	jointNode->m_selectionNames.Empty();

	TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>> items(m_editor->GetNewtonModelPhysicsTree()->GetItems());
	for (TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>>::TConstIterator it(items); it; ++it)
	{
		TSharedPtr<FNewtonModelPhysicsTreeItem> itemInSet(*it);
		if (Cast<UNewtonLinkRigidBody>(itemInSet->GetNode()))
		{
			TSharedPtr<FNewtonModelPhysicsTreeItem> parentItem(itemInSet->GetParent());
			if (parentItem.IsValid())
			{
				TObjectPtr<UNewtonLink> link(parentItem->GetNode());
				if (Cast<UNewtonLinkJointVehicleTire>(link))
				{
					jointNode->m_selectionNames.Push(link->Name);
				}
			}
		}
	}
}

void FNewtonModelPhysicsTreeItemJointLoopVehicleTireAxle::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	const UNewtonLinkJointLoop* const jointNode = Cast<UNewtonLinkJointLoop>(m_node);

	check(jointNode);

	if (jointNode->m_hidden || !jointNode->ShowDebug)
	{
		return;
	}

	float scale = jointNode->DebugScale * 5.0f;
	float thickness = NEWTON_EDITOR_DEBUG_THICKENESS * 5.0f;
	const FTransform parentTransform(CalculateGlobalTransform());

	const UNewtonLinkJointLoopVehicleTireAxle* const axle = Cast<UNewtonLinkJointLoopVehicleTireAxle>(jointNode);
	check(axle);

	FTransform gear;
	if (axle->ReverseGear)
	{
		const FRotator rotator(0.0f, 0.0f, 180.0f);
		gear.SetRotation(FQuat(rotator));
	}
	const FTransform differentialTransform(gear * parentTransform);
	const FVector positionParent(differentialTransform.GetLocation());
	const FVector xAxisParent(differentialTransform.GetUnitAxis(EAxis::X));
	const FVector yAxisParent(differentialTransform.GetUnitAxis(EAxis::Y));
	const FVector zAxisParent(differentialTransform.GetUnitAxis(EAxis::Z));
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

void FNewtonModelPhysicsTreeItemJointLoopVehicleTireAxle::OnPropertyChange(const FPropertyChangedEvent& event)
{
	FNewtonModelPhysicsTreeItemJointLoop::OnPropertyChange(event);
} 