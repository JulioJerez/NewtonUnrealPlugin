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


#include "ndTree/NewtonModelPhysicsTreeItemShape.h"

FNewtonModelPhysicsTreeItemShape::FNewtonModelPhysicsTreeItemShape(const FNewtonModelPhysicsTreeItemShape& src)
	:FNewtonModelPhysicsTreeItem(src)
{
}

FNewtonModelPhysicsTreeItemShape::FNewtonModelPhysicsTreeItemShape(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode)
	:FNewtonModelPhysicsTreeItem(parentNode, modelNode)
{
	UNewtonLinkCollision* const shapeNodeInfo = Cast<UNewtonLinkCollision>(Node);
	check(shapeNodeInfo);
	shapeNodeInfo->CreateWireFrameMesh(m_wireFrameMesh);
}

FName FNewtonModelPhysicsTreeItemShape::BrushName() const
{
	return TEXT("shapeIcon.png");
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemShape::Clone() const
{
	check(0);
	return new FNewtonModelPhysicsTreeItemShape(*this);
}

bool FNewtonModelPhysicsTreeItemShape::HaveSelection() const
{
	const UNewtonLinkCollision* const shapeNode = Cast<UNewtonLinkCollision>(Node);
	check(shapeNode);
	return shapeNode->ShowDebug;
}

bool FNewtonModelPhysicsTreeItemShape::ShouldDrawWidget() const
{
	const UNewtonLinkCollision* const shapeNode = Cast<UNewtonLinkCollision>(Node);
	check(shapeNode);
	return shapeNode->ShowDebug;
}

FMatrix FNewtonModelPhysicsTreeItemShape::GetWidgetMatrix() const
{
	const UNewtonLinkCollision* const shapeNode = Cast<UNewtonLinkCollision>(Node);
	check(shapeNode);
	return shapeNode->Transform.ToMatrixNoScale();
}

void FNewtonModelPhysicsTreeItemShape::ApplyDeltaTransform(const FVector& inDrag, const FRotator& inRot, const FVector& inScale)
{
	UNewtonLinkCollision* const shapeNode = Cast<UNewtonLinkCollision>(Node);
	check(shapeNode);

	shapeNode->Transform.SetLocation(shapeNode->Transform.GetLocation() + inDrag);

	if ((inRot.Pitch != 0.0f) || (inRot.Yaw != 0.0) || (inRot.Roll != 0.0))
	{
		const FQuat deltaRot(inRot.Quaternion());
		FQuat rotation(deltaRot * shapeNode->Transform.GetRotation());
		rotation.Normalize();
		shapeNode->Transform.SetRotation(rotation);
	}

	FVector scale(shapeNode->Transform.GetScale3D() + inScale);
	scale.X = (scale.X < 0.1f) ? 0.1f : scale.X;
	scale.Y = (scale.Y < 0.1f) ? 0.1f : scale.Y;
	scale.Z = (scale.Z < 0.1f) ? 0.1f : scale.Z;
	shapeNode->Transform.SetScale3D(scale);
}

void FNewtonModelPhysicsTreeItemShape::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	const UNewtonLinkCollision* const shapeNode = Cast<UNewtonLinkCollision>(Node);
	check(shapeNode);

	if (shapeNode->m_hidden || !shapeNode->ShowDebug)
	{
		return;
	}

	check(m_parent->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")));
	const UNewtonLinkRigidBody* const bodyNode = Cast<UNewtonLinkRigidBody>(m_parent->Node);
	check(bodyNode);
	if (bodyNode->BoneIndex < 0)
	{
		return;
	}

	float thickness = NEWTON_EDITOR_DEBUG_THICKENESS;
	const FTransform tranform(shapeNode->Transform);
	for (int i = m_wireFrameMesh.Num() - 2; i >= 0; i -= 2)
	{
		FVector4 p0(tranform.TransformFVector4(m_wireFrameMesh[i + 0]));
		FVector4 p1(tranform.TransformFVector4(m_wireFrameMesh[i + 1]));
		pdi->DrawLine(p0, p1, FColor::Blue, SDPG_Foreground, thickness);
	}
}