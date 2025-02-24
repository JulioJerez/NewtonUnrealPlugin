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

#include "NewtonJointLoop6dofEffector.h"
#include "Animation\Skeleton.h"

#include "NewtonModel.h"
#include "NewtonAsset.h"
#include "NewtonRigidBody.h"
#include "NewtonWorldActor.h"
#include "NewtonRuntimeModule.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonJointLoop6dofEffector::UNewtonJointLoop6dofEffector()
	:Super()
{
	//ReferencedBodyName = TEXT("None");
	AngularDamper = 500.0f;
	AngularSpring = 10000.0f;
	AngularMaxTorque = 10000.0f;
	AngularRegularizer = 1.0e-5f;

	m_target.X = 0.0f;
	m_target.Z = 0.0f;
	m_targetYaw = 0.0f;
	m_targetRoll = 0.0f;
	m_targetPitch = 0.0f;
	m_target.Azimuth = 0.0f;

	LinearDamper = 500.0f;
	LinearSpring = 10000.0f;
	LinearMaxForce = 10000.0f;
	LinearRegularizer = 1.0e-5f;
	m_referenceFrame = FMatrix::Identity;
	//rotationType(ndIk6DofEffector::m_shortestPath)
	//controlDofOptions(0xff)
}

void UNewtonJointLoop6dofEffector::DrawGizmo(float timestep) const
{
	const UWorld* const world = GetWorld();
	ndFloat32 scale = DebugScale * UNREAL_UNIT_SYSTEM;
	const FTransform parentTransform(GetComponentTransform());

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
	DrawFrame(parentTransform);
	DrawFrame(TargetFrame * parentTransform);

	if (m_joint)
	{
		// draw current effector frame
		ndIk6DofEffector* const joint = (ndIk6DofEffector*)m_joint;
		const FTransform effectorTransform(ToUnrealTransform(joint->GetEffectorMatrix()));
		DrawFrame(effectorTransform * parentTransform);

		// calculate the target frame for debug draw
		const ndMatrix refMatrix(ToNewtonMatrix(m_referenceFrame));
		const ndMatrix similarRotation(ndRollMatrix(ndAtan2(refMatrix.m_posit.m_y, refMatrix.m_posit.m_x)));

		const ndVector localPosit(similarRotation.UnrotateVector(refMatrix.m_posit));
		const ndVector planePosition(m_target.X * UNREAL_INV_UNIT_SYSTEM, ndFloat32(0.0f), m_target.Z * UNREAL_INV_UNIT_SYSTEM, ndFloat32(0.0f));
		const ndVector paramTarget(localPosit + planePosition);
		const ndVector referencePoint(similarRotation.RotateVector(paramTarget));

		const ndMatrix azimuthRotation(ndRollMatrix(m_target.Azimuth * ndDegreeToRad));
		const ndVector target(azimuthRotation.RotateVector(referencePoint));

		ndMatrix targetMatrix(ndRollMatrix(m_targetRoll * ndDegreeToRad) * ndYawMatrix(m_targetYaw * ndDegreeToRad) * ndPitchMatrix(m_targetPitch * ndDegreeToRad));
		targetMatrix.m_posit = target;
		targetMatrix.m_posit.m_w = 1.0f;

		const FTransform targetTransform(ToUnrealTransform(targetMatrix));
		DrawFrame(targetTransform * parentTransform);
	}
}

// Called when the game starts
ndJointBilateralConstraint* UNewtonJointLoop6dofEffector::CreateJoint()
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
		//ndWorld* const world = newtonWorldActor->GetNewtonWorld();
		ndBodyKinematic* const childBody = childComponent->GetBody();
		ndBodyKinematic* const parentBody = parentComponet->GetBody();
		const FTransform transform(GetRelativeTransform());
		const ndMatrix parentMarix(ToNewtonMatrix(transform) * parentBody->GetMatrix());
		const ndMatrix childMarix(ToNewtonMatrix(TargetFrame) * parentMarix);
		ndIk6DofEffector* const joint = new ndIk6DofEffector(childMarix, parentMarix, childBody, parentBody);

		joint->EnableRotationAxis(ndIk6DofEffector::m_shortestPath);
		joint->SetAngularSpringDamper(LinearRegularizer, LinearSpring, LinearDamper);
		joint->SetLinearSpringDamper(AngularRegularizer, AngularSpring, AngularDamper);
		joint->SetMaxForce(LinearMaxForce);
		joint->SetMaxTorque(AngularMaxTorque);

		const ndMatrix refFrame(joint->GetEffectorMatrix());
		m_referenceFrame = ToUnrealMatrix(refFrame);

		return joint;
	}
	return nullptr;
}

FTransform UNewtonJointLoop6dofEffector::GetTargetTransform() const
{
	check(m_joint);
	const ndIk6DofEffector* const joint = (ndIk6DofEffector*)m_joint;
	const ndMatrix matrix (joint->GetEffectorMatrix());
	return ToUnrealTransform(matrix);
}

void UNewtonJointLoop6dofEffector::SetTargetTransform(const FTransform& transform)
{
	check(m_joint);
	ndIk6DofEffector* const joint = (ndIk6DofEffector*)m_joint;

	const ndMatrix targetMatrix(ToNewtonMatrix(transform));
	const ndMatrix currentMatrix(joint->GetEffectorMatrix());
	const ndMatrix relative(targetMatrix * currentMatrix.OrthoInverse());

	bool isZero = true;
	for (ndInt32 i = 0; isZero && (i < 3); ++i)
	{
		isZero = isZero && (ndAbs(relative[i][i]) > ndFloat32(0.9999f)) && (ndAbs(relative.m_posit[i]) < ndFloat32(2.5e-3f));
	}

	if (!isZero)
	{
		AwakeBodies();
		joint->SetOffsetMatrix(targetMatrix);
	}
}

FVector UNewtonJointLoop6dofEffector::ClipRobotTarget()
{
	check(m_joint);
	ndIk6DofEffector* const joint = (ndIk6DofEffector*)m_joint;

	const ndMatrix refMatrix(ToNewtonMatrix(m_referenceFrame));
	const ndMatrix currentEffectorMatrix(joint->GetEffectorMatrix());

	ndFloat32 angle0 = ndAtan2(refMatrix.m_posit.m_y, refMatrix.m_posit.m_x);
	ndFloat32 angle = ndAtan2(currentEffectorMatrix.m_posit.m_y, currentEffectorMatrix.m_posit.m_x) - angle0;
	const ndVector localPosit(ndRollMatrix(angle).UnrotateVector(currentEffectorMatrix.m_posit) - refMatrix.m_posit);

	ndFloat32 azimuth = angle * ndRadToDegree;
	ndFloat32 x = (localPosit.m_x - 0.01f * ndSign(localPosit.m_x)) * UNREAL_UNIT_SYSTEM;
	ndFloat32 z = (localPosit.m_z - 0.01f * ndSign(localPosit.m_z)) * UNREAL_UNIT_SYSTEM;
	return FVector(x, azimuth, z);
}

void UNewtonJointLoop6dofEffector::SetRobotTarget(float x, float z, float azimuth, float pitch, float yaw, float roll)
{
	// save target for debugging porpuses 
	m_target.X = x;
	m_target.Z = z;
	m_targetYaw = yaw;
	m_targetRoll = roll;
	m_targetPitch = pitch;
	m_target.Azimuth = azimuth;

	check(m_joint);
	ndIk6DofEffector* const joint = (ndIk6DofEffector*)m_joint;

	// claculate the orientation
	yaw *= ndDegreeToRad; 
	roll *= ndDegreeToRad;
	pitch *= ndDegreeToRad;

	// calculate the target position
	azimuth *= ndDegreeToRad;
	x = x * UNREAL_INV_UNIT_SYSTEM;
	z = z * UNREAL_INV_UNIT_SYSTEM;

	// interpolate translation
	const ndMatrix refMatrix(ToNewtonMatrix(m_referenceFrame));
	const ndMatrix similarRotation(ndRollMatrix(ndAtan2(refMatrix.m_posit.m_y, refMatrix.m_posit.m_x)));
	const ndVector localPosit(similarRotation.UnrotateVector(refMatrix.m_posit));
	const ndVector planePosition(x, ndFloat32(0.0f), z, ndFloat32(0.0f));
	const ndVector target(localPosit + planePosition);

	const ndMatrix currentMatrix(joint->GetOffsetMatrix());
	const ndVector effectorPosit(similarRotation.UnrotateVector(currentMatrix.m_posit));
	ndFloat32 effectorAzimuth = ndAtan2(effectorPosit.m_y, effectorPosit.m_x);
	const ndMatrix effectorAzimuthRotation(ndRollMatrix(effectorAzimuth));
	ndVector currentPosit(effectorAzimuthRotation.UnrotateVector(effectorPosit));

	ndFloat32 longitudinalStep = 0.05f;
	for (ndInt32 i = 0; i < 3; ++i)
	{
		ndFloat32 step = target[i] - currentPosit[i];
		if (ndAbs(step) > longitudinalStep)
		{
			currentPosit[i] += longitudinalStep * ndSign(step);
		}
		else
		{
			currentPosit[i] = target[i];
		}
	}

	currentPosit = effectorAzimuthRotation.RotateVector(currentPosit);

	ndFloat32 slowAngularStep = ndDegreeToRad * 0.25f;
	ndFloat32 fastAngularStep = slowAngularStep * 2.0f;
	ndFloat32 deltaAzimuth = azimuth - effectorAzimuth;
	if (ndAbs(deltaAzimuth) > fastAngularStep)
	{
		azimuth = ndSign(deltaAzimuth) * fastAngularStep;
	}
	else if (ndAbs(deltaAzimuth) > slowAngularStep)
	{
		azimuth = ndSign(deltaAzimuth) * slowAngularStep;
	}
	else
	{
		azimuth = 0.0f;
	}

	const ndMatrix azimuthRotation(ndRollMatrix(azimuth));
	currentPosit = azimuthRotation.RotateVector(currentPosit);
	currentPosit = similarRotation.RotateVector(currentPosit);

	// interpolate rotations
	const ndMatrix targetMatrix(ndRollMatrix(roll) * ndYawMatrix(yaw) * ndPitchMatrix(pitch));
	ndQuaternion targetRotation(targetMatrix);
	const ndQuaternion currentRotation(currentMatrix);
	if (currentRotation.DotProduct(targetRotation).GetScalar() < 0.0f)
	{
		targetRotation = targetRotation.Scale(-1.0f);
	}
	ndVector omega(currentRotation.CalcAverageOmega(targetRotation, 1.0f));
	ndFloat32 omegaMag = ndSqrt(omega.DotProduct(omega).GetScalar());
	ndFloat32 omegaSpeed = 5.0f * ndDegreeToRad;

	ndQuaternion rotation(targetRotation);
	if (omegaMag > omegaSpeed)
	{
		omega = omega.Normalize().Scale(omegaSpeed);
		rotation = currentRotation.IntegrateOmega(omega, 1.0f);
	}
	ndMatrix matrix(ndCalculateMatrix(rotation, currentPosit));
	matrix.m_posit.m_w = 1.0f;
	SetTargetTransform(ToUnrealTransform(matrix));
}

FRobotTargetPosit UNewtonJointLoop6dofEffector::WorldLocationToEffectorSpace(const FVector& worldPosition)
{
	ndIk6DofEffector* const joint = (ndIk6DofEffector*)m_joint;

	const ndMatrix refMatrix(ToNewtonMatrix(m_referenceFrame));
	const ndMatrix similarRotation(ndRollMatrix(ndAtan2(refMatrix.m_posit.m_y, refMatrix.m_posit.m_x)));
	const ndVector refPosit(similarRotation.UnrotateVector(refMatrix.m_posit));
	
	const ndMatrix baseMatrix(joint->CalculateGlobalBaseMatrix1());
	const ndVector localPosit(baseMatrix.UntransformVector(ndVector(ndFloat32(worldPosition.X * UNREAL_INV_UNIT_SYSTEM), ndFloat32(worldPosition.Y * UNREAL_INV_UNIT_SYSTEM), ndFloat32(worldPosition.Z * UNREAL_INV_UNIT_SYSTEM), ndFloat32(0.0f))));
	const ndVector alignPosit(similarRotation.UnrotateVector(localPosit));

	ndFloat32 rollAngle = ndAtan2(alignPosit.m_y, alignPosit.m_x);
	const ndMatrix azimuthRotation(ndRollMatrix(rollAngle));
	const ndVector target(azimuthRotation.UnrotateVector(alignPosit));
	const ndVector params(target - refPosit);

	FRobotTargetPosit result;
	result.X = params.m_x * UNREAL_UNIT_SYSTEM;
	result.Z = params.m_z * UNREAL_UNIT_SYSTEM;
	result.Azimuth = rollAngle * ndRadToDegree;
	return result;
}
