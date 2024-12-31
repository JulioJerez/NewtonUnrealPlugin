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
//#include "NewtonLoopEffector6dof.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkLoopEffector6dof::UNewtonLinkLoopEffector6dof()
	:Super()
{
	Name = TEXT("effector6dof");

	//MaxAngle = 45.0f;
	//MinAngle = -45.0f;
	//EnableLimits = false;
}


#include "NewtonJointHinge.h"
TObjectPtr<USceneComponent> UNewtonLinkLoopEffector6dof::CreateBlueprintProxy() const
{
	//TObjectPtr<UNewtonLoopEffector6dof> component(NewObject<UNewtonLoopEffector6dof>(UNewtonLoopEffector6dof::StaticClass(), Name, RF_Transient));
	TObjectPtr<UNewtonJointHinge> component(NewObject<UNewtonJointHinge>(UNewtonJointHinge::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkLoopEffector6dof::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	//UNewtonLoopEffector6dof* const joint = Cast<UNewtonLoopEffector6dof>(component.Get());
	UNewtonJointHinge* const joint = Cast<UNewtonJointHinge>(component.Get());

	SetCommonProperties(joint);

	//joint->MinAngle = MinAngle;
	//joint->MaxAngle = MaxAngle;
	//joint->EnableLimits = EnableLimits;
}