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
	
	// draw references frames
	const FTransform parentTransform(GetComponentTransform());
	DrawFrame(parentTransform);
	DrawFrame(TargetFrame * parentTransform);
}

// Called when the game starts
ndJointBilateralConstraint* UNewtonJointLoopVehicleDifferentialAxle::CreateJoint()
{
	Super::CreateJoint();
	
	check(!m_joint);

	AActor* const owner = GetOwner();
	check(owner);
	UNewtonModel* const model = owner->FindComponentByClass<UNewtonModel>();
	check(model);
	USkeleton* const skeleton = model->NewtonAsset->SkeletalMeshAsset->GetSkeleton();
	const FReferenceSkeleton& refSkeleton = skeleton->GetReferenceSkeleton();
	const TArray<FMeshBoneInfo>& boneInfo = refSkeleton.GetRefBoneInfo();
	
	ndInt32 boneIndex = -1;
	for (ndInt32 i = boneInfo.Num() - 1; i >= 0; --i)
	{
		if (boneInfo[i].Name == ReferencedBodyName)
		{
			boneIndex = i;
			break;
		}
	}
	check(boneIndex != -1);
	
	UNewtonRigidBody* childComponent = nullptr;
	ndFixSizeArray<USceneComponent*, ND_STACK_DEPTH> stack;
	stack.PushBack(owner->GetRootComponent());
	while (stack.GetCount())
	{
		USceneComponent* const component = stack.Pop();
		check(component);
		UNewtonRigidBody* const bodyComponent = Cast<UNewtonRigidBody>(component);
		if (Cast<UNewtonRigidBody>(component))
		{
			if (bodyComponent->BoneIndex == boneIndex)
			{
				childComponent = bodyComponent;
				break;
			}
		}
	
		const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
		for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
		{
			stack.PushBack(childrenComp[i].Get());
		}
	}
	
	UNewtonRigidBody* const parentComponet = Cast<UNewtonRigidBody>(GetAttachParent());
	check(parentComponet);
	if (parentComponet && childComponent)
	{
		ndBodyKinematic* const childBody = childComponent->GetBody();
		ndBodyKinematic* const parentBody = parentComponet->GetBody();

		const UNewtonJointVehicleTire* tireJoint = nullptr;
		const UNewtonJointVehicleDifferential* differentialJoint = nullptr;
		for (TSet<UActorComponent*>::TConstIterator it(parentComponet->GetOwner()->GetComponents().CreateConstIterator()); it; ++it)
		{
			const UNewtonJointVehicleTire* const wheelJoint = Cast<UNewtonJointVehicleTire>(*it);
			if (wheelJoint && (wheelJoint->GetJoint()->GetBody0() == childBody))
			{
				tireJoint = wheelJoint;
			}

			const UNewtonJointVehicleDifferential* const diffJoint = Cast<UNewtonJointVehicleDifferential>(*it);
			if (diffJoint && (diffJoint->GetJoint()->GetBody0() == parentBody))
			{
				differentialJoint = diffJoint;
			}
		}
		check(tireJoint);
		check(differentialJoint);

		const FTransform transform(GetRelativeTransform());
		const ndMatrix childMarix(tireJoint->GetJoint()->CalculateGlobalMatrix0());
		const ndMatrix parentMarix(ToNewtonMatrix(transform) * parentBody->GetMatrix());
		
		const ndVector upPin(parentMarix.m_up);
		const ndVector pin(parentMarix.m_front);
		const ndVector drivePin(childMarix.m_front);
		ndMultiBodyVehicleDifferentialAxle* const joint = new ndMultiBodyVehicleDifferentialAxle(pin, upPin, parentBody, drivePin, childBody);
		
		return joint;
	}
	return nullptr;
}




