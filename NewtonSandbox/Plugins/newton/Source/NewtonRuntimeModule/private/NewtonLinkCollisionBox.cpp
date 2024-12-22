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

#include "NewtonLinkCollisionBox.h"

#include "NewtonCommons.h"
#include "NewtonCollisionBox.h"
#include "NewtonLinkRigidBody.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkCollisionBox::UNewtonLinkCollisionBox()
	:Super()
{
	SetName(TEXT("NewBox"));

	SizeX = 100.0f;
	SizeY = 100.0f;
	SizeZ = 100.0f;
}

ndShapeInstance UNewtonLinkCollisionBox::CreateInstance() const
{
	float x = SizeX * UNREAL_INV_UNIT_SYSTEM;
	float y = SizeY * UNREAL_INV_UNIT_SYSTEM;
	float z = SizeZ * UNREAL_INV_UNIT_SYSTEM;
	return ndShapeInstance (new ndShapeBox(x, y, z));
}

TObjectPtr<USceneComponent> UNewtonLinkCollisionBox::CreateBlueprintProxy() const
{
	TObjectPtr<UNewtonCollisionBox> component(NewObject<UNewtonCollisionBox>(UNewtonCollisionBox::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkCollisionBox::InitBlueprintProxy(TObjectPtr<USceneComponent> component) const
{
	UNewtonCollisionBox* const shape = Cast<UNewtonCollisionBox>(component.Get());

	shape->SizeX = SizeX;
	shape->SizeY = SizeY;
	shape->SizeZ = SizeZ;
}