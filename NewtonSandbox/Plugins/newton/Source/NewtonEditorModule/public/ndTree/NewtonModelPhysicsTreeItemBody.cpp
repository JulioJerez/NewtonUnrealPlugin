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


#include "ndTree/NewtonModelPhysicsTreeItemBody.h"
#include "ndTree/NewtonModelPhysicsTreeItemAcyclicGraphs.h"

FNewtonModelPhysicsTreeItemBodyRoot::FNewtonModelPhysicsTreeItemBodyRoot(const FNewtonModelPhysicsTreeItemBodyRoot& src)
	:FNewtonModelPhysicsTreeItemBody(src)
{
}

FNewtonModelPhysicsTreeItemBodyRoot::FNewtonModelPhysicsTreeItemBodyRoot(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode)
	:FNewtonModelPhysicsTreeItemBody(parentNode, modelNode)
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemBodyRoot::Clone() const
{
	return new FNewtonModelPhysicsTreeItemBodyRoot(*this);
}


//***********************************************************************************
//
//***********************************************************************************
FNewtonModelPhysicsTreeItemBody::FNewtonModelPhysicsTreeItemBody(const FNewtonModelPhysicsTreeItemBody& src)
	:FNewtonModelPhysicsTreeItem(src)
{
}

FNewtonModelPhysicsTreeItemBody::FNewtonModelPhysicsTreeItemBody(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode)
	:FNewtonModelPhysicsTreeItem(parentNode, modelNode)
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemBody::Clone() const
{
	return new FNewtonModelPhysicsTreeItemBody(*this);
}

FName FNewtonModelPhysicsTreeItemBody::BrushName() const
{
	return TEXT("bodyIcon.png");
}

void FNewtonModelPhysicsTreeItemBody::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	const UNewtonLinkRigidBody* const bodyNode = Cast<UNewtonLinkRigidBody>(Node);
	check(bodyNode);

	if (bodyNode->BoneIndex >= 0)
	{
		if (bodyNode->ShowCenterOfMass)
		{
			// remember to get the com form the collision shape if there are some 
			FTransform com(bodyNode->Transform);

			const FVector position(com.GetLocation());
			const FVector xAxis(com.GetUnitAxis(EAxis::X));
			const FVector yAxis(com.GetUnitAxis(EAxis::Y));
			const FVector zAxis(com.GetUnitAxis(EAxis::Z));

			float size = bodyNode->DebugScale * 25.0f;
			float thickness = NEWTON_EDITOR_DEBUG_THICKENESS;
			pdi->DrawLine(position, position + size * xAxis, FColor::Red, SDPG_Foreground, thickness);
			pdi->DrawLine(position, position + size * yAxis, FColor::Green, SDPG_Foreground, thickness);
			pdi->DrawLine(position, position + size * zAxis, FColor::Blue, SDPG_Foreground, thickness);
		}
		else if (bodyNode->Inertia.ShowPrincipalAxis)
		{
			check(0);
		}
	}
}

bool FNewtonModelPhysicsTreeItemBody::HaveSelection() const
{
	const UNewtonLinkRigidBody* const bodyNode = Cast<UNewtonLinkRigidBody>(Node);
	check(bodyNode);
	return bodyNode->ShowDebug;
}

bool FNewtonModelPhysicsTreeItemBody::ShouldDrawWidget() const
{
	const UNewtonLinkRigidBody* const bodyNode = Cast<UNewtonLinkRigidBody>(Node);
	check(bodyNode);
	return bodyNode->ShowDebug && (bodyNode->Inertia.ShowPrincipalAxis ^ bodyNode->ShowCenterOfMass);
}

FMatrix FNewtonModelPhysicsTreeItemBody::GetWidgetMatrix() const
{
	UNewtonLinkRigidBody* const bodyNode = Cast<UNewtonLinkRigidBody>(Node);
	check(bodyNode);

	TArray<const UNewtonLinkCollision*> childrenShapes;
	for (int i = m_acyclicGraph->m_children.Num() - 1; i >= 0; --i)
	{
		TSharedPtr<FNewtonModelPhysicsTreeItem> childItem(m_acyclicGraph->m_children[i]->m_item);
		if (childItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemShape")))
		{
			childrenShapes.Push(Cast<UNewtonLinkCollision>(childItem->Node));
		}
	}


	FMatrix matrix (bodyNode->Transform.ToMatrixNoScale());
	if (bodyNode->ShowCenterOfMass)
	{
		const FVector com(bodyNode->CalculateLocalCenterOfMass(childrenShapes) + bodyNode->CenterOfMass);
		matrix.SetOrigin(matrix.TransformFVector4(com));
	}
	else
	{
		check(0);
		check(bodyNode->Inertia.ShowPrincipalAxis);
		matrix.SetOrigin(matrix.TransformFVector4(bodyNode->CenterOfMass));
	}

	return matrix;
}

void FNewtonModelPhysicsTreeItemBody::ApplyDeltaTransform(const FVector& inDrag, const FRotator& inRot, const FVector& inScale)
{
	UNewtonLinkRigidBody* const bodyNode = Cast<UNewtonLinkRigidBody>(Node);
	check(bodyNode);
	if (bodyNode->ShowCenterOfMass)
	{
		bodyNode->CenterOfMass += inDrag;
	}
	else
	{
		check(0);
		check(bodyNode->Inertia.ShowPrincipalAxis);
	}
}