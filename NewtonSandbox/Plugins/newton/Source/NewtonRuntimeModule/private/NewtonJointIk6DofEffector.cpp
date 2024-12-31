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

	LinearDamper = 500.0f;
	LinearSpring = 10000.0f;
	LinearMaxForce = 10000.0f;
	LinearRegularizer = 1.0e-5f;
	//rotationType(ndIk6DofEffector::m_shortestPath)
	//controlDofOptions(0xff)
}

void UNewtonJointIk6DofEffector::DrawGizmo(float timestep) const
{
	float thickness = 0.5f;
	const UWorld* const world = GetWorld();
	ndFloat32 scale = DebugScale * UNREAL_UNIT_SYSTEM;
	
	const FTransform parentTransform(GetComponentTransform());
	const FVector positionParent(parentTransform.GetLocation());
	const FVector xAxisParent(parentTransform.GetUnitAxis(EAxis::X));
	const FVector yAxisParent(parentTransform.GetUnitAxis(EAxis::Y));
	const FVector zAxisParent(parentTransform.GetUnitAxis(EAxis::Z));
	DrawDebugLine(world, positionParent, positionParent + scale * xAxisParent, FColor::Red, false, timestep, thickness);
	DrawDebugLine(world, positionParent, positionParent + scale * yAxisParent, FColor::Green, false, timestep, thickness);
	DrawDebugLine(world, positionParent, positionParent + scale * zAxisParent, FColor::Blue, false, timestep, thickness);

	const FTransform childTransform(TargetFrame * parentTransform);
	const FVector positionChild(childTransform.GetLocation());
	const FVector xAxisChild(childTransform.GetUnitAxis(EAxis::X));
	const FVector yAxisChild(childTransform.GetUnitAxis(EAxis::Y));
	const FVector zAxisChild(childTransform.GetUnitAxis(EAxis::Z));
	DrawDebugLine(world, positionChild, positionChild + scale * xAxisChild, FColor::Red, false, timestep, thickness);
	DrawDebugLine(world, positionChild, positionChild + scale * yAxisChild, FColor::Green, false, timestep, thickness);
	DrawDebugLine(world, positionChild, positionChild + scale * zAxisChild, FColor::Blue, false, timestep, thickness);
}

// Called when the game starts
void UNewtonJointIk6DofEffector::CreateJoint(ANewtonWorldActor* const newtonWorldActor)
{
	Super::CreateJoint(newtonWorldActor);
	
	check(!m_joint);

	UNewtonRigidBody* childComponent = nullptr;
	ndFixSizeArray<USceneComponent*, ND_STACK_DEPTH> stack;
	stack.PushBack(GetOwner()->GetRootComponent());
	while (stack.GetCount())
	{
		USceneComponent* const component = stack.Pop();
		check(component);
		if (component->GetName() == ReferencedBodyName)
		{
			childComponent = Cast<UNewtonRigidBody>(component);
			break;
		}
	
		const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
		for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
		{
			stack.PushBack(childrenComp[i].Get());
		}
	}

	UNewtonRigidBody* const parentComponet = FindParent();
	if (parentComponet && childComponent)
	{
		ndWorld* const world = newtonWorldActor->GetNewtonWorld();
		ndBodyKinematic* const childBody = childComponent->m_body;
		ndBodyKinematic* const parentBody = parentComponet->m_body;
		const FTransform transform(GetRelativeTransform());
		const ndMatrix parentMarix(ToNewtonMatrix(transform) * parentBody->GetMatrix());
		const ndMatrix childMarix(ToNewtonMatrix(TargetFrame) * parentMarix);
		ndIk6DofEffector* const joint = new ndIk6DofEffector(childMarix, parentMarix, childBody, parentBody);

		joint->EnableRotationAxis(ndIk6DofEffector::m_shortestPath);
		joint->SetAngularSpringDamper(LinearRegularizer, LinearSpring, LinearDamper);
		joint->SetLinearSpringDamper(AngularRegularizer, AngularSpring, AngularDamper);
		joint->SetMaxForce(LinearMaxForce);
		joint->SetMaxTorque(AngularMaxTorque);

		m_joint = joint;
		world->AddJoint(m_joint);
	}
}
