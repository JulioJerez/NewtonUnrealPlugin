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

#include "NewtonLinkCollisionWheel.h"

#include "NewtonCommons.h"
#include "NewtonLinkRigidBody.h"
#include "NewtonCollisionWheel.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkCollisionWheel::UNewtonLinkCollisionWheel()
	:Super()
{
	Radio = 50.0f;
	Width = 50.0f;
	Name = TEXT("wheel");
}

ndShapeInstance UNewtonLinkCollisionWheel::CreateInstance() const
{
	ndShapeInstance instance(new ndShapeChamferCylinder(ndFloat32(0.75f), ndFloat32(0.5f)));

	const ndVector scale(ndFloat32(Width * UNREAL_INV_UNIT_SYSTEM), ndFloat32(Radio * UNREAL_INV_UNIT_SYSTEM), ndFloat32(Radio * UNREAL_INV_UNIT_SYSTEM), ndFloat32(0.0f));
	instance.SetScale(scale);
	return instance;
}

ndShapeInstance UNewtonLinkCollisionWheel::CreateInstance(TObjectPtr<USkeletalMesh>, int) const
{
	return CreateInstance();
}

ndShapeInstance UNewtonLinkCollisionWheel::CreateInstance(TObjectPtr<UStaticMesh>) const
{
	return CreateInstance();
}

TObjectPtr<USceneComponent> UNewtonLinkCollisionWheel::CreateBlueprintProxy() const
{
	TObjectPtr<USceneComponent> component(NewObject<UNewtonCollisionWheel>(UNewtonCollisionWheel::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkCollisionWheel::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonCollisionWheel* const shape = Cast<UNewtonCollisionWheel>(component.Get());

	shape->Radio = Radio;
	shape->Width = Width;
}