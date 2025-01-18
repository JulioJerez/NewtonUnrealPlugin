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


#include "NewtonJointKinematic.h"

#include "NewtonRigidBody.h"
#include "NewtonWorldActor.h"
#include "NewtonRuntimeModule.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonJointKinematic::UNewtonJointKinematic()
	:Super()
{
}

void UNewtonJointKinematic::DrawGizmo(float timestep) const
{
	//ndFloat32 scale = DebugScale * UNREAL_UNIT_SYSTEM;
	//const FTransform transform(GetComponentTransform());
	//const ndMatrix matrix(UNewtonRigidBody::ToNewtonMatrix(transform));
	//const FColor pinColor(255.0f, 255.0f, 0.0f);
	//const ndVector pinDir(matrix.m_front.Scale(scale * 0.9f));
	//const FVector coneDir(matrix.m_front.m_x, matrix.m_front.m_y, matrix.m_front.m_z);
	//const FVector pinStart(transform.GetLocation());
	//const FVector pinEnd(float(pinStart.X + pinDir.m_x), float(pinStart.Y + pinDir.m_y), float(pinStart.Z + pinDir.m_z));
	//
	//const UWorld* const world = GetWorld();
	//DrawDebugLine(world, pinStart, pinEnd, pinColor, false, timestep);
	//DrawDebugCone(world, pinEnd, coneDir, -scale * 0.125f, 15.0f * ndDegreeToRad, 15.0f * ndDegreeToRad, 8, pinColor, false, timestep);
	//
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
	//		const ndMatrix parentMatrix(UNewtonRigidBody::ToNewtonMatrix(transform));
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
	//		DrawDebugMesh(world, verts, indices, ND_DEBUG_MESH_COLOR, false, timestep);
	//	}
	//}
}

// Called when the game starts
ndJointBilateralConstraint* UNewtonJointKinematic::CreateJoint()
{
	Super::CreateJoint();

	check(0);
	return nullptr;
	//check(!m_joint);
	//ndBodyKinematic* body0;
	//ndBodyKinematic* body1;
	//ndWorld* const world = newtonWorldActor->GetNewtonWorld();
	//GetBodyPairs(world, &body0, &body1);
	//
	//if (body0 && body1)
	//{
	//  const ndMatrix matrix(GetPivotMatrix());
	//	ndJointHinge* const joint = new ndJointHinge(matrix, body0, body1);
	//
	//	joint->SetLimitState(EnableLimits);
	//	joint->SetLimits(ndFloat32(MinAngle * ndDegreeToRad), ndFloat32(MaxAngle * ndDegreeToRad));
	//	joint->SetAsSpringDamper(SpringDamperRegularizer, SpringConst, DampingConst);
	//
	//	m_joint = joint;
	//	world->AddJoint(m_joint);
	//}
}

FTransform UNewtonJointKinematic::GetTargetMatrix() const
{
	if (m_joint)
	{
		ndJointKinematicController* const joint = (ndJointKinematicController*)m_joint;

		const ndMatrix matrix(joint->GetTargetMatrix());
		return ToUnrealTransform(matrix);
	}
	return FTransform();
}

void UNewtonJointKinematic::SetTargetPosit(const FVector& glocalSpacePosit)
{
	if (m_joint)
	{
		ndJointKinematicController* const joint = (ndJointKinematicController*)m_joint;

		const FVector p(glocalSpacePosit * UNREAL_INV_UNIT_SYSTEM);
		const ndVector posit(ndFloat32(p.X), ndFloat32(p.Y), ndFloat32(p.Z), ndFloat32(1.0f));
		UE_LOG(LogTemp, Display, TEXT("w(%f %f %f)"), posit.m_x, posit.m_y, posit.m_z);

		joint->SetTargetPosit(posit);
	}
}

void UNewtonJointKinematic::SetTargetRotation(const FRotator& glocalSpaceRotation)
{
	if (m_joint)
	{
		ndJointKinematicController* const joint = (ndJointKinematicController*)m_joint;

		const ndMatrix rotation(ToNewtonMatrix(FTransform(glocalSpaceRotation)));
		joint->SetTargetRotation(rotation);
	}
}

void UNewtonJointKinematic::SetTargetMatrix(const FTransform& glocalSpaceTransform)
{
	if (m_joint)
	{
		ndJointKinematicController* const joint = (ndJointKinematicController*)m_joint;

		const ndMatrix matrix(ToNewtonMatrix(glocalSpaceTransform));
		joint->SetTargetMatrix(matrix);
	}
}

void UNewtonJointKinematic::CreateAttachament(UNewtonRigidBody* const childBody, const FVector& location, float angularFriction, float linearFriction)
{
	check(!m_joint);
	if (!m_joint)
	{
		FTransform transform(location);
		ndMatrix matrix(ToNewtonMatrix(transform));
		ndWorld* const world = childBody->GetBody()->GetScene()->GetWorld();
		ndJointKinematicController* const pickJoint = new ndJointKinematicController(childBody->GetBody(), world->GetSentinelBody(), matrix);

		m_joint = pickJoint;

		const ndVector massMatrix(childBody->GetBody()->GetMassMatrix());
		const FVector gravity(childBody->Gravity * UNREAL_INV_UNIT_SYSTEM);

		ndFloat32 inertia = ndMax(ndMax(massMatrix.m_x, massMatrix.m_y), massMatrix.m_z);
		ndFloat32 weight = massMatrix.m_w * ndMax(ndMax(ndAbs(gravity.X), ndAbs(gravity.Y)), ndAbs(gravity.Z));

		pickJoint->SetMaxLinearFriction(linearFriction * weight);
		pickJoint->SetMaxAngularFriction(angularFriction * inertia);
		pickJoint->SetControlMode(ndJointKinematicController::m_linearPlusAngularFriction);

		world->AddJoint(pickJoint);
	}
}

void UNewtonJointKinematic::DestroyAttachament()
{
	if (m_joint)
	{
		const ndBodyDynamic* const body = m_joint->GetBody0()->GetAsBodyDynamic();
		check(body);
		ndWorld* const world = body->GetScene()->GetWorld();

		world->RemoveJoint(m_joint);
		m_joint = nullptr;
	}
}