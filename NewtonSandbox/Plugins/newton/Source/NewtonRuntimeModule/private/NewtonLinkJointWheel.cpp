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

#include "NewtonLinkJointWheel.h"

#include "NewtonCommons.h"
#include "NewtonJointWheel.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkJointWheel::UNewtonLinkJointWheel()
	:Super()
{
	Name = TEXT("wheel");

	Radio = 50.0f;
	SpringK = 100000.0f;
	DamperC = 100.0f;
	UpperStop = 25.0f;
	LowerStop = -25.0f;
	Regularizer = 1.0e-3f;
	BrakeTorque = 0.0f;
	SteeringAngle = 0.0f;
	HandBrakeTorque = 0.0f;
}

TObjectPtr<USceneComponent> UNewtonLinkJointWheel::CreateBlueprintProxy() const
{
	TObjectPtr<USceneComponent> component(NewObject<UNewtonJointWheel>(UNewtonJointWheel::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkJointWheel::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonJointWheel* const joint = Cast<UNewtonJointWheel>(component.Get());

	SetCommonProperties(joint);

	joint->SpringK = SpringK;
	joint->DamperC = DamperC;
	joint->UpperStop = UpperStop;
	joint->LowerStop = LowerStop;
	joint->Regularizer = Regularizer;
	joint->BrakeTorque = BrakeTorque;
	joint->SteeringAngle = SteeringAngle;
	joint->HandBrakeTorque = HandBrakeTorque;
}