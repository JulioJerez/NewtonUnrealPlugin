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

#include "NewtonLinkJointLoop6dofEffector.h"

#include "NewtonCommons.h"
#include "NewtonJointLoop6dofEffector.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkJointLoop6dofEffector::UNewtonLinkJointLoop6dofEffector()
	:Super()
{
	Name = TEXT("loop6dofEffector");
	AngularDamper = 500.0f;
	AngularSpring = 10000.0f;
	AngularMaxTorque = 10000.0f;
	AngularRegularizer = 1.0e-5f;

	LinearDamper = 500.0f;
	LinearSpring = 10000.0f;
	LinearMaxForce = 10000.0f;
	LinearRegularizer = 1.0e-5f;
}

TObjectPtr<USceneComponent> UNewtonLinkJointLoop6dofEffector::CreateBlueprintProxy() const
{
	TObjectPtr<USceneComponent> component(NewObject<UNewtonJointLoop6dofEffector>(UNewtonJointLoop6dofEffector::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkJointLoop6dofEffector::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonJointLoop6dofEffector* const joint = Cast<UNewtonJointLoop6dofEffector>(component.Get());

	SetCommonProperties(joint);

	joint->LinearSpring = LinearSpring;
	joint->LinearDamper = LinearDamper;
	joint->LinearMaxForce = LinearMaxForce;
	joint->LinearRegularizer = LinearRegularizer;
	joint->AngularSpring = AngularSpring;
	joint->AngularDamper = AngularDamper;
	joint->AngularMaxTorque = AngularMaxTorque;
	joint->AngularRegularizer = AngularRegularizer;
}