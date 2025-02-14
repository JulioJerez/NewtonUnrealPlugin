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

#include "NewtonLinkLoopEffector6dof.h"

#include "NewtonCommons.h"
#include "NewtonJointIk6DofEffector.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkLoopEffector6dof::UNewtonLinkLoopEffector6dof()
	:Super()
{
	Name = TEXT("effector6dof");
	TargetFrame = FTransform();
	AngularDamper = 500.0f;
	AngularSpring = 10000.0f;
	AngularMaxTorque = 10000.0f;
	AngularRegularizer = 1.0e-5f;

	LinearDamper = 500.0f;
	LinearSpring = 10000.0f;
	LinearMaxForce = 10000.0f;
	LinearRegularizer = 1.0e-5f;
}

TObjectPtr<USceneComponent> UNewtonLinkLoopEffector6dof::CreateBlueprintProxy() const
{
	TObjectPtr<USceneComponent> component(NewObject<UNewtonJointIk6DofEffector>(UNewtonJointIk6DofEffector::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkLoopEffector6dof::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonJointIk6DofEffector* const joint = Cast<UNewtonJointIk6DofEffector>(component.Get());

	SetCommonProperties(joint);

	joint->TargetFrame = TargetFrame;
	check(0);
	//joint->ReferencedBodyName = BoneName;

	joint->LinearSpring = LinearSpring;
	joint->LinearDamper = LinearDamper;
	joint->LinearMaxForce = LinearMaxForce;
	joint->LinearRegularizer = LinearRegularizer;
	joint->AngularSpring = AngularSpring;
	joint->AngularDamper = AngularDamper;
	joint->AngularMaxTorque = AngularMaxTorque;
	joint->AngularRegularizer = AngularRegularizer;
}