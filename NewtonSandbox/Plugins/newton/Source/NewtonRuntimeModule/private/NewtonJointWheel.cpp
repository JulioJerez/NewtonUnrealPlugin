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
	SpringK = 2000.0f;
	DamperC = 50.0f;
	UpperStop = 25.0f;
	LowerStop = -25.0f;
	Regularizer = 0.1f;
	BrakeTorque = 0.0f;
	SteeringAngle = 0.0f;
	HandBrakeTorque = 0.0f;
}

void UNewtonJointWheel::DrawGizmo(float timestep) const
{
	//ndFloat32 scale = DebugScale * UNREAL_UNIT_SYSTEM;
	//const FTransform transform(GetComponentTransform());
	//const ndMatrix matrix(ToNewtonMatrix(transform));
	//const FColor pinColor(255.0f, 255.0f, 0.0f);
	//const ndVector pinDir(matrix.m_front.Scale(scale * 0.9f));
	//const FVector coneDir(matrix.m_front.m_x, matrix.m_front.m_y, matrix.m_front.m_z);
	//const FVector pinStart(transform.GetLocation());
	//const FVector pinEnd(float(pinStart.X + pinDir.m_x), float(pinStart.Y + pinDir.m_y), float(pinStart.Z + pinDir.m_z));
	//
	//const UWorld* const world = GetWorld();
	//DrawDebugLine(world, pinStart, pinEnd, pinColor, false, timestep);
	//DrawDebugCone(world, pinEnd, coneDir, -scale * 0.125f, 15.0f * ndDegreeToRad, 15.0f * ndDegreeToRad, 8, pinColor, false, timestep);

	UNewtonRigidBody* const child = FindChild();
	if (child)
	{
		const UWorld* const world = GetWorld();
		const FTransform transform(GetComponentTransform());

		const FVector axisLoc(transform.GetLocation());
		const FRotator axisRot(transform.GetRotation());
		DrawDebugCoordinateSystem(world, axisLoc, axisRot, DebugScale * UNREAL_UNIT_SYSTEM, false, timestep);
	}
}

// Called when the game starts
ndJointBilateralConstraint* UNewtonJointWheel::CreateJoint()
{
	Super::CreateJoint();

	check(!m_joint);
	check(!m_joint);
	ndBodyKinematic* body0;
	ndBodyKinematic* body1;
	GetBodyPairs(&body0, &body1);

	if (body0 && body1)
	{
		ndWheelDescriptor desc;
		desc.m_springK = SpringK;
		desc.m_damperC = DamperC;
		desc.m_upperStop = UpperStop * UNREAL_INV_UNIT_SYSTEM;
		desc.m_lowerStop = LowerStop * UNREAL_INV_UNIT_SYSTEM;
		desc.m_regularizer = Regularizer;
		desc.m_brakeTorque = BrakeTorque;
		desc.m_steeringAngle = SteeringAngle;
		desc.m_handBrakeTorque = HandBrakeTorque;

		//const FTransform transform(GetRelativeTransform());
		//const ndMatrix matrix(ToNewtonMatrix(transform) * body1->GetMatrix());

		const ndMatrix matrix(GetPivotMatrix());
		ndJointWheel* const joint = new ndJointWheel(matrix, body0, body1, desc);
		
		//joint->SetLimitState(EnableLimits);
		//joint->SetLimits(ndFloat32 (MinAngle * ndDegreeToRad), ndFloat32(MaxAngle * ndDegreeToRad));
		//joint->SetAsSpringDamper(SpringDamperRegularizer, SpringConst, DampingConst);

		//m_joint = joint;
		//world->AddJoint(m_joint);
		return joint;
	}
	return nullptr;
}

float UNewtonJointWheel::GetSteering() const
{
	if (m_joint)
	{
		ndJointWheel* const joint = (ndJointWheel*)m_joint;
		return joint->GetSteering();
	}
	return 0.0f;
}

float UNewtonJointWheel::GetBreak() const
{
	if (m_joint)
	{
		ndJointWheel* const joint = (ndJointWheel*)m_joint;
		return joint->GetBreak();
	}
	return 0.0f;
}

float UNewtonJointWheel::GetHandBreak() const
{
	if (m_joint)
	{
		ndJointWheel* const joint = (ndJointWheel*)m_joint;
		return joint->GetHandBreak();
	}
	return 0.0f;
}

void UNewtonJointWheel::SetSteering(float parametricAngle)
{
	if (m_joint)
	{
		ndJointWheel* const joint = (ndJointWheel*)m_joint;
		joint->SetSteering(parametricAngle);
	}
}

void UNewtonJointWheel::SetBreak(float parametricBreak)
{
	if (m_joint)
	{
		ndJointWheel* const joint = (ndJointWheel*)m_joint;
		joint->SetBreak(parametricBreak);
	}
}

void UNewtonJointWheel::SetHandBreak(float parametricBreak)
{
	if (m_joint)
	{
		ndJointWheel* const joint = (ndJointWheel*)m_joint;
		joint->SetHandBreak(parametricBreak);
	}
}

float UNewtonJointWheel::GetSuspensionPosit() const
{
	if (m_joint)
	{
		ndJointWheel* const joint = (ndJointWheel*)m_joint;
		return joint->GetPosit() * UNREAL_UNIT_SYSTEM;
	}
	return 0.0f;
}

float UNewtonJointWheel::GetSuspensionSpeed() const
{
	if (m_joint)
	{
		ndJointWheel* const joint = (ndJointWheel*)m_joint;
		return joint->SetSpeed() * UNREAL_UNIT_SYSTEM;
	}
	return 0.0f;
}
