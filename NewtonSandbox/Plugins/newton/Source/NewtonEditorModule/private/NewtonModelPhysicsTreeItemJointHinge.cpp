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


#include "NewtonModelPhysicsTreeItemJointHinge.h"

FNewtonModelPhysicsTreeItemJointHinge::FNewtonModelPhysicsTreeItemJointHinge(const FNewtonModelPhysicsTreeItemJointHinge& src)
	:FNewtonModelPhysicsTreeItemJoint(src)
{
}

FNewtonModelPhysicsTreeItemJointHinge::FNewtonModelPhysicsTreeItemJointHinge(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode, const FNewtonModelEditor* const editor)
	:FNewtonModelPhysicsTreeItemJoint(parentNode, modelNode, editor)
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemJointHinge::Clone() const
{
	return new FNewtonModelPhysicsTreeItemJointHinge(*this);
}


void FNewtonModelPhysicsTreeItemJointHinge::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	const UNewtonLinkJointHinge* const jointNode = Cast<UNewtonLinkJointHinge>(m_node);
	check(jointNode);

	if (jointNode->m_hidden || !jointNode->ShowDebug)
	{
		return;
	}

	float thickness = NEWTON_EDITOR_DEBUG_THICKENESS;
	const FColor pinColor(NEWTON_EDITOR_DEBUG_COLOR);

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
