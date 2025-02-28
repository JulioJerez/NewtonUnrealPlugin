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

#include "NewtonLinkJointSlider.h"

#include "NewtonCommons.h"
#include "NewtonJointSlider.h"

#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkJointSlider::UNewtonLinkJointSlider()
	:Super()
{
	Name = TEXT("slider");

	MaxDistance = 100.0f;
	MinDistance = -100.0f;
	EnableLimits = false;

	SpringConst = 0.0f;
	DampingConst = 0.0f;
	SpringDamperRegularizer = 1.0e-3f;
}

TObjectPtr<USceneComponent> UNewtonLinkJointSlider::CreateBlueprintProxy() const
{
	TObjectPtr<USceneComponent> component(NewObject<UNewtonJointSlider>(UNewtonJointSlider::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkJointSlider::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonJointSlider* const joint = Cast<UNewtonJointSlider>(component.Get());

	SetCommonProperties(joint);

	joint->MinDistance = MinDistance;
	joint->MaxDistance = MaxDistance;
	joint->EnableLimits = EnableLimits;

	joint->SpringConst = SpringConst;
	joint->DampingConst = DampingConst;
	joint->EnableLimits = EnableLimits;
	joint->SpringDamperRegularizer = SpringDamperRegularizer;
}