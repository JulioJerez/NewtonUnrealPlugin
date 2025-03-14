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

#include "NewtonLinkJointRoller.h"

#include "NewtonCommons.h"
#include "NewtonJointRoller.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkJointRoller::UNewtonLinkJointRoller()
	:Super()
{
	Name = TEXT("roller");

	SpringK = 10000.0f;
	DamperC = 100.0f;
	MinLimit = -1.0f;
	MaxLimit = 1.0f;
	Regularizer = 1.0e-2f;
	EnableLimits = false;
}

TObjectPtr<USceneComponent> UNewtonLinkJointRoller::CreateBlueprintProxy() const
{
	TObjectPtr<USceneComponent> component(NewObject<UNewtonJointRoller>(UNewtonJointRoller::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkJointRoller::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonJointRoller* const joint = Cast<UNewtonJointRoller>(component.Get());

	SetCommonProperties(joint);

	joint->SpringK = SpringK;
	joint->DamperC = DamperC;
	joint->MaxLimit= MaxLimit;
	joint->MinLimit = MinLimit;
	joint->Regularizer = Regularizer;
	joint->EnableLimits = EnableLimits;
}