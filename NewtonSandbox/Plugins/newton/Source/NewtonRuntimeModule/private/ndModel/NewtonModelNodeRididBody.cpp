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

#include "ndModel/NewtonModelNodeRigidBody.h"

#include "ndModel/NewtonModelNodeCollision.h"

UNewtonModelNodeRigidBody::UNewtonModelNodeRigidBody()
	:Super()
	,Transform()
	,ShowCenterOfMass(false)
	,AutoSleepMode(true)
	,DebugScale(1.0f)
	,Mass(1.0f)
	,LinearDamp(0.0f)
	,AngularDamp(0.0f)
	,InitialVeloc(0.0f, 0.0f, 0.0f)
	,InitialOmega(0.0f, 0.0f, 0.0f)
	,CenterOfMass(0.0f, 0.0f, 0.0f)
	,Gravity(0.0f, 0.0f, -980.0f)
{
	SetName (TEXT("NewBody"));
	BoneIndex = -1;

	float inertia = (2.0f / 5.0f) * 0.5f * 0.5f;
	Inertia.PrincipalInertia = FVector(inertia, inertia, inertia);
}


FVector UNewtonModelNodeRigidBody::CalculateLocalCenterOfMass(const TArray<const UNewtonModelNodeCollision*>& childen) const
{
	return FVector(0.0f, -50.0f, 25.0f);
}