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

#include "NewtonLinkCollisionCylinder.h"

#include "NewtonCommons.h"
#include "NewtonCollisionCylinder.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"


UNewtonLinkCollisionCylinder::UNewtonLinkCollisionCylinder()
	:Super()
{
	Name = TEXT("cylinder");

	Radio0 = 50.0f;
	Radio1 = 50.0f;
	Length = 100.0f;
}

ndShapeInstance UNewtonLinkCollisionCylinder::CreateInstance(TObjectPtr<USkeletalMesh>, int) const
{
	ndShapeInstance instance(new ndShapeCylinder(Radio0 * UNREAL_INV_UNIT_SYSTEM, Radio1 * UNREAL_INV_UNIT_SYSTEM, Length * UNREAL_INV_UNIT_SYSTEM));
	return instance;
}

TObjectPtr<USceneComponent> UNewtonLinkCollisionCylinder::CreateBlueprintProxy() const
{
	TObjectPtr<UNewtonCollisionCylinder> component(NewObject<UNewtonCollisionCylinder>(UNewtonCollisionCylinder::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkCollisionCylinder::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonCollisionCylinder* const shape = Cast<UNewtonCollisionCylinder>(component.Get());

	shape->Radio0 = Radio0;
	shape->Radio1 = Radio1;
	shape->Length = Length;
}