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

#include "NewtonJointWheel.h"

#include "NewtonRigidBody.h"
#include "NewtonWorldActor.h"
#include "NewtonRuntimeModule.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonJointWheel::UNewtonJointWheel()
	:Super()
{
	Radio = 50.0f;
	SpringK = 100000.0f;
	DamperC = 100.0f;
	UpperStop = 25.0f;
	LowerStop = -25.0f;
	Regularizer = 1.0e-3f;
	BrakeTorque = 0.0f;
	SteeringAngle = 0.0f;
	HandBrakeTorque = 0.0f;
}

void UNewtonJointWheel::DrawGizmo(float timestep) const
{
	ndFloat32 scale = DebugScale * UNREAL_UNIT_SYSTEM;
	const FTransform transform(GetComponentTransform());
	const ndMatrix matrix(ToNewtonMatrix(transform));
	const FColor pinColor(255.0f, 255.0f, 0.0f);
	const ndVector pinDir(matrix.m_front.Scale(scale * 0.9f));
	const FVector coneDir(matrix.m_front.m_x, matrix.m_front.m_y, matrix.m_front.m_z);
	const FVector pinStart(transform.GetLocation());
	const FVector pinEnd(float(pinStart.X + pinDir.m_x), float(pinStart.Y + pinDir.m_y), float(pinStart.Z + pinDir.m_z));

	const UWorld* const world = GetWorld();
	DrawDebugLine(world, pinStart, pinEnd, pinColor, false, timestep);
	DrawDebugCone(world, pinEnd, coneDir, -scale * 0.125f, 15.0f * ndDegreeToRad, 15.0f * ndDegreeToRad, 8, pinColor, false, timestep);

	//UNewtonRigidBody* const child = FindChild();
	//if (EnableLimits && child)
	//{
	//	TArray<int32> indices;
	//	TArray<FVector> verts;
	//	float deltaTwist = MaxAngle - MinAngle;
	//	if ((deltaTwist > 1.0e-3f) && (deltaTwist < 360.0f))
	//	{
	//		const ndVector point(ndFloat32(0.0f), scale, ndFloat32(0.0f), ndFloat32(0.0f));
	//		const ndInt32 subdiv = 12;
	//		ndFloat32 angle0 = MinAngle;
	//		ndFloat32 angleStep = ndMin(deltaTwist, 360.0f) / subdiv;
	//	
	//		const FVector parentOrigin(transform.GetLocation());
	//		const ndMatrix parentMatrix(ToNewtonMatrix(transform));
	//		verts.Push(parentOrigin);
	//		for (ndInt32 i = 0; i <= subdiv; ++i)
	//		{
	//			const ndVector p(parentMatrix.RotateVector(ndPitchMatrix(angle0 * ndDegreeToRad).RotateVector(point)));
	//			const FVector p1(float(p.m_x + parentOrigin.X), float(p.m_y + parentOrigin.Y), float(p.m_z + parentOrigin.Z));
	//			angle0 += angleStep;
	//			verts.Push(p1);
	//		}
	//		for (ndInt32 i = 0; i < subdiv; ++i)
	//		{
	//			indices.Push(0);
	//			indices.Push(i + 1);
	//			indices.Push(i + 2);
	//		}
	//		const FColor meshColor(255.0f, 0.0f, 0.0f, 32.0f);
	//		DrawDebugMesh(world, verts, indices, meshColor, false, timestep);
	//	}
	//}
}

// Called when the game starts
void UNewtonJointWheel::CreateJoint(ANewtonWorldActor* const newtonWorldActor)
{
	Super::CreateJoint(newtonWorldActor);

	check(!m_joint);
	ndBodyKinematic* body0;
	ndBodyKinematic* body1;
	ndWorld* const world = newtonWorldActor->GetNewtonWorld();
	GetBodyPairs(world, &body0, &body1);
	
	if (body0 && body1)
	{
		check(0);
		const FTransform transform(GetRelativeTransform());
		const ndMatrix matrix(ToNewtonMatrix(transform) * body1->GetMatrix());
		//ndJointWheel* const joint = new ndJointWheel(matrix, body0, body1);
		//joint->SetLimitState(EnableLimits);
		//joint->SetLimits(ndFloat32 (MinAngle * ndDegreeToRad), ndFloat32(MaxAngle * ndDegreeToRad));
		//joint->SetAsSpringDamper(SpringDamperRegularizer, SpringConst, DampingConst);
		//
		//m_joint = joint;
		//world->AddJoint(m_joint);
	}
}
