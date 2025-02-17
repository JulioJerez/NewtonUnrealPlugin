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


#include "NewtonModelPhysicsTreeItemJointLoop.h"
#include "NewtonModelPhysicsTreeItemAcyclicGraphs.h"

FNewtonModelPhysicsTreeItemJointLoop::FNewtonModelPhysicsTreeItemJointLoop(const FNewtonModelPhysicsTreeItemJointLoop& src)
	:FNewtonModelPhysicsTreeItem(src)
{
}

FNewtonModelPhysicsTreeItemJointLoop::FNewtonModelPhysicsTreeItemJointLoop(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode, FNewtonModelEditor* const editor)
	:FNewtonModelPhysicsTreeItem(parentNode, modelNode, editor)
{
}

FName FNewtonModelPhysicsTreeItemJointLoop::BrushName() const
{
	return TEXT("loopIcon.png");
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemJointLoop::Clone() const
{
	return new FNewtonModelPhysicsTreeItemJointLoop(*this);
}

void FNewtonModelPhysicsTreeItemJointLoop::DrawCone(FPrimitiveDrawInterface* const pdi, const FMatrix& matrix, const FColor& color) const
{
	const int numSides = 16;
	const float size = 10.0f;
	const float angle = 2.0f * PI / numSides;
	const float sinAngle = FMath::Sin(angle);
	const float cosAngle = FMath::Cos(angle);

	FMatrix localRotation(FMatrix::Identity);
	localRotation.M[1][1] = cosAngle;
	localRotation.M[1][2] = sinAngle;
	localRotation.M[2][2] = cosAngle;
	localRotation.M[2][1] = -sinAngle;

	const FVector origin(matrix.GetOrigin());
	FVector point(-size, size * 0.5f, 0.0f);

	FVector base[numSides];
	float thickness = NEWTON_EDITOR_DEBUG_THICKENESS;
	for (int32 i = 0; i < numSides; ++i)
	{
		FVector p(matrix.TransformFVector4(point));
		base[i] = p;
		pdi->DrawLine(origin, p, color, SDPG_Foreground, thickness);
		point = localRotation.TransformFVector4(point);
	}

	FVector p0(base[numSides - 1]);
	for (int32 i = 0; i < numSides; ++i)
	{
		pdi->DrawLine(base[i], p0, color, SDPG_Foreground, thickness);
		p0 = base[i];
	}
}

bool FNewtonModelPhysicsTreeItemJointLoop::ShouldDrawWidget() const
{
	const UNewtonLink* const node = Cast<UNewtonLink>(m_node);
	check(node);
	return node->ShowDebug;
}

bool FNewtonModelPhysicsTreeItemJointLoop::HaveSelection() const
{
	const UNewtonLink* const node = Cast<UNewtonLink>(m_node);
	check(node);
	return node->ShowDebug;
}

void FNewtonModelPhysicsTreeItemJointLoop::ApplyDeltaTransform(const FVector& inDrag, const FRotator& inRot, const FVector& inScale)
{
	UNewtonLinkJointLoop* const loopNode = Cast<UNewtonLinkJointLoop>(m_node);
	check(loopNode);

	if ((inRot.Pitch != 0.0f) || (inRot.Yaw != 0.0) || (inRot.Roll != 0.0))
	{
		const FNewtonModelPhysicsTreeItemAcyclicGraph* const acyclicGraph = m_acyclicGraph;
		check(acyclicGraph->m_children.Num() == 1);
		FNewtonModelPhysicsTreeItemAcyclicGraph* const acyclicChild = acyclicGraph->m_children[0];
		check(acyclicChild->m_item->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")));
		UNewtonLinkRigidBody* const childBodyNode = Cast<UNewtonLinkRigidBody>(acyclicChild->m_item->m_node);
		check(childBodyNode);

		const FTransform parentTransform(m_parent->CalculateGlobalTransform());
		const FTransform childBodyTransform(childBodyNode->Transform * loopNode->Transform * parentTransform);

		const FTransform deltaTransform(inRot);
		FTransform jointTransform(loopNode->Transform * parentTransform * deltaTransform * parentTransform.Inverse());
		jointTransform.NormalizeRotation();
		loopNode->Transform.SetRotation(jointTransform.GetRotation());

		const FTransform bodyParentTransform(loopNode->Transform * parentTransform);
		childBodyNode->Transform = childBodyTransform * bodyParentTransform.Inverse();
		childBodyNode->Transform.NormalizeRotation();
	}
}

