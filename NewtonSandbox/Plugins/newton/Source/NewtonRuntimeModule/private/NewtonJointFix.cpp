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

#include "NewtonJointFix.h"

#include "NewtonRigidBody.h"
#include "NewtonWorldActor.h"
#include "NewtonRuntimeModule.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonJointFix::UNewtonJointFix()
	:Super()
{
}

void UNewtonJointFix::DrawGizmo(float timestep) const
{
	const FTransform transform(GetComponentTransform());
	DrawDebugCoordinateSystem(GetWorld(), transform.GetLocation(), transform.Rotator(), DebugScale, false, timestep);
}

// Called when the game starts
ndJointBilateralConstraint* UNewtonJointFix::CreateJoint()
{
	Super::CreateJoint();

	check(!m_joint);
	ndBodyKinematic* body0;
	ndBodyKinematic* body1;
	GetBodyPairs(&body0, &body1);

	if (body0 && body1)
	{
		const ndMatrix matrix(GetPivotMatrix());
		ndJointFix6dof* const joint = new ndJointFix6dof(matrix, body0, body1);
		return joint;
	}
	return nullptr;
}

