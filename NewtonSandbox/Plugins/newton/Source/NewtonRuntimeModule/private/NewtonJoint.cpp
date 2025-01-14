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


#include "NewtonJoint.h"

#include "NewtonRigidBody.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

// Sets default values for this component's properties
UNewtonJoint::UNewtonJoint()
	:Super()
	,ShowDebug(false)
	,DebugScale(1.0f)
	,m_joint(nullptr)
	,m_propertyChanged(true)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UNewtonJoint::ClearDebug()
{
	ShowDebug = false;
	m_propertyChanged = true;
}

void UNewtonJoint::ActivateDebug()
{
	ShowDebug = true;
	m_propertyChanged = true;
}

ndJointBilateralConstraint* UNewtonJoint::CreateJoint()
{
	m_transfrom = GetComponentTransform();
	ApplyPropertyChanges();
	return nullptr;
}

void UNewtonJoint::DestroyJoint()
{
	if (m_joint)
	{
		ndBodyKinematic* const body = m_joint->GetBody0();
		check(body);

		ndWorld* const world = body->GetScene()->GetWorld();
		check(world);

		world->RemoveJoint(m_joint);
		m_joint = nullptr;
	}
}

void UNewtonJoint::AwakeBodies() const
{
	check(m_joint);
	ndBodyKinematic* const body0 = m_joint->GetBody0();
	ndBodyKinematic* const body1 = m_joint->GetBody1();
	body0->SetSleepState(false);
	body1->SetSleepState(false);
}

UNewtonRigidBody* UNewtonJoint::FindChild() const
{
	const TArray<TObjectPtr<USceneComponent>>& childArray = GetAttachChildren();
	if (childArray.Num())
	{
		check(childArray.Num() == 1);
		return Cast<UNewtonRigidBody>(childArray[0]);
	}
	return nullptr;
}

UNewtonRigidBody* UNewtonJoint::FindParent() const
{
	return Cast<UNewtonRigidBody>(GetAttachParent());
}

void UNewtonJoint::GetBodyPairs(ndBodyKinematic** body0, ndBodyKinematic** body1) const
{
	*body0 = nullptr;
	*body1 = nullptr;

	UNewtonRigidBody* const child = FindChild();
	if (child)
	{
		check(child->GetBody());
		*body0 = child->GetBody();
	}

	UNewtonRigidBody* const parent = FindParent();
	if (parent)
	{
		check(parent->GetBody());
		*body1 = parent->GetBody();
	}
	//else
	//{
	//	*body1 = world->GetSentinelBody();
	//}

	if (!body0)
	{
		UE_LOG(LogTemp, Warning, TEXT("The child of a: %s must be a UNewtonRigidBody. Failed to create joint"), *GetName());
	}

	if (!body1)
	{
		check(0);
		UE_LOG(LogTemp, Warning, TEXT("The parent of a: %s must be a UNewtonRigidBody. Failed to create joint"), *GetName());
	}
}

void UNewtonJoint::DrawGizmo(float timestep) const
{
	check(0);
}

void UNewtonJoint::UpdateTransform()
{
	UNewtonRigidBody* const parent = FindParent();
	if (parent)
	{
		m_transfrom = GetRelativeTransform() * parent->m_globalTransform;
	}
}

void UNewtonJoint::ApplyPropertyChanges()
{
	m_propertyChanged = false;
	const FTransform transform(GetComponentTransform());
	const UNewtonRigidBody* const child = FindChild();
	if (child)
	{
		const FTransform childTransform(child->GetComponentTransform());
		m_localChildTransfrom = transform * childTransform.Inverse();
	}
	
	const UNewtonRigidBody* const parent = FindParent();
	if (parent)
	{
		const FTransform parentTransform(parent->GetComponentTransform());
		m_localParentTransfrom = transform * parentTransform.Inverse();
	}
}

void UNewtonJoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	m_propertyChanged = true;
}

// Called every frame
void UNewtonJoint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetComponentToWorld(m_transfrom);
	if (ShowDebug)
	{
		DrawGizmo(DeltaTime);
	}
}

bool UNewtonJoint::HitLimits() const
{
	check(m_joint);
	return m_joint->GetJointHitLimits();
}
