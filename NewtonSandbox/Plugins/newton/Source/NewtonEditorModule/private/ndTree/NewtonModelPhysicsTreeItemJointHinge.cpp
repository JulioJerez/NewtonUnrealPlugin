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


#include "ndTree/NewtonModelPhysicsTreeItemJointHinge.h"

FNewtonModelPhysicsTreeItemJointHinge::FNewtonModelPhysicsTreeItemJointHinge(const FNewtonModelPhysicsTreeItemJointHinge& src)
	:FNewtonModelPhysicsTreeItemJoint(src)
{
}

FNewtonModelPhysicsTreeItemJointHinge::FNewtonModelPhysicsTreeItemJointHinge(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonModelNode> modelNode)
	:FNewtonModelPhysicsTreeItemJoint(parentNode, modelNode)
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemJointHinge::Clone() const
{
	return new FNewtonModelPhysicsTreeItemJointHinge(*this);
}


void FNewtonModelPhysicsTreeItemJointHinge::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	const UNewtonModelNodeJointHinge* const jointNode = Cast<UNewtonModelNodeJointHinge>(Node);
	check(jointNode);

	if (jointNode->m_hidden)
	{
		return;
	}

	float thickness = NEWTON_EDITOR_DEBUG_THICKENESS;
	FMatrix matrix(GetWidgetMatrix());
	float scale = jointNode->DebugScale;
	const FColor pinColor(255.0f, 255.0f, 0.0f);

	const FVector pinDir(matrix.GetUnitAxis(EAxis::X));
	const FVector pingStart(matrix.GetOrigin());
	const FVector pingEnd(pingStart + pinDir * (scale * 0.9f * 100.0f));

	FMatrix coneMatrix(matrix);
	coneMatrix.SetOrigin(pingEnd);
	DrawCone(pdi, coneMatrix, pinColor);
	pdi->DrawLine(pingStart, pingEnd, pinColor, SDPG_Foreground, thickness);
}
