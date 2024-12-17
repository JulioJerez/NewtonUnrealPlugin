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

#include "ndModel/NewtonLinkRigidBody.h"

#include "NewtonCommons.h"
#include "ndModel/NewtonLinkCollision.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkRigidBody::UNewtonLinkRigidBody()
	:Super()
{
	//;Transform	

	SetName(TEXT("NewBody"));
	BoneIndex = -1;

	Mass = 1.0f;
	LinearDamp = 0.0f;
	AngularDamp = 0.0f;
	DebugScale = 1.0f;
	AutoSleepMode = true;
	ShowCenterOfMass = true;
	//InitialVeloc(0.0f, 0.0f, 0.0f)
	//InitialOmega(0.0f, 0.0f, 0.0f)
	//CenterOfMass(0.0f, 0.0f, 0.0f)
	Gravity = FVector(0.0f, 0.0f, -980.0f);

	float inertia = (2.0f / 5.0f) * 0.5f * 0.5f;
	Inertia.PrincipalInertia = FVector(inertia, inertia, inertia);
}

FVector UNewtonLinkRigidBody::CalculateLocalCenterOfMass(const TArray<const UNewtonLinkCollision*>& childen) const
{
	FVector com(0.0f, 0.0f, 0.0f);
	if (childen.Num() == 1)
	{
		ndBodyKinematic body;
		const UNewtonLinkCollision* const shapeNopde = childen[0];
		ndShapeInstance shape(shapeNopde->CreateInstance());

		const ndMatrix bodyMatrix(ToNewtonMatrix(Transform));
		const ndMatrix shapeMatrix(ToNewtonMatrix(shapeNopde->Transform));
		const ndMatrix shapeLocalMatrix(shapeMatrix * bodyMatrix.OrthoInverse());

		body.SetMatrix(bodyMatrix);
			
		//FTransform tranform;
		//tranform.SetRotation(FQuat(Inertia.PrincipalInertiaAxis));
		//const ndMatrix shapeInestiaMatrix(ToNewtonMatrix(tranform));
		//shape->SetLocalMatrix(shapeInestiaMatrix * shapeLocalMatrix);
		shape.SetLocalMatrix(shapeLocalMatrix);

		bool fullInertia = ndAbs(Inertia.PrincipalInertiaAxis.Pitch) > 0.1f;
		fullInertia = fullInertia || (ndAbs(Inertia.PrincipalInertiaAxis.Yaw) > 0.1f);
		fullInertia = fullInertia || (ndAbs(Inertia.PrincipalInertiaAxis.Roll) > 0.1f);
		//body.SetIntrinsicMassMatrix(1.0fMass, shape, fullInertia);
		body.SetIntrinsicMassMatrix(1.0f, shape, fullInertia);

		ndVector centerOfGravity(body.GetCentreOfMass());
		//centerOfGravity += ndVector(ndFloat32(CenterOfMass.X * UNREAL_INV_UNIT_SYSTEM), ndFloat32(CenterOfMass.Y * UNREAL_INV_UNIT_SYSTEM), ndFloat32(CenterOfMass.Z * UNREAL_INV_UNIT_SYSTEM), ndFloat32(0.0f));
		//centerOfGravity = matrix.TransformVector(centerOfGravity);
		//
		//const FTransform tranform(ToUnRealTransform(matrix));
		//const FRotator axisRot(tranform.GetRotation());
		//const FVector axisLoc(centerOfGravity.m_x * UNREAL_UNIT_SYSTEM, centerOfGravity.m_y * UNREAL_UNIT_SYSTEM, centerOfGravity.m_z * UNREAL_UNIT_SYSTEM);
		//DrawDebugCoordinateSystem(GetWorld(), axisLoc, axisRot, DebugScale * UNREAL_UNIT_SYSTEM, false, timestep);

		com.X = centerOfGravity.m_x * UNREAL_UNIT_SYSTEM;
		com.Y = centerOfGravity.m_y * UNREAL_UNIT_SYSTEM;
		com.Z = centerOfGravity.m_z * UNREAL_UNIT_SYSTEM;
	}
	else if (childen.Num() > 1)
	{
		check(0);
		//for (int i = childen.Num() - 1; i >= 0; --i)
		//{
		//	const UNewtonLinkCollision* const shapeNopde = childen[i];
		//	ndShapeInstance instance(shapeNopde->CreateInstance());
		//}
	}

	return com;
}