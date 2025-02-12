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

#include "NewtonJointVehicleTire.h"

#include "NewtonRigidBody.h"
#include "NewtonWorldActor.h"
#include "NewtonRuntimeModule.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonJointVehicleTire::UNewtonJointVehicleTire()
	:Super()
{
}


ndJointBilateralConstraint* UNewtonJointVehicleTire::CreateJoint()
{
	Super::Super::CreateJoint();

	check(!m_joint);
	check(!m_joint);
	ndBodyKinematic* body0;
	ndBodyKinematic* body1;
	GetBodyPairs(&body0, &body1);

	if (body0 && body1)
	{
		ndMultiBodyVehicleTireJointInfo desc;
		desc.m_springK = SpringK;
		desc.m_damperC = DamperC;
		desc.m_upperStop = UpperStop;
		desc.m_lowerStop = LowerStop;
		desc.m_regularizer = Regularizer;
		desc.m_brakeTorque = BrakeTorque;
		desc.m_steeringAngle = SteeringAngle;
		desc.m_handBrakeTorque = HandBrakeTorque;

		const ndMatrix matrix(GetPivotMatrix());
		ndMultiBodyVehicleTireJoint* const joint = new ndMultiBodyVehicleTireJoint(matrix, body0, body1, desc, nullptr);

		return joint;
	}
	return nullptr;

}
