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
#include "NewtonLinkJointDifferential.h"
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
	FVector com(0.0f, 0.0f, 0.0f);
	if (childen.Num() == 1)
	{
		ndBodyKinematic body;
		const UNewtonLinkCollision* const shapeNode = childen[0];
		ndShapeInstance shape(shapeNode->CreateInstance(mesh, boneIndex));
		
		const ndMatrix bodyMatrix(ToNewtonMatrix(globalTransform));
		const ndMatrix shapeLocalMatrix(ToNewtonMatrix(shapeNode->Transform));
		FVector scale(globalTransform.GetScale3D() * shapeNode->Transform.GetScale3D());
		
		body.SetMatrix(bodyMatrix);
		shape.SetLocalMatrix(shapeLocalMatrix);
		shape.SetScale(ndVector(float(scale.X), float(scale.Y), float(scale.Z), float (1.0f)));
		body.SetIntrinsicMassMatrix(1.0f, shape);
		
		ndVector centerOfGravity(body.GetCentreOfMass());
		
		com.X = centerOfGravity.m_x * UNREAL_UNIT_SYSTEM;
		com.Y = centerOfGravity.m_y * UNREAL_UNIT_SYSTEM;
		com.Z = centerOfGravity.m_z * UNREAL_UNIT_SYSTEM;
	}
	else if (childen.Num() > 1)
	{
		check(0);
		//for (int i = childen.Num() - 1; i >= 0; --i)
		//{
		//	const UNewtonLinkCollision* const shapeNode = childen[i];
		//	ndShapeInstance instance(shapeNode->CreateInstance());
		//}
	}

	return com;
}

void UNewtonLinkRigidBody::PostCreate(const UNewtonLink* const parentNde)
{
	Super::PostCreate(parentNde);
	if (parentNde && Cast<UNewtonLinkJoint>(parentNde))
	{
		const UNewtonLinkJointDifferential* const differential = Cast<UNewtonLinkJointDifferential>(parentNde);
		if (differential)
		{
			Mass = differential->BodyMass;
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