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


#include "NewtonModelPhysicsTreeItemShapeConvexApproximate.h"
#include "NewtonModelEditor.h"

FNewtonModelPhysicsTreeItemShapeConvexApproximate::FNewtonModelPhysicsTreeItemShapeConvexApproximate(const FNewtonModelPhysicsTreeItemShapeConvexApproximate& src)
	:FNewtonModelPhysicsTreeItemShape(src)
{
	check(0);
}

FNewtonModelPhysicsTreeItemShapeConvexApproximate::FNewtonModelPhysicsTreeItemShapeConvexApproximate(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode, FNewtonModelEditor* const editor)
	:FNewtonModelPhysicsTreeItemShape(parentNode, modelNode, editor)
{
	UNewtonLinkCollisionConvexApproximate* const shapeNodeInfo = Cast<UNewtonLinkCollisionConvexApproximate>(m_node);
	check(shapeNodeInfo);

	check(m_parent);
	const FTransform parentTranform(m_parent->CalculateGlobalTransform());
	const FVector scale(parentTranform.GetScale3D());
	shapeNodeInfo->Transform.SetScale3D(FVector(1.0f / scale.X, 1.0f / scale.Y, 1.0f / scale.Z));

	if (shapeNodeInfo->m_hulls.Num())
	{
		shapeNodeInfo->CreateWireFrameMesh(m_wireFrameMesh);
	}
	else
	{
		UNewtonLinkRigidBody* const parentNodeInfo = Cast<UNewtonLinkRigidBody>(m_parent->GetNode());
		if (parentNodeInfo)
		{
			UNewtonAsset* const asset = m_editor->GetNewtonModel();
			shapeNodeInfo->CreateWireFrameMesh(m_wireFrameMesh, asset->SkeletalMeshAsset, parentNodeInfo->BoneIndex);
		}
		else
		{
			shapeNodeInfo->Transform = m_parent->m_node->Transform;
			shapeNodeInfo->Transform.SetScale3D(FVector(1.0f / scale.X, 1.0f / scale.Y, 1.0f / scale.Z));

			UNewtonLinkStaticMesh* const staticMeshParentNodeInfo = Cast<UNewtonLinkStaticMesh>(m_parent->GetNode());
			check(staticMeshParentNodeInfo);
			shapeNodeInfo->CreateWireFrameMesh(m_wireFrameMesh, staticMeshParentNodeInfo->StaticMesh);
			m_parent = m_parent->GetParent();
		}
	}
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemShapeConvexApproximate::Clone() const
{
	return new FNewtonModelPhysicsTreeItemShapeConvexApproximate(*this);
}