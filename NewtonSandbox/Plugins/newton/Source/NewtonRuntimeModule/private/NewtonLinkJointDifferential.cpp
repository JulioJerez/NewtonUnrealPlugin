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

#include "NewtonLinkJointDifferential.h"
#include "NewtonCommons.h"
#include "NewtonJointVehicleDifferential.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkJointDifferential::UNewtonLinkJointDifferential()
	:Super()
{
	Name = TEXT("differential");
	BodyMass = 25.0f;
	BodyRadio = 25.0f;
	LimitedSlipRpmLock = 30.0f;
}

TObjectPtr<USceneComponent> UNewtonLinkJointDifferential::CreateBlueprintProxy() const
{
	TObjectPtr<USceneComponent> component(NewObject<UNewtonJointVehicleDifferential>(UNewtonJointVehicleDifferential::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkJointDifferential::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonJointVehicleDifferential* const joint = Cast<UNewtonJointVehicleDifferential>(component.Get());

	SetCommonProperties(joint);

	joint->BodyMass = BodyMass;
	joint->BodyRadio = BodyRadio;
	joint->LimitedSlipRpmLock = LimitedSlipRpmLock;
}