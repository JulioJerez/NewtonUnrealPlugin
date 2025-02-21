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

#include "NewtonJointLoopVehicleDifferentialAxle.h"
#include "Animation\Skeleton.h"

#include "NewtonModel.h"
#include "NewtonAsset.h"
#include "NewtonRigidBody.h"
#include "NewtonWorldActor.h"
#include "NewtonRuntimeModule.h"
#include "NewtonJointVehicleTire.h"
#include "NewtonJointVehicleDifferential.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonJointLoopVehicleDifferentialAxle::UNewtonJointLoopVehicleDifferentialAxle()
	:Super()
{
	DifferentialFrame = FTransform();
}

void UNewtonJointLoopVehicleDifferentialAxle::DrawGizmo(float timestep) const
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
	
	if (m_joint)
	{
		ndMatrix matrix0;
		ndMatrix matrix1;
		m_joint->CalculateGlobalMatrix(matrix0, matrix1);

		const FTransform transform0(ToUnrealTransform(matrix0));
		const FTransform transform1(ToUnrealTransform(matrix1));
		DrawFrame(transform0);
		DrawFrame(transform1);
	}
	else
	{
		// draw references frames
		const FTransform parentTransform(GetComponentTransform());
		DrawFrame(parentTransform);
		DrawFrame(TargetFrame * parentTransform);
	}
}

// Called when the game starts
ndJointBilateralConstraint* UNewtonJointLoopVehicleDifferentialAxle::CreateJoint()
{
	Super::CreateJoint();

	check(!m_joint);

	AActor* const owner = GetOwner();
	check(owner);

	UNewtonRigidBody* const differentialBodyComponet = Cast<UNewtonRigidBody>(GetAttachParent());
	check(differentialBodyComponet);

	if (differentialBodyComponet)
	{
		const UNewtonRigidBody* otherDiffrentialComponent = nullptr;
		const FString bodyName(ReferencedBodyName.ToString());
		for (TSet<UActorComponent*>::TConstIterator it(owner->GetComponents().CreateConstIterator()); it; ++it)
		{
			const UNewtonRigidBody* const body = Cast<UNewtonRigidBody>(*it);
			if (body)
			{
				UNewtonJointVehicleDifferential* const otherDifferetial = Cast<UNewtonJointVehicleDifferential>(body->GetAttachParent());
				if (otherDifferetial)
				{
					const FString name(otherDifferetial->GetFName().ToString());
					int32 index = name.Find(bodyName);
					if (index == 0)
					{
						otherDiffrentialComponent = body;
						break;
					}
				}
			}
		}
		check(otherDiffrentialComponent);
		if (otherDiffrentialComponent)
		{
			ndBodyKinematic* const differentialBody = differentialBodyComponet->GetBody();
			const FTransform transform(DifferentialFrame * GetRelativeTransform());
			const ndMatrix parentMarix(ToNewtonMatrix(transform) * differentialBody->GetMatrix());
			const ndVector upPin(parentMarix.m_up);
			const ndVector frontPin(parentMarix.m_front);

			ndBodyKinematic* const childDiffrentialBody = otherDiffrentialComponent->GetBody();
			const UNewtonJointVehicleDifferential* const childDifferentialJoint = Cast<UNewtonJointVehicleDifferential>(otherDiffrentialComponent->GetAttachParent());
			check(childDifferentialJoint);
			const ndMatrix childMarix(childDifferentialJoint->GetJoint()->CalculateGlobalMatrix0());
			const ndVector drivePin(childMarix.m_front);

			ndMultiBodyVehicleDifferentialAxle* const joint = new ndMultiBodyVehicleDifferentialAxle(frontPin, upPin, differentialBody, drivePin, childDiffrentialBody);
			return joint;
		}
	}
	return nullptr;
}




