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


#include "NewtonModelPhysicsTreeItemLoopEffector6dof.h"

#include "NewtonModelPhysicsTree.h"
#include "NewtonModelPhysicsTreeItemBody.h"
#include "NewtonModelPhysicsTreeItemJoint.h"

FNewtonModelPhysicsTreeItemLoopEffector6dof::FNewtonModelPhysicsTreeItemLoopEffector6dof(const FNewtonModelPhysicsTreeItemLoopEffector6dof& src)
	:FNewtonModelPhysicsTreeItemLoop(src)
{
}

FNewtonModelPhysicsTreeItemLoopEffector6dof::FNewtonModelPhysicsTreeItemLoopEffector6dof(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode, FNewtonModelEditor* const editor)
	:FNewtonModelPhysicsTreeItemLoop(parentNode, modelNode, editor)
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemLoopEffector6dof::Clone() const
{
	return new FNewtonModelPhysicsTreeItemLoopEffector6dof(*this);
}


void FNewtonModelPhysicsTreeItemLoopEffector6dof::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	const UNewtonLinkLoopEffector6dof* const jointNode = Cast<UNewtonLinkLoopEffector6dof>(m_node);
	check(jointNode);

	if (jointNode->m_hidden || !jointNode->ShowDebug)
	{
		return;
	}

	float thickness = NEWTON_EDITOR_DEBUG_THICKENESS;
	const FColor pinColor(NEWTON_EDITOR_DEBUG_JOINT_COLOR);

	FMatrix matrix(GetWidgetMatrix());
	float scale = jointNode->DebugScale;
	
	const FVector pinDir(matrix.GetUnitAxis(EAxis::X));
	const FVector pinStart(matrix.GetOrigin());
	const FVector pinEnd(pinStart + pinDir * (scale * 0.5f * 100.0f));
	
	FMatrix coneMatrix(matrix);
	coneMatrix.SetOrigin(pinEnd);
	DrawCone(pdi, coneMatrix, pinColor);
	pdi->DrawLine(pinStart, pinEnd, pinColor, SDPG_Foreground, thickness);
	
	if (jointNode->EnableLimits)
	{
		const int numSides = 16;
		const float size = scale * 25.0f;
		const float degToRad = PI / 180.0f;
		const float angle = jointNode->MinAngle * degToRad;
		const float deltaAngle = degToRad * (jointNode->MaxAngle - jointNode->MinAngle) / numSides;
		const float sinAngle = FMath::Sin(angle);
		const float cosAngle = FMath::Cos(angle);
	
		FMatrix localRotation(FMatrix::Identity);
		localRotation.M[1][1] = cosAngle;
		localRotation.M[1][2] = sinAngle;
		localRotation.M[2][2] = cosAngle;
		localRotation.M[2][1] = -sinAngle;
		FVector point(localRotation.TransformFVector4(FVector(0.0f, size, 0.0f)));
	
		const float deltaSinAngle = FMath::Sin(deltaAngle);
		const float deltaCosAngle = FMath::Cos(deltaAngle);
		localRotation.M[1][1] = deltaCosAngle;
		localRotation.M[1][2] = deltaSinAngle;
		localRotation.M[2][2] = deltaCosAngle;
		localRotation.M[2][1] = -deltaSinAngle;
	
		FVector base[numSides + 1];
	
		const FVector origin(matrix.GetOrigin());
		for (int32 i = 0; i <= numSides; ++i)
		{
			FVector p(matrix.TransformFVector4(point));
			base[i] = p;
			pdi->DrawLine(origin, p, pinColor, SDPG_Foreground, thickness);
			point = localRotation.TransformFVector4(point);
		}
	
		for (int32 i = 0; i < numSides; ++i)
		{
			pdi->DrawLine(base[i], base[i + 1], pinColor, SDPG_Foreground, thickness);
		}
	}
}

void FNewtonModelPhysicsTreeItemLoopEffector6dof::OnPropertyChange(const FPropertyChangedEvent& event)
{
	FNewtonModelPhysicsTreeItemLoop::OnPropertyChange(event);
	FProperty* const property = event.Property;
	if (property->GetName() != TEXT("BoneIndex"))
	{
		return;
	}

	check(GetParent());
	UNewtonLinkLoop* const loopNode = Cast<UNewtonLinkLoop>(m_node);
	TSharedPtr<FNewtonModelPhysicsTreeItem> bodyItem0(GetParent());
	const UNewtonLinkRigidBody* const body0 = Cast<UNewtonLinkRigidBody>(bodyItem0->GetNode());
	if (!body0 || (body0->BoneIndex < 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("LoopJoint parentBody is not mapped to a bone"));
		loopNode->BoneIndex = -1;
		loopNode->BoneName = TEXT("none");
		return;
	}

	TSharedPtr<FNewtonModelPhysicsTreeItem> bodyItem1(nullptr);
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
				bodyItem1 = itemInSet;
				break;
			}
		}
	}
	
	if (!bodyItem1)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoopJoint childBody not found"));
		loopNode->BoneIndex = -1;
		loopNode->BoneName = TEXT("none");
		return;
	}
	
	auto IsChildOf = [](TSharedPtr<FNewtonModelPhysicsTreeItem> child, TSharedPtr<FNewtonModelPhysicsTreeItem> parent)
	{
		for (TSharedPtr<FNewtonModelPhysicsTreeItem> node(child); node != nullptr; node = node->GetParent())
		{
			if (node == parent)
			{
				return true;
			}
		}
		return false;
	};

	TSharedPtr<FNewtonModelPhysicsTreeItem> childItem(nullptr);
	TSharedPtr<FNewtonModelPhysicsTreeItem> parentItem(nullptr);
	if (IsChildOf(bodyItem0, bodyItem1))
	{
		childItem = bodyItem0;
		parentItem = bodyItem1;
	}
	else if (IsChildOf(bodyItem1, bodyItem0))
	{
		childItem = bodyItem1;
		parentItem = bodyItem0;
	}

	if (!childItem || !parentItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("node are not part of the same ik chain"));
		loopNode->BoneIndex = -1;
		loopNode->BoneName = TEXT("none");
		return;
	}

	int dof = 0;
	TSharedPtr<FNewtonModelPhysicsTreeItem> node(childItem);
	do
	{
		if (node->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemJoint")))
		{
			const FNewtonModelPhysicsTreeItemJoint* const jointItem = (FNewtonModelPhysicsTreeItemJoint*)node.Get();
			dof += jointItem->GetFreeDof();
		}
		node = node->GetParent();
	} while (node && (node != parentItem));

	if (dof < 6)
	{
		UE_LOG(LogTemp, Warning, TEXT("selected bone is in the ik chain but the chain is underdetermined"));
		loopNode->BoneIndex = -1;
		loopNode->BoneName = TEXT("none");
		return;
	}

	const UNewtonLinkRigidBody* const body1 = Cast<UNewtonLinkRigidBody>(bodyItem1->GetNode());
	check(body1->BoneIndex == loopNode->BoneIndex);
	FString jointName(body1->BoneName.GetPlainNameString() + TEXT("_effector6dof"));
	loopNode->BoneName = m_editor->GetNewtonModelPhysicsTree()->GetUniqueName(FName(*jointName));
} 