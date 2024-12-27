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



#include "NewtonLinkCollisionConvexhull.h"

#include "NewtonCommons.h"
#include "NewtonCollisionConvexHull.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"


UNewtonLinkCollisionConvexhull::UNewtonLinkCollisionConvexhull()
	:Super()
{
	SetName(TEXT("NewConvexhull"));

	Radio = 50.0f;
}

#include "NewtonCollisionSphere.h"
ndShapeInstance UNewtonLinkCollisionConvexhull::CreateInstance(TObjectPtr<USkeletalMesh> mesh, int boneIndex) const
{
	//ndShapeInstance instance(new ndShapeConvexhull(Radio * UNREAL_INV_UNIT_SYSTEM));
	ndShapeInstance instance(new ndShapeSphere(Radio * UNREAL_INV_UNIT_SYSTEM));
	return instance;
}

TObjectPtr<USceneComponent> UNewtonLinkCollisionConvexhull::CreateBlueprintProxy() const
{
	//TObjectPtr<UNewtonCollisionConvexhull> component(NewObject<UNewtonCollisionConvexhull>(UNewtonCollisionConvexhull::StaticClass(), Name, RF_Transient));
	TObjectPtr<UNewtonCollisionSphere> component(NewObject<UNewtonCollisionSphere>(UNewtonCollisionSphere::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkCollisionConvexhull::InitBlueprintProxy(TObjectPtr<USceneComponent> component) const
{
	//UNewtonCollisionConvexhull* const shape = Cast<UNewtonCollisionConvexhull>(component.Get());
	UNewtonCollisionSphere* const shape = Cast<UNewtonCollisionSphere>(component.Get());
	shape->Radio = Radio;
}