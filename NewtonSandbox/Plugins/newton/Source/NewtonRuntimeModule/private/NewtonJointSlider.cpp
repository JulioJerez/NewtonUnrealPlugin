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


#include "NewtonJointSlider.h"

#include "NewtonRigidBody.h"
#include "NewtonWorldActor.h"
#include "NewtonRuntimeModule.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonJointSlider::UNewtonJointSlider()
	:Super()
	,EnableLimits(false)
	,MinDistance(-100.0f)
	,MaxDistance(100.0f)
	,SpringConst(0.0f)
	,DampingConst(0.0f)
	,SpringDamperRegularizer(1.0e-3f)
{
}

void UNewtonJointSlider::DrawGizmo(float timestep) const
{
	ndFloat32 scale = DebugScale * UNREAL_UNIT_SYSTEM;
	const FTransform transform(GetComponentTransform());
	const ndMatrix matrix(ToNewtonMatrix(transform));
	const FColor pinColor(255.0f, 255.0f, 0.0f);
	const ndVector pinDir(matrix.m_front.Scale(scale * 0.9f));
	const FVector pinStart(transform.GetLocation());
	const FVector pinEnd(float(pinStart.X + pinDir.m_x), float(pinStart.Y + pinDir.m_y), float(pinStart.Z + pinDir.m_z));

	const UWorld* const world = GetWorld();
	DrawDebugLine(world, pinStart, pinEnd, pinColor, false, timestep);

	UNewtonRigidBody* const child = FindChild();
	if (EnableLimits && child)
	{
		const ndVector maxLimit(matrix.m_front.Scale(MaxDistance));
		const FVector maxConeStart(pinStart.X + maxLimit.m_x, pinStart.Y + maxLimit.m_y, pinStart.Z + maxLimit.m_z);
		const ndVector minLimit(matrix.m_front.Scale(MinDistance));
		const FVector minConeStart(pinStart.X + minLimit.m_x, pinStart.Y + minLimit.m_y, pinStart.Z + minLimit.m_z);

		const FVector coneDir(matrix.m_front.m_x, matrix.m_front.m_y, matrix.m_front.m_z);
		DrawDebugLine(world, minConeStart, maxConeStart, pinColor, false, timestep);
		DrawDebugCone(world, maxConeStart, coneDir, scale * 0.125f, 15.0f * ndDegreeToRad, 15.0f * ndDegreeToRad, 8, pinColor, false, timestep);
		DrawDebugCone(world, minConeStart, coneDir, -scale * 0.125f, 15.0f * ndDegreeToRad, 15.0f * ndDegreeToRad, 8, pinColor, false, timestep);
	}
}

// Called when the game starts
ndJointBilateralConstraint* UNewtonJointSlider::CreateJoint()
{
	Super::CreateJoint();

	check(!m_joint);
	check(!m_joint);
	ndBodyKinematic* body0;
	ndBodyKinematic* body1;
	//ndWorld* const world = newtonWorldActor->GetNewtonWorld();
	//GetBodyPairs(world, &body0, &body1);
	GetBodyPairs(&body0, &body1);

	if (body0 && body1)
	{
		const FTransform transform(GetRelativeTransform());
		const ndMatrix matrix(ToNewtonMatrix(transform) * body1->GetMatrix());

		ndJointSlider* const joint = new ndJointSlider(matrix, body0, body1);

		joint->SetLimitState(EnableLimits);
		joint->SetLimits(ndFloat32(MinDistance * UNREAL_INV_UNIT_SYSTEM), ndFloat32(MaxDistance * UNREAL_INV_UNIT_SYSTEM));
		joint->SetAsSpringDamper(SpringDamperRegularizer, SpringConst, DampingConst);

		//m_joint = joint;
		//world->AddJoint(m_joint);
		return joint;
	}
	return nullptr;
}
