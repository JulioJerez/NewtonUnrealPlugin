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

#include "NewtonLinkJointHinge.h"

#include "NewtonCommons.h"
#include "NewtonJointHinge.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkJointHinge::UNewtonLinkJointHinge()
	:Super()
{
	Name = TEXT("hinge");

	MaxAngle = 45.0f;
	MinAngle = -45.0f;
	SpringConst = 0.0f;
	DampingConst = 0.0f;
	EnableLimits = false;
	SpringDamperRegularizer = 1.0e-3f;
}

TObjectPtr<USceneComponent> UNewtonLinkJointHinge::CreateBlueprintProxy() const
{
	TObjectPtr<UNewtonJointHinge> component(NewObject<UNewtonJointHinge>(UNewtonJointHinge::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkJointHinge::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonJointHinge* const joint = Cast<UNewtonJointHinge>(component.Get());

	SetCommonProperties(joint);

	joint->MinAngle = MinAngle;
	joint->MaxAngle = MaxAngle;
	joint->EnableLimits = EnableLimits;

	joint->SpringConst = SpringConst;
	joint->DampingConst = DampingConst;
	joint->EnableLimits = EnableLimits;
	joint->SpringDamperRegularizer = SpringDamperRegularizer;
}