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

#include "NewtonCommons.h"

#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

ndHullPoints::ndHullPoints()
	:TArray<FVector3f>()
{
}

ndHullPoints::~ndHullPoints()
{
}

ndConvexHullSet::ndConvexHullSet()
	:TArray<ndHullPoints*>()
{
}

ndConvexHullSet::~ndConvexHullSet()
{
	for (int i = 0; i < Num(); ++i)
	{
		delete (*this)[i];
	}
}

FTransform ToUnRealTransform(const ndMatrix& matrix)
{
	const ndQuaternion rotation(matrix);
	const ndVector posit(matrix.m_posit.Scale(UNREAL_UNIT_SYSTEM));
	const FVector uPosit(posit.m_x, posit.m_y, posit.m_z);
	const FQuat uRot(rotation.m_x, rotation.m_y, rotation.m_z, rotation.m_w);

	FTransform transform;
	transform.SetRotation(uRot);
	transform.SetLocation(uPosit);
	return transform;
}

ndMatrix ToNewtonMatrix(const FTransform& transform)
{
	const FVector location(transform.GetLocation());
	const FQuat rotation(transform.Rotator().Quaternion());

	const ndQuaternion quat(ndFloat32(rotation.X), ndFloat32(rotation.Y), ndFloat32(rotation.Z), ndFloat32(rotation.W));
	const ndVector posit(UNREAL_INV_UNIT_SYSTEM * ndFloat32(location.X), UNREAL_INV_UNIT_SYSTEM * ndFloat32(location.Y), UNREAL_INV_UNIT_SYSTEM * ndFloat32(location.Z), ndFloat32(1.0f));
	const ndMatrix matrix(ndCalculateMatrix(quat, posit));
	return matrix;
}
