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

#include "NewtonJointIk6DofEffector.h"
#include "Animation\Skeleton.h"

#include "NewtonModel.h"
#include "NewtonAsset.h"
#include "NewtonRigidBody.h"
#include "NewtonWorldActor.h"
#include "NewtonRuntimeModule.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonJointIk6DofEffector::UNewtonJointIk6DofEffector()
	:Super()
{
	ReferencedBodyName = TEXT("None");

	TargetFrame = FTransform();
	AngularDamper = 500.0f;
	AngularSpring = 10000.0f;
	AngularMaxTorque = 10000.0f;
	AngularRegularizer = 1.0e-5f;

	m_targetX = 0.0f;
	m_targetZ = 0.0f;
	m_targetYaw = 0.0f;
	m_targetRoll = 0.0f;
	m_targetPitch = 0.0f;
	m_targetAzimuth = 0.0f;

	LinearDamper = 500.0f;
	LinearSpring = 10000.0f;
	LinearMaxForce = 10000.0f;
	LinearRegularizer = 1.0e-5f;
	m_referenceFrame = FMatrix::Identity;
	//rotationType(ndIk6DofEffector::m_shortestPath)
	//controlDofOptions(0xff)
}

void UNewtonJointIk6DofEffector::DrawGizmo(float timestep) const
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
		const ndVector referencePoint(refMatrix.m_posit);
		const ndVector step(m_targetX * UNREAL_INV_UNIT_SYSTEM, ndFloat32(0.0f), m_targetZ * UNREAL_INV_UNIT_SYSTEM, ndFloat32(0.0f));
		const ndMatrix azimuthRotation(ndRollMatrix(m_targetAzimuth * ndDegreeToRad));
		const ndVector target(azimuthRotation.RotateVector(referencePoint + step));

		ndMatrix targetMatrix(ndRollMatrix(m_targetRoll * ndDegreeToRad) * ndYawMatrix(m_targetYaw * ndDegreeToRad) * ndPitchMatrix(m_targetPitch * ndDegreeToRad));
		targetMatrix.m_posit = target;
		targetMatrix.m_posit.m_w = 1.0f;

		const FTransform targetTransform(ToUnrealTransform(targetMatrix));
		DrawFrame(targetTransform * parentTransform);
	}
}

// Called when the game starts
ndJointBilateralConstraint* UNewtonJointIk6DofEffector::CreateJoint()
{
	Super::CreateJoint();
	
	check(!m_joint);

	AActor* const owner = GetOwner();
	check(owner);
	UNewtonModel* const model = owner->FindComponentByClass<UNewtonModel>();
	check(model);
	USkeleton* const skeleton = model->NewtonAsset->SkeletalMeshAsset->GetSkeleton();;
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

	//UNewtonRigidBody* const parentComponet = FindParent();
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

		//m_joint = joint;
		//world->AddJoint(m_joint);
		return joint;
	}
	return nullptr;
}


FTransform UNewtonJointIk6DofEffector::GetTargetTransform() const
{
	check(m_joint);
	const ndIk6DofEffector* const joint = (ndIk6DofEffector*)m_joint;
	const ndMatrix matrix (joint->GetEffectorMatrix());
	return ToUnrealTransform(matrix);
}

void UNewtonJointIk6DofEffector::SetTargetTransform(const FTransform& transform)
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

FVector UNewtonJointIk6DofEffector::ClipRobotTarget()
{
	check(m_joint);
	ndIk6DofEffector* const joint = (ndIk6DofEffector*)m_joint;

	const ndMatrix refMatrix(ToNewtonMatrix(m_referenceFrame));
	const ndMatrix currentEffectorMatrix(joint->GetEffectorMatrix());

	ndFloat32 angle0 = ndAtan2(refMatrix.m_posit.m_y, refMatrix.m_posit.m_x);
	ndFloat32 angle = ndAtan2(currentEffectorMatrix.m_posit.m_y, currentEffectorMatrix.m_posit.m_x) - angle0;
	const ndVector localPosit(ndRollMatrix(angle).UnrotateVector(currentEffectorMatrix.m_posit) - refMatrix.m_posit);
	ndFloat32 x = (localPosit.m_x - 0.01f * ndSign(localPosit.m_x)) * UNREAL_UNIT_SYSTEM;
	ndFloat32 z = (localPosit.m_z - 0.01f * ndSign(localPosit.m_z)) * UNREAL_UNIT_SYSTEM;
	ndFloat32 azimuth = angle * ndRadToDegree;
	return FVector(x, azimuth, z);
}

void UNewtonJointIk6DofEffector::SetRobotTarget(float x, float z, float azimuth, float pitch, float yaw, float roll)
{
	check(m_joint);
	ndIk6DofEffector* const joint = (ndIk6DofEffector*)m_joint;

	// save target for debugging porpuses 
	m_targetX = x;
	m_targetZ = z;
	m_targetYaw = yaw;
	m_targetRoll = roll;
	m_targetPitch = pitch;
	m_targetAzimuth = azimuth;

	const ndVector upPin(0.0f, 0.0f, 1.0f, 0.0f);
	const ndMatrix currentMatrix(joint->GetEffectorMatrix());
	const ndMatrix refMatrix(ToNewtonMatrix(m_referenceFrame));

	// claculate the orientation
	yaw *= ndDegreeToRad; 
	roll *= ndDegreeToRad;
	pitch *= ndDegreeToRad;
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
	ndMatrix matrix(ndCalculateMatrix(rotation, currentMatrix.m_posit));

	// calculate the target position
	azimuth *= ndDegreeToRad;
	x = x * UNREAL_INV_UNIT_SYSTEM;
	z = z * UNREAL_INV_UNIT_SYSTEM;

	const ndVector referencePoint(refMatrix.m_posit);
	const ndVector step(x, ndFloat32(0.0f), z, ndFloat32(0.0f));
	const ndVector source(currentMatrix.m_posit);

	const ndMatrix azimuthRotation(ndRollMatrix(azimuth));
	const ndVector target(azimuthRotation.RotateVector(referencePoint + step));

	ndFloat32 longitudinalStep = 0.05f;
	ndFloat32 angularStep = 2.0f * ndDegreeToRad;

	auto CalculateTargetPosition = [&source, &target, &upPin, longitudinalStep, angularStep]()
	{
		const ndVector mask(ndFloat32(1.0f), ndFloat32(1.0f), ndFloat32(1.0f), 0.0f);
		const ndVector src(source * mask);
		const ndVector dst(target * mask);
		const ndVector src1(src - upPin * (src.DotProduct(upPin)));
		const ndVector dst1(dst - upPin * (dst.DotProduct(upPin)));
		const ndVector srcDir(src1.Normalize());
		const ndVector dstDir(dst1.Normalize());
		ndFloat32 cosAngle(ndClamp(srcDir.DotProduct(dstDir).GetScalar(), ndFloat32(-1.0f), ndFloat32(1.0f)));
		ndFloat32 angle = ndAcos(cosAngle);
		
		ndVector result(src);
		if (ndAbs(angle) > angularStep)
		{
			const ndVector pin(srcDir.CrossProduct(dstDir).Normalize());
			const ndQuaternion rotation(pin, angularStep);
			result = rotation.RotateVector(result);
		}
		
		auto PlaneRotation = [&result, &dst, &upPin, longitudinalStep]()
		{
			const ndVector src1(result - upPin * (result.DotProduct(upPin)));
			const ndVector dst1(dst - upPin * (dst.DotProduct(upPin)));
			const ndVector srcDir(src1.Normalize());
			const ndVector dstDir(dst1.Normalize());
			ndFloat32 cosAngle(ndClamp(srcDir.DotProduct(dstDir).GetScalar(), ndFloat32(-1.0f), ndFloat32(1.0f)));
			ndFloat32 angle = ndAcos(cosAngle);
		
			ndVector target(dst);
			if (ndAbs(angle) > ndFloat32(1.0e-3f))
			{
				const ndVector pin(srcDir.CrossProduct(dstDir).Normalize());
				const ndQuaternion rotation(pin, angle);
				target = rotation.UnrotateVector(target);
			}
		
			for (ndInt32 i = 0; i < 3; ++i)
			{
				ndFloat32 step = target[i] - result[i];
				if (ndAbs(step) > longitudinalStep)
				{
					result[i] += longitudinalStep * ndSign(step);
				}
				else
				{
					result[i] = target[i];
				}
			}
			return result;
		};
		result = PlaneRotation();

		result.m_w = ndFloat32(1.0f);
		return result;
	};
	matrix.m_posit = CalculateTargetPosition();

	SetTargetTransform(ToUnrealTransform(matrix));
}