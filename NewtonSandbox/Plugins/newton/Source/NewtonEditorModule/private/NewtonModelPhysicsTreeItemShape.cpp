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


#include "NewtonModelPhysicsTreeItemShape.h"

FNewtonModelPhysicsTreeItemShape::FNewtonModelPhysicsTreeItemShape(const FNewtonModelPhysicsTreeItemShape& src)
	:FNewtonModelPhysicsTreeItem(src)
{
}

FNewtonModelPhysicsTreeItemShape::FNewtonModelPhysicsTreeItemShape(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode)
	:FNewtonModelPhysicsTreeItem(parentNode, modelNode)
{
	UNewtonLinkCollision* const shapeNodeInfo = Cast<UNewtonLinkCollision>(Node);
	check(shapeNodeInfo);

	check(m_parent);
	const FTransform parentTranform(m_parent->CalculateGlobalTransform());
	const FVector scale(parentTranform.GetScale3D());
	shapeNodeInfo->Transform.SetScale3D(FVector(1.0f / scale.X, 1.0f / scale.Y, 1.0f / scale.Z));

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
	//const UNewtonLinkCollision* const shapeNode = Cast<UNewtonLinkCollision>(Node);
	//check(shapeNode);
	//return shapeNode->Transform.ToMatrixNoScale();
	return CalculateGlobalTransform().ToMatrixNoScale();
}

void FNewtonModelPhysicsTreeItemShape::ApplyDeltaTransform(const FVector& inDrag, const FRotator& inRot, const FVector& inScale)
{
	UNewtonLinkCollision* const shapeNode = Cast<UNewtonLinkCollision>(Node);
	check(shapeNode);

	//UE_LOG(LogTemp, Warning, TEXT("%d %f %f %f"), mode, inDrag.X, inDrag.Y, inDrag.Z);
	if ((inDrag.X != 0.0f) || (inDrag.Y != 0.0f) || (inDrag.Z != 0.0f))
	{
		const FTransform parentTransform(m_parent->CalculateGlobalTransform());
		FTransform globalTransform(Node->Transform * parentTransform);

		globalTransform.SetLocation(globalTransform.GetLocation() + inDrag);
		shapeNode->Transform.SetLocation((globalTransform * parentTransform.Inverse()).GetLocation());
	}

	if ((inRot.Pitch != 0.0f) || (inRot.Yaw != 0.0f) || (inRot.Roll != 0.0f))
	{
		const FQuat deltaRot(inRot.Quaternion());
		FQuat rotation(deltaRot * shapeNode->Transform.GetRotation());
		rotation.Normalize();
		shapeNode->Transform.SetRotation(rotation);
	}
	
	if ((inScale.X != 0.0f) || (inScale.Y != 0.0f) || (inScale.Z != 0.0f))
	{
		FVector scale(shapeNode->Transform.GetScale3D() + inScale);
		scale.X = (scale.X < 0.01f) ? 0.01f : scale.X;
		scale.Y = (scale.Y < 0.01f) ? 0.01f : scale.Y;
		scale.Z = (scale.Z < 0.01f) ? 0.01f : scale.Z;
		shapeNode->Transform.SetScale3D(scale);
	}
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
	const FTransform tranform(CalculateGlobalTransform());
	for (int i = m_wireFrameMesh.Num() - 2; i >= 0; i -= 2)
	{
		FVector4 p0(tranform.TransformFVector4(m_wireFrameMesh[i + 0]));
		FVector4 p1(tranform.TransformFVector4(m_wireFrameMesh[i + 1]));
		pdi->DrawLine(p0, p1, FColor::Blue, SDPG_Foreground, thickness);
	}
}