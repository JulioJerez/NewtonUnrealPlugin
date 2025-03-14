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

#include "NewtonLinkJointVehicleMotor.h"
#include "NewtonCommons.h"
#include "NewtonJointVehicleMotor.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkJointVehicleMotor::UNewtonLinkJointVehicleMotor()
	:Super()
{
	Name = TEXT("motor");
	BodyMass = 25.0f;
	BodyRadio = 25.0f;
	TestRPM = 1500.0f;
	TestToque = 200.0f;
	EnableTestTorque = false;
}

TObjectPtr<USceneComponent> UNewtonLinkJointVehicleMotor::CreateBlueprintProxy() const
{
	TObjectPtr<USceneComponent> component(NewObject<UNewtonJointVehicleMotor>(UNewtonJointVehicleMotor::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkJointVehicleMotor::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonJointVehicleMotor* const joint = Cast<UNewtonJointVehicleMotor>(component.Get());

	SetCommonProperties(joint);

	joint->BodyMass = BodyMass;
	joint->BodyRadio = BodyRadio;

	joint->TestRPM = TestRPM;
	joint->TestToque = TestToque;
	joint->EnableTestTorque = EnableTestTorque;
}