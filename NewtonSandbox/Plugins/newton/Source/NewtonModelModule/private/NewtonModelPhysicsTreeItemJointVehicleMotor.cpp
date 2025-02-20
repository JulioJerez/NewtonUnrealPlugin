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


#include "NewtonModelPhysicsTreeItemJointVehicleMotor.h"

FNewtonModelPhysicsTreeItemJointVehicleMotor::FNewtonModelPhysicsTreeItemJointVehicleMotor(const FNewtonModelPhysicsTreeItemJointVehicleMotor& src)
	:FNewtonModelPhysicsTreeItemJoint(src)
{
}

FNewtonModelPhysicsTreeItemJointVehicleMotor::FNewtonModelPhysicsTreeItemJointVehicleMotor(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode, FNewtonModelEditor* const editor)
	:FNewtonModelPhysicsTreeItemJoint(parentNode, modelNode, editor)
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemJointVehicleMotor::Clone() const
{
	return new FNewtonModelPhysicsTreeItemJointVehicleMotor(*this);
}

int FNewtonModelPhysicsTreeItemJointVehicleMotor::GetFreeDof() const
{
	return 1;
}

void FNewtonModelPhysicsTreeItemJointVehicleMotor::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	const UNewtonLinkJointVehicleMotor* const jointNode = Cast<UNewtonLinkJointVehicleMotor>(m_node);
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
}
