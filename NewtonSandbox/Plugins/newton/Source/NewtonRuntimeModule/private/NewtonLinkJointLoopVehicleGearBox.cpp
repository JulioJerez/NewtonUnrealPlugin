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

#include "NewtonLinkJointLoopVehicleGearBox.h"

#include "NewtonCommons.h"
#include "NewtonJointLoopVehicleGearBox.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkJointLoopVehicleGearBox::UNewtonLinkJointLoopVehicleGearBox()
	:Super()
{
	Name = TEXT("gearBox");
	ReverseGear = false;
}

TObjectPtr<USceneComponent> UNewtonLinkJointLoopVehicleGearBox::CreateBlueprintProxy() const
{
	TObjectPtr<USceneComponent> component(NewObject<UNewtonJointLoopVehicleGearBox>(UNewtonJointLoopVehicleGearBox::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkJointLoopVehicleGearBox::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonJointLoopVehicleGearBox* const joint = Cast<UNewtonJointLoopVehicleGearBox>(component.Get());
	SetCommonProperties(joint);

	FTransform gear;
	if (ReverseGear)
	{
		const FRotator rotator(0.0f, 180.0f, 0.0f);
		gear.SetRotation(FQuat(rotator));
		joint->TargetFrame = gear * joint->TargetFrame;
	}
}