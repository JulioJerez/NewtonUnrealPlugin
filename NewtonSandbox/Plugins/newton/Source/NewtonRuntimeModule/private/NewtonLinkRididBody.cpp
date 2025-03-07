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

#include "NewtonLinkRigidBody.h"

#include "NewtonCommons.h"
#include "NewtonRigidBody.h"
#include "NewtonLinkJoint.h"
#include "NewtonLinkCollision.h"
#include "NewtonLinkJointVehicleMotor.h"
#include "NewtonLinkJointVehicleDifferential.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkRigidBody::UNewtonLinkRigidBody()
	:Super()
{
	Mass = 1.0f;
	Name = TEXT("rigidBody");
	BoneIndex = -1;
}

FVector UNewtonLinkRigidBody::CalculateLocalCenterOfMass(TObjectPtr<USkeletalMesh> mesh, int boneIndex,
	const FTransform& globalTransform, const TArray<const UNewtonLinkCollision*>& childen) const
{
	ndFloat32 volume = 0.0f;
	ndVector centerOfGravity (0.0f, 0.0f, 0.0f, 0.0f);
	for (int i = childen.Num() - 1; i >= 0; --i)
	{
		ndBodyKinematic body;
		const UNewtonLinkCollision* const shapeNode = childen[i];
		ndShapeInstance shape(shapeNode->CreateInstance(mesh, boneIndex));

		const ndMatrix bodyMatrix(ToNewtonMatrix(globalTransform));
		const ndMatrix shapeLocalMatrix(ToNewtonMatrix(shapeNode->Transform));
		FVector scale(globalTransform.GetScale3D() * shapeNode->Transform.GetScale3D());

		body.SetMatrix(bodyMatrix);
		shape.SetLocalMatrix(shapeLocalMatrix);
		shape.SetScale(ndVector(float(scale.X), float(scale.Y), float(scale.Z), float(1.0f)));
		body.SetIntrinsicMassMatrix(1.0f, shape);

		ndFloat32 vol = shape.GetVolume();
		centerOfGravity += body.GetCentreOfMass().Scale(vol);
		volume += vol;
	}
	centerOfGravity = centerOfGravity.Scale(UNREAL_UNIT_SYSTEM / volume);
	return FVector(centerOfGravity.m_x, centerOfGravity.m_y, centerOfGravity.m_y);
}

void UNewtonLinkRigidBody::PostCreate(const UNewtonLink* const parentNde)
{
	Super::PostCreate(parentNde);
	if (parentNde && Cast<UNewtonLinkJoint>(parentNde))
	{
		const UNewtonLinkJointVehicleDifferential* const differential = Cast<UNewtonLinkJointVehicleDifferential>(parentNde);
		if (differential)
		{
			Mass = differential->BodyMass;
		}
		const UNewtonLinkJointVehicleMotor* const motor = Cast<UNewtonLinkJointVehicleMotor>(parentNde);
		if (motor)
		{
			Mass = motor->BodyMass;
		}
	}
}

TObjectPtr<USceneComponent> UNewtonLinkRigidBody::CreateBlueprintProxy() const
{
	TObjectPtr<USceneComponent> component(NewObject<UNewtonRigidBody>(UNewtonRigidBody::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkRigidBody::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonRigidBody* const body = Cast<UNewtonRigidBody>(component.Get());
	check(body);

	body->ApplyPropertyChanges();

	body->Mass = Mass;
	body->BoneIndex = BoneIndex;
	body->ShowDebug = true;
	body->AutoSleepMode = true;
	body->ShowCenterOfMass = true;
	body->CenterOfMass = CenterOfMass;
}