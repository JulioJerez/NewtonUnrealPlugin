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

FNewtonModelPhysicsTreeItemBodyRoot::FNewtonModelPhysicsTreeItemBodyRoot(const FNewtonModelPhysicsTreeItemBodyRoot& src)
	:FNewtonModelPhysicsTreeItemBody(src)
{
}

FNewtonModelPhysicsTreeItemBodyRoot::FNewtonModelPhysicsTreeItemBodyRoot(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonModelNode> modelNode)
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

FNewtonModelPhysicsTreeItemBody::FNewtonModelPhysicsTreeItemBody(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonModelNode> modelNode)
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
	const UNewtonModelNodeRigidBody* const body = Cast<UNewtonModelNodeRigidBody>(Node);
	check(body);

	if (body->ShowCenterOfMass && (body->BoneIndex >= 0))
	{
		// remember to get the com form the collision shape if there are some 
		FTransform com(body->Transform);
	
		const FVector position (com.GetLocation());
		const FVector xAxis (com.GetUnitAxis(EAxis::X));
		const FVector yAxis (com.GetUnitAxis(EAxis::Y));
		const FVector zAxis (com.GetUnitAxis(EAxis::Z));
	
		float thickness = 0.2f;
		float size = body->DebugScale * 25.0f;
		pdi->DrawLine(position, position + size * xAxis, FColor::Red, SDPG_Foreground, thickness);
		pdi->DrawLine(position, position + size * yAxis, FColor::Green, SDPG_Foreground, thickness);
		pdi->DrawLine(position, position + size * zAxis, FColor::Blue, SDPG_Foreground, thickness);
	}

	//const TSharedPtr<FNewtonModelPhysicsTreeItem>& item = m_item;
//if (item->Node->m_hidden)
//{
//	return;
//}
//
//UNewtonModelNode* const node = item->Node;
//	check(0);
//	//node->DebugDraw(view, viewport, pdi);
//}

}