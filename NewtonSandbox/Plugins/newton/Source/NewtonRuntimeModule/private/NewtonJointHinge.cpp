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

#include "NewtonJointHinge.h"

#include "NewtonRigidBody.h"
#include "NewtonWorldActor.h"
#include "NewtonRuntimeModule.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonJointHinge::UNewtonJointHinge()
	:Super()
{
	MaxAngle = 45.0f;
	MinAngle = -45.0f;
	SpringConst = 0.0f;
	DampingConst = 0.0f;
	EnableLimits = false;
	ProptionalDerivativeAngle = 0.0f;
	SpringDamperRegularizer = 1.0e-3f;
}

void UNewtonJointHinge::DrawGizmo(float timestep) const
{
	ndFloat32 scale = DebugScale * UNREAL_UNIT_SYSTEM;
	const FTransform transform(GetComponentTransform());
	const ndMatrix matrix(ToNewtonMatrix(transform));
	const FColor pinColor(255.0f, 255.0f, 0.0f);
	const FVector pinStart(transform.GetLocation());

	const UWorld* const world = GetWorld();

	UNewtonRigidBody* const child = FindChild();
	if (EnableLimits && child)
	{
		if (m_joint)
		{
			ndMatrix childMatrix(m_joint->GetLocalMatrix0() * ToNewtonMatrix(child->m_globalTransform));
			const ndVector hingeAnglePin(childMatrix.m_up.Scale(scale * 0.9f));
			const FVector hingeAngleEnd(float(pinStart.X + hingeAnglePin.m_x), float(pinStart.Y + hingeAnglePin.m_y), float(pinStart.Z + hingeAnglePin.m_z));
			DrawDebugLine(world, pinStart, hingeAngleEnd, pinColor, false, timestep);
		}

		TArray<int32> indices;
		TArray<FVector> verts;
		float deltaTwist = MaxAngle - MinAngle;
		if ((deltaTwist > 1.0e-3f) && (deltaTwist < 360.0f))
		{
			const ndVector point(ndFloat32(0.0f), scale, ndFloat32(0.0f), ndFloat32(0.0f));
			const ndInt32 subdiv = 12;
			ndFloat32 angle0 = MinAngle;
			ndFloat32 angleStep = ndMin(deltaTwist, 360.0f) / subdiv;
		
			const FVector parentOrigin(transform.GetLocation());
			const ndMatrix parentMatrix(ToNewtonMatrix(transform));
			verts.Push(parentOrigin);
			for (ndInt32 i = 0; i <= subdiv; ++i)
			{
				const ndVector p(parentMatrix.RotateVector(ndPitchMatrix(angle0 * ndDegreeToRad).RotateVector(point)));
				const FVector p1(float(p.m_x + parentOrigin.X), float(p.m_y + parentOrigin.Y), float(p.m_z + parentOrigin.Z));
				angle0 += angleStep;
				verts.Push(p1);
			}
			for (ndInt32 i = 0; i < subdiv; ++i)
			{
				indices.Push(0);
				indices.Push(i + 1);
				indices.Push(i + 2);
			}
			DrawDebugMesh(world, verts, indices, ND_DEBUG_MESH_COLOR, false, timestep);
		}
	}
	else
	{
		const ndVector pinDir(matrix.m_front.Scale(scale * 0.9f));
		const FVector coneDir(matrix.m_front.m_x, matrix.m_front.m_y, matrix.m_front.m_z);
		const FVector pinEnd(float(pinStart.X + pinDir.m_x), float(pinStart.Y + pinDir.m_y), float(pinStart.Z + pinDir.m_z));
		DrawDebugLine(world, pinStart, pinEnd, pinColor, false, timestep);
		DrawDebugCone(world, pinEnd, coneDir, -scale * 0.125f, 15.0f * ndDegreeToRad, 15.0f * ndDegreeToRad, 8, pinColor, false, timestep);
	}
}

// Called when the game starts
ndJointBilateralConstraint* UNewtonJointHinge::CreateJoint()
{
	Super::CreateJoint();

	check(!m_joint);
	ndBodyKinematic* body0;
	ndBodyKinematic* body1;
	GetBodyPairs(&body0, &body1);

	if (body0 && body1)
	{
		const ndMatrix matrix(GetPivotMatrix());
		ndJointHinge* const joint = new ndJointHinge(matrix, body0, body1);

		joint->SetLimitState(EnableLimits);
		joint->SetLimits(ndFloat32 (MinAngle * ndDegreeToRad), ndFloat32(MaxAngle * ndDegreeToRad));
		joint->SetAsSpringDamper(SpringDamperRegularizer, SpringConst, DampingConst);
		
		return joint;
	}
	return nullptr;
}

float UNewtonJointHinge::GetTargetAngle() const
{
	check(m_joint);
	ndJointHinge* const joint = (ndJointHinge*)m_joint;
	return joint->GetTargetAngle() * ndRadToDegree;
}

void UNewtonJointHinge::SetTargetAngle(float degrees)
{
	check(m_joint);
	ndJointHinge* const joint = (ndJointHinge*)m_joint;

	ndFloat32 radians = degrees * ndDegreeToRad;
	ndFloat32 currentValue = joint->GetTargetAngle();
	if (ndAbs(radians - currentValue) > ndFloat32(2.5e-3f))
	{
		AwakeBodies();
		joint->SetTargetAngle(radians);
	}
}
