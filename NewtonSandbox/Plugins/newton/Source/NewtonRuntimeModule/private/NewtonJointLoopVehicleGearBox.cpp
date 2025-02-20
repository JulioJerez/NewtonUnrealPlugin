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

#include "NewtonJointLoopVehicleGearBox.h"
#include "Animation\Skeleton.h"

#include "NewtonModel.h"
#include "NewtonAsset.h"
#include "NewtonRigidBody.h"
#include "NewtonWorldActor.h"
#include "NewtonRuntimeModule.h"
#include "NewtonJointVehicleTire.h"
#include "NewtonJointVehicleMotor.h"
#include "NewtonJointVehicleDifferential.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonJointLoopVehicleGearBox::UNewtonJointLoopVehicleGearBox()
	:Super()
{
}

void UNewtonJointLoopVehicleGearBox::DrawGizmo(float timestep) const
{
	const UWorld* const world = GetWorld();
	ndFloat32 scale = DebugScale * UNREAL_UNIT_SYSTEM;
	
	auto DrawFrame = [world, scale, timestep](const FTransform& frame)
	{
		float thickness = 0.5f;
		const FVector positionParent(frame.GetLocation());
		const FVector xAxisParent(frame.GetUnitAxis(EAxis::X));
		const FVector yAxisParent(frame.GetUnitAxis(EAxis::Y));
		const FVector zAxisParent(frame.GetUnitAxis(EAxis::Z));
		DrawDebugLine(world, positionParent, positionParent + scale * xAxisParent, FColor::Red, false, timestep, thickness);
		DrawDebugLine(world, positionParent, positionParent + scale * zAxisParent, FColor::Blue, false, timestep, thickness);
		DrawDebugLine(world, positionParent, positionParent + scale * yAxisParent, FColor::Green, false, timestep, thickness);
	};
	
	// draw references frames
	const FTransform parentTransform(GetComponentTransform());
	DrawFrame(parentTransform);
	DrawFrame(TargetFrame * parentTransform);
}

// Called when the game starts
ndJointBilateralConstraint* UNewtonJointLoopVehicleGearBox::CreateJoint()
{
	Super::CreateJoint();
	
	check(!m_joint);

	AActor* const owner = GetOwner();
	check(owner);
	UNewtonModel* const model = owner->FindComponentByClass<UNewtonModel>();

	const UNewtonRigidBody* differentialBody = nullptr;
	const UNewtonRigidBody* const motorBody = Cast<UNewtonRigidBody>(GetAttachParent());
	check(motorBody && Cast<UNewtonJointVehicleMotor>(motorBody->GetAttachParent()));

	const FString bodyName(ReferencedBodyName.ToString());
	for (TSet<UActorComponent*>::TConstIterator it(owner->GetComponents().CreateConstIterator()); it; ++it)
	{
		const UNewtonRigidBody* const body = Cast<UNewtonRigidBody>(*it);
		if (body)
		{
			UNewtonJointVehicleDifferential* const differential = Cast<UNewtonJointVehicleDifferential>(body->GetAttachParent());
			if (differential)
			{
				const FString name(differential->GetFName().ToString());
				int32 index = name.Find(bodyName);
				if (index == 0)
				{
					differentialBody = body;
					break;
				}
			}
		}
	}
	check(differentialBody);
	
	if (motorBody && differentialBody)
	{
		const UNewtonModel* const vehicle = owner->FindComponentByClass<UNewtonModel>();
		check(vehicle && vehicle->m_model);

		const FVector motorPin(TargetFrame.GetUnitAxis(EAxis::X));
		bool reverseSpin = motorPin.X < 0.0f ? true : false;

		ndMultiBodyVehicleGearBox* const joint = new ndMultiBodyVehicleGearBox(motorBody->GetBody(), differentialBody->GetBody(), vehicle->m_model->GetAsMultiBodyVehicle(), reverseSpin);
		return joint;
	}
	return nullptr;
}
