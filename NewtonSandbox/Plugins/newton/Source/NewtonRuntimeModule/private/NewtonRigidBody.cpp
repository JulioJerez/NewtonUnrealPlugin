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


#include "NewtonRigidBody.h"
#include "Selection.h"
#include "LevelEditor.h"
#include "ContentBrowserModule.h"
#include "Misc/OutputDeviceNull.h"
#include "Modules/ModuleManager.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/BodySetup.h"

#include "NewtonJoint.h"
#include "NewtonCollision.h"
#include "NewtonWorldActor.h"
#include "NewtonRuntimeModule.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

//FLinearColor UNewtonRigidBody::m_awakeColor(1.0f, 0.0f, 0.f);
//FLinearColor UNewtonRigidBody::m_sleepingColor(0.0f, 1.0f, 0.f);

FLinearColor UNewtonRigidBody::m_awakeColor(0.0f, 0.5f, 1.0f);
FLinearColor UNewtonRigidBody::m_sleepingColor(0.0f, 0.125f, 0.25f);

class UNewtonRigidBody::NotifyCallback : public ndBodyNotify
{
	public:
	NotifyCallback(UNewtonRigidBody* const owner, const ndVector& gravity)
		:ndBodyNotify(gravity)
		,m_owner(owner)
		,m_sleepState(false)
	{
		const FTransform transform(owner->GetComponentTransform());
		const FVector location(transform.GetLocation());
		m_posit0.m_x = ndFloat32(location.X * UNREAL_INV_UNIT_SYSTEM);
		m_posit0.m_y = ndFloat32(location.Y * UNREAL_INV_UNIT_SYSTEM);
		m_posit0.m_z = ndFloat32(location.Z * UNREAL_INV_UNIT_SYSTEM);
		m_posit0.m_w = ndFloat32(1.0f);
		m_posit1 = m_posit0;

		const FQuat rot (transform.Rotator().Quaternion());
		m_rotation0.m_x = ndFloat32(rot.X);
		m_rotation0.m_y = ndFloat32(rot.Y);
		m_rotation0.m_z = ndFloat32(rot.Z);
		m_rotation0.m_w = ndFloat32(rot.W);

		m_timestep = 0.0f;
		AActor* const actorOwner = m_owner->GetOwner();
		FString onforceAndTRqueName(ND_RIGID_BODIES_EVENT_NAME);
		m_onForceAndTorque = actorOwner->FindFunction(FName(*onforceAndTRqueName));
	}

	NotifyCallback(const NotifyCallback& src)
		:ndBodyNotify(src)
		,m_posit0(src.m_posit0)
		,m_posit1(src.m_posit1)
		,m_rotation0(src.m_rotation0)
		,m_rotation1(src.m_rotation1)
		,m_owner(src.m_owner)
		,m_sleepState(false)
	{
		m_timestep = 0.0f;
		AActor* const actorOwner = m_owner->GetOwner();
		FString onforceAndTRqueName(ND_RIGID_BODIES_EVENT_NAME);
		m_onForceAndTorque = actorOwner->FindFunction(FName(*onforceAndTRqueName));
	}

	~NotifyCallback()
	{
	}

	ndBodyNotify* Clone() const
	{
		return new NotifyCallback(*this);
	}

	virtual bool OnSceneAabbOverlap(const ndBody* const otherBody) const
	{
		return true;
	}

	virtual void* GetUserData() const
	{
		return m_owner;
	}

	void UpdateTransform()
	{
		m_posit0 = m_posit1;
		m_rotation0 = m_rotation1;
		m_posit1 = GetBody()->GetMatrix().m_posit;
		m_rotation1 = GetBody()->GetRotation();

		if (m_rotation0.DotProduct(m_rotation1).GetScalar() < 0.0f)
		{
			m_rotation0 = m_rotation0.Scale(-1.0f);
		}
	}

	FTransform InteptolateTransform(ndFloat32 param)
	{
		const ndVector posit(m_posit0 + (m_posit1 - m_posit0).Scale(param));
		const ndQuaternion rotation(m_rotation0.Slerp(m_rotation1, param));
		const FQuat uRot(rotation.m_x, rotation.m_y, rotation.m_z, rotation.m_w);
		const FVector uPosit(posit.m_x * UNREAL_UNIT_SYSTEM, posit.m_y * UNREAL_UNIT_SYSTEM, posit.m_z * UNREAL_UNIT_SYSTEM);

		FTransform transform;
		transform.SetRotation(uRot);
		transform.SetLocation(uPosit);
		return transform;
	}

	virtual void OnApplyExternalForce(ndInt32 threadIndex, ndFloat32 timestep)
	{
		ndBodyDynamic* const body = GetBody()->GetAsBodyDynamic();
		const ndVector force(GetGravity().Scale(body->GetMassMatrix().m_w));
		body->SetForce(force);
		if (m_onForceAndTorque)
		{
			m_timestep = timestep;
			CallBlueprintEvent();
		}
	}

	void CallBlueprintFunction()
	{
		FOutputDeviceNull ar;
		AActor* const actorOwner = m_owner->GetOwner();
		FString command(ND_RIGID_BODIES_EVENT_NAME);
		bool test = actorOwner->CallFunctionByNameWithArguments(*command, ar, nullptr, true);
		check(test);
	}

	void CallBlueprintEvent()
	{
		AActor* const actorOwner = m_owner->GetOwner();
		actorOwner->ProcessEvent(m_onForceAndTorque, nullptr);
	}

	ndVector m_posit0;
	ndVector m_posit1;
	ndQuaternion m_rotation0;
	ndQuaternion m_rotation1;
	UNewtonRigidBody* m_owner;
	ndFloat32 m_timestep;
	UFunction* m_onForceAndTorque;
	bool m_sleepState;
};

// Sets default values for this component's properties
UNewtonRigidBody::UNewtonRigidBody()
	:Super()
	,ShowDebug(false)
	,ShowCenterOfMass(false)
	,AutoSleepMode(true)
	,DebugScale(1.0f)
	,Mass(0.0f)
	,LinearDamp(0.0f)
	,AngularDamp(0.0f)
	,InitialVeloc(0.0f, 0.0f, 0.0f)
	,InitialOmega(0.0f, 0.0f, 0.0f)
	,CenterOfMass(0.0f, 0.0f, 0.0f)
	,Gravity(0.0f, 0.0f, -980.0f)
	,m_localScale(1.0f, 1.0f, 1.0f)
	,m_globalScale(1.0f, 1.0f, 1.0f)
	,m_localTransform()
	,m_globalTransform()
	,m_body(nullptr)
	,m_newtonWorld(nullptr)
	,m_sleeping(true)
	,m_propertyChanged(true)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetMobility(EComponentMobility::Movable);
}

void UNewtonRigidBody::BeginDestroy()
{
	Super::BeginDestroy();
}

void UNewtonRigidBody::DestroyRigidBody()
{
	if (m_body)
	{
		ndWorld* const world = m_body->GetScene()->GetWorld();
		if (world)
		{
			world->RemoveBody(m_body);
		}
		m_body = nullptr;
	}
}

void UNewtonRigidBody::PostLoad()
{
	Super::PostLoad();
	m_propertyChanged = true;
}

void UNewtonRigidBody::OnRegister()
{
	Super::OnRegister();
	m_propertyChanged = true;
}

void UNewtonRigidBody::OnUnregister()
{
	Super::OnUnregister();
}

void UNewtonRigidBody::OnChildAttached(USceneComponent* component)
{
	Super::OnChildAttached(component);
	m_propertyChanged = true;
}

void UNewtonRigidBody::OnChildDetached(USceneComponent* component)
{
	Super::OnChildDetached(component);
	m_propertyChanged = true;
}

void UNewtonRigidBody::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	m_propertyChanged = true;
}

void UNewtonRigidBody::ClearDebug()
{
	ShowDebug = false;
	ShowCenterOfMass = false;
	m_propertyChanged = true;
	Inertia.ShowPrincipalAxis = false;

	ndFixSizeArray<USceneComponent*, 1024> stack;
	stack.PushBack(this);
	while (stack.GetCount())
	{
		USceneComponent* const component = stack.Pop();
		UNewtonCollision* const collisionComponent = Cast<UNewtonCollision>(component);
		if (collisionComponent)
		{
			collisionComponent->m_debugVisualIsDirty = true;
			collisionComponent->ApplyPropertyChanges();
		}
		const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
		for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
		{
			stack.PushBack(childrenComp[i].Get());
		}
	}
}

void UNewtonRigidBody::ActivateDebug()
{
	ShowDebug = true;
	//ShowCenterOfMass = true;
	m_propertyChanged = true;
	//Inertia.ShowPrincipalAxis = true;

	ndFixSizeArray<USceneComponent*, 1024> stack;
	stack.PushBack(this);
	while (stack.GetCount())
	{
		USceneComponent* const component = stack.Pop();
		UNewtonCollision* const collisionComponent = Cast<UNewtonCollision>(component);
		if (collisionComponent)
		{
			collisionComponent->m_debugVisualIsDirty = true;
			collisionComponent->ApplyPropertyChanges();
		}
		const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
		for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
		{
			stack.PushBack(childrenComp[i].Get());
		}
	}
}

void UNewtonRigidBody::DrawGizmo(float timestep)
{
	if (Inertia.ShowPrincipalAxis && (Mass > 0.0f))
	{
		ndBodyKinematic body;
		const ndMatrix matrix(ToNewtonMatrix(m_globalTransform));
		body.SetMatrix(matrix);
		ndSharedPtr<ndShapeInstance> shape(CreateCollision(matrix));

		FTransform transform;
		transform.SetRotation(FQuat(Inertia.PrincipalInertiaAxis));
		const ndMatrix shapeMatrix(ToNewtonMatrix(transform));
		shape->SetLocalMatrix(shapeMatrix * shape->GetLocalMatrix());

		bool fullInertia = ndAbs(Inertia.PrincipalInertiaAxis.Pitch) > 0.1f;
		fullInertia = fullInertia || (ndAbs(Inertia.PrincipalInertiaAxis.Yaw) > 0.1f);
		fullInertia = fullInertia || (ndAbs(Inertia.PrincipalInertiaAxis.Roll) > 0.1f);
		body.SetIntrinsicMassMatrix(Mass, **shape, fullInertia);

		ndVector centerOfGravity(body.GetCentreOfMass());
		centerOfGravity += ndVector(ndFloat32(CenterOfMass.X * UNREAL_INV_UNIT_SYSTEM), ndFloat32(CenterOfMass.Y * UNREAL_INV_UNIT_SYSTEM), ndFloat32(CenterOfMass.Z * UNREAL_INV_UNIT_SYSTEM), ndFloat32(0.0f));
		centerOfGravity = matrix.TransformVector(centerOfGravity);

		const ndVector massMatrix(body.GetMassMatrix());
		const ndMatrix hygenAxis(fullInertia ? body.GetPrincipalAxis() : ndGetIdentityMatrix());
		ndMatrix diagonal(ndGetIdentityMatrix());
		diagonal[0][0] = massMatrix[0] * Inertia.PrincipalInertiaScaler.X;
		diagonal[1][1] = massMatrix[1] * Inertia.PrincipalInertiaScaler.Y;
		diagonal[2][2] = massMatrix[2] * Inertia.PrincipalInertiaScaler.Z;
		const ndMatrix fullScaledIntertia(hygenAxis.OrthoInverse() * diagonal * hygenAxis);
		body.SetMassMatrix(Mass, fullScaledIntertia);

		body.SetMatrix(ndGetIdentityMatrix());
		ndMatrix inertiaMatrix(body.CalculateInertiaMatrix());
		inertiaMatrix.EigenVectors();
		const ndMatrix axisMatrix(inertiaMatrix * matrix);
		const FTransform axisTranform(ToUnRealTransform(axisMatrix));
		const FRotator axisRot(axisTranform.GetRotation());
		const FVector axisLoc(centerOfGravity.m_x * UNREAL_UNIT_SYSTEM, centerOfGravity.m_y * UNREAL_UNIT_SYSTEM, centerOfGravity.m_z * UNREAL_UNIT_SYSTEM);
		DrawDebugCoordinateSystem(GetWorld(), axisLoc, axisRot, DebugScale * UNREAL_UNIT_SYSTEM, false, timestep);
	}

	if (ShowCenterOfMass && (Mass > 0.0f))
	{
		ndBodyKinematic body;
		ndMatrix matrix(ToNewtonMatrix(m_globalTransform));
		body.SetMatrix(matrix);
		ndSharedPtr<ndShapeInstance> shape(CreateCollision(matrix));

		//FTransform transform;
		//transform.SetRotation(FQuat(Inertia.PrincipalInertiaAxis));
		//const ndMatrix shapeMatrix(ToNewtonMatrix(transform));
		//shape->SetLocalMatrix(shapeMatrix * shape->GetLocalMatrix());

		bool fullInertia = ndAbs(Inertia.PrincipalInertiaAxis.Pitch) > 0.1f;
		fullInertia = fullInertia || (ndAbs(Inertia.PrincipalInertiaAxis.Yaw) > 0.1f);
		fullInertia = fullInertia || (ndAbs(Inertia.PrincipalInertiaAxis.Roll) > 0.1f);

		body.SetIntrinsicMassMatrix(Mass, **shape, fullInertia);
		ndVector centerOfGravity(body.GetCentreOfMass());
		centerOfGravity += ndVector(ndFloat32(CenterOfMass.X * UNREAL_INV_UNIT_SYSTEM), ndFloat32(CenterOfMass.Y * UNREAL_INV_UNIT_SYSTEM), ndFloat32(CenterOfMass.Z * UNREAL_INV_UNIT_SYSTEM), ndFloat32(0.0f));
		centerOfGravity = matrix.TransformVector(centerOfGravity);

		const FTransform transform(ToUnRealTransform(matrix));
		const FRotator axisRot(transform.GetRotation());
		const FVector axisLoc(centerOfGravity.m_x * UNREAL_UNIT_SYSTEM, centerOfGravity.m_y * UNREAL_UNIT_SYSTEM, centerOfGravity.m_z * UNREAL_UNIT_SYSTEM);
		DrawDebugCoordinateSystem(GetWorld(), axisLoc, axisRot, DebugScale * UNREAL_UNIT_SYSTEM, false, timestep);
	}

	if (ShowDebug && m_body)
	{
		bool sleepState = m_body->GetSleepState();
		if (m_sleeping && !sleepState)
		{
			ndFixSizeArray<USceneComponent*, 1024> stack;
			stack.PushBack(this);
			while (stack.GetCount())
			{
				USceneComponent* const component = stack.Pop();
				UNewtonCollision* const shape = Cast<UNewtonCollision>(component);
				if (shape)
				{
					shape->SetWireFrameColor(m_awakeColor);
				}

				const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
				for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
				{
					stack.PushBack(childrenComp[i].Get());
				}
			}
		}
		else if (!m_sleeping && sleepState)
		{
			ndFixSizeArray<USceneComponent*, 1024> stack;
			stack.PushBack(this);
			while (stack.GetCount())
			{
				USceneComponent* const component = stack.Pop();
				UNewtonCollision* const shape = Cast<UNewtonCollision>(component);
				if (shape)
				{
					shape->SetWireFrameColor(m_sleepingColor);
				}

				const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
				for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
				{
					stack.PushBack(childrenComp[i].Get());
				}
			}
		}
		m_sleeping = sleepState;
	}
}

void UNewtonRigidBody::UpdateTransform()
{
	check(m_body);
	NotifyCallback* const notify = (NotifyCallback*)m_body->GetNotifyCallback();
	notify->UpdateTransform();
}

void UNewtonRigidBody::InterpolateTransform(float param)
{
	check(m_body);
	NotifyCallback* const notify = (NotifyCallback*)m_body->GetNotifyCallback();
	m_globalTransform = notify->InteptolateTransform(ndFloat32 (param));
	m_globalTransform.SetScale3D(m_globalScale);
}

void UNewtonRigidBody::CalculateLocalTransform()
{
	check(m_body);
	FTransform parentTransform;
	const USceneComponent* const parent = GetAttachParent();
	if (parent)
	{
		parentTransform = parentTransform = parent->GetComponentTransform();;
	}

	m_localTransform = m_globalTransform * parentTransform.Inverse();
	m_localTransform.SetScale3D(m_localScale);
}

void UNewtonRigidBody::CreateRigidBody(ANewtonWorldActor* const worldActor, bool overrideAutoSleep)
{
	m_newtonWorld = worldActor;
	const ndMatrix matrix(ToNewtonMatrix(m_globalTransform));
	m_body = new ndBodyDynamic();
	m_body->SetMatrix(matrix);

	ndSharedPtr<ndShapeInstance> shape(CreateCollision(matrix));
	m_body->SetCollisionShape(**shape);

	FTransform transform;
	transform.SetRotation(FQuat(Inertia.PrincipalInertiaAxis));
	const ndMatrix shapeMatrix(ToNewtonMatrix(transform));
	shape->SetLocalMatrix(shapeMatrix * shape->GetLocalMatrix());

	bool fullInertia = ndAbs(Inertia.PrincipalInertiaAxis.Pitch) > 0.1f;
	fullInertia = fullInertia || (ndAbs(Inertia.PrincipalInertiaAxis.Yaw) > 0.1f);
	fullInertia = fullInertia || (ndAbs(Inertia.PrincipalInertiaAxis.Roll) > 0.1f);
	// Unreal meshes tend to have the origin at the zero value in the z direction,
	// this causes SetMassMatrix to generate an skew inertia because of the 
	// perpendicular axis theorem central. 
	// when in reality the com is a the geometric center 
	// and the inertia is relative to that point.
	// SetIntrinsicMassMatrix does that.   
	m_body->SetIntrinsicMassMatrix(Mass, **shape, fullInertia);

	const ndVector massMatrix(m_body->GetMassMatrix());
	const ndMatrix hygenAxis(fullInertia ? m_body->GetPrincipalAxis() : ndGetIdentityMatrix());
	ndMatrix diagonal(ndGetIdentityMatrix());
	diagonal[0][0] = massMatrix[0] * Inertia.PrincipalInertiaScaler.X;
	diagonal[1][1] = massMatrix[1] * Inertia.PrincipalInertiaScaler.Y;
	diagonal[2][2] = massMatrix[2] * Inertia.PrincipalInertiaScaler.Z;
	const ndMatrix fullScaledIntertia(hygenAxis.OrthoInverse() * diagonal * hygenAxis);
	m_body->SetMassMatrix(Mass, fullScaledIntertia);

	ndVector centerOfGravity(m_body->GetCentreOfMass());
	centerOfGravity += ndVector(ndFloat32(CenterOfMass.X * UNREAL_INV_UNIT_SYSTEM), ndFloat32(CenterOfMass.Y * UNREAL_INV_UNIT_SYSTEM), ndFloat32(CenterOfMass.Z * UNREAL_INV_UNIT_SYSTEM), ndFloat32(0.0f));
	m_body->SetCentreOfMass(centerOfGravity);

	m_body->SetAutoSleep(AutoSleepMode && overrideAutoSleep);
	m_body->SetNotifyCallback(new NotifyCallback(this, ndVector(ndFloat32(Gravity.X * UNREAL_INV_UNIT_SYSTEM), ndFloat32(Gravity.Y * UNREAL_INV_UNIT_SYSTEM), ndFloat32(Gravity.Z * UNREAL_INV_UNIT_SYSTEM), ndFloat32(0.0f))));

	m_body->SetLinearDamping(LinearDamp);
	m_body->SetAngularDamping(ndVector(AngularDamp));

	const ndVector omega(ndVector(ndFloat32(InitialOmega.X), ndFloat32(InitialOmega.Y), ndFloat32(InitialOmega.Z), ndFloat32(0.0f)));
	const ndVector veloc(ndFloat32(InitialVeloc.X * UNREAL_INV_UNIT_SYSTEM), ndFloat32(InitialVeloc.Y * UNREAL_INV_UNIT_SYSTEM), ndFloat32(InitialVeloc.Z * UNREAL_INV_UNIT_SYSTEM), ndFloat32(0.0f));
	m_body->SetOmega(matrix.RotateVector(omega));
	m_body->SetVelocity(matrix.RotateVector(veloc));

	ndWorld* const world = m_newtonWorld->GetNewtonWorld();
	world->AddBody(m_body);

	AActor* const actor = GetOwner();
	m_sleeping = false;
	ndFixSizeArray<USceneComponent*, 1024> stack;
	stack.PushBack(this);
	while (stack.GetCount())
	{
		USceneComponent* const component = stack.Pop();
		UNewtonCollision* const collision = Cast<UNewtonCollision>(component);
		if (collision)
		{
			collision->SetWireFrameColor(m_awakeColor);
		}

		const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
		for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
		{
			stack.PushBack(childrenComp[i].Get());
		}
	}
}

ndShapeInstance* UNewtonRigidBody::CreateCollision(const ndMatrix& bodyMatrix) const
{
	ndArray<UNewtonCollision*> collisionShapes;
	ndFixSizeArray<USceneComponent*, 1024> stack;
	stack.PushBack((USceneComponent*)this);
	while (stack.GetCount())
	{
		USceneComponent* const component = stack.Pop();
		UNewtonCollision* const shape = Cast<UNewtonCollision>(component);
		if (shape)
		{
			shape->ApplyPropertyChanges();
			collisionShapes.PushBack(shape);
		}
		const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
		for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
		{
			USceneComponent* const childComponent = childrenComp[i].Get();
			if (!(Cast<UNewtonRigidBody>(childComponent) || Cast<UNewtonJoint>(childComponent)))
			{
				stack.PushBack(childrenComp[i].Get());
			}
		}
	}

	if (collisionShapes.GetCount() == 0)
	{
		return new ndShapeInstance(new ndShapeNull());
	}

	if (collisionShapes.GetCount() == 1)
	{
		return collisionShapes[0]->CreateBodyInstanceShape(bodyMatrix);
	}

	ndShapeInstance* const compoundInstance = new ndShapeInstance(new ndShapeCompound());
	ndShapeCompound* const compound = compoundInstance->GetShape()->GetAsShapeCompound();
	compound->BeginAddRemove();
	for (ndInt32 i = collisionShapes.GetCount() - 1; i >= 0; --i)
	{
		ndShapeInstance* const subShape = collisionShapes[i]->CreateBodyInstanceShape(bodyMatrix);
		check(subShape->GetShape()->GetAsShapeCompound() == nullptr);
		compound->AddCollision(subShape);
		delete subShape;
	}
	compound->EndAddRemove();
	return compoundInstance;
}

void UNewtonRigidBody::ApplyPropertyChanges()
{
	m_propertyChanged = false;

	m_localTransform = GetRelativeTransform();
	m_globalTransform = GetComponentTransform();

	m_localScale = m_localTransform.GetScale3D();
	m_globalScale = m_globalTransform.GetScale3D();

	Inertia.PrincipalInertia = FVector(0.0f, 0.0f, 0.0f);
	if (Mass > 0.0f)
	{
		ndBodyKinematic body;
		const ndMatrix matrix(ToNewtonMatrix(m_globalTransform));
		body.SetMatrix(matrix);
		ndSharedPtr<ndShapeInstance> shape(CreateCollision(matrix));

		FTransform transform;
		transform.SetRotation(FQuat(Inertia.PrincipalInertiaAxis));
		const ndMatrix shapeMatrix(ToNewtonMatrix(transform));
		shape->SetLocalMatrix(shapeMatrix * shape->GetLocalMatrix());

		bool fullInertia = ndAbs(Inertia.PrincipalInertiaAxis.Pitch) > 0.1f;
		fullInertia = fullInertia || (ndAbs(Inertia.PrincipalInertiaAxis.Yaw) > 0.1f);
		fullInertia = fullInertia || (ndAbs(Inertia.PrincipalInertiaAxis.Roll) > 0.1f);

		body.SetIntrinsicMassMatrix(Mass, **shape, fullInertia);

		const ndVector massMatrix(body.GetMassMatrix());
		const ndMatrix hygenAxis(fullInertia ? body.GetPrincipalAxis() : ndGetIdentityMatrix());
		ndMatrix diagonal(ndGetIdentityMatrix());
		diagonal[0][0] = massMatrix[0] * Inertia.PrincipalInertiaScaler.X;
		diagonal[1][1] = massMatrix[1] * Inertia.PrincipalInertiaScaler.Y;
		diagonal[2][2] = massMatrix[2] * Inertia.PrincipalInertiaScaler.Z;
		const ndMatrix fullScaledIntertia(hygenAxis.OrthoInverse() * diagonal * hygenAxis);
		body.SetMassMatrix(Mass, fullScaledIntertia);

		const ndVector scaledMassMatrix(body.GetMassMatrix());
		ndFloat32 scale2 = UNREAL_UNIT_SYSTEM * UNREAL_UNIT_SYSTEM;
		Inertia.PrincipalInertia = FVector(scaledMassMatrix.m_x * scale2, scaledMassMatrix.m_y * scale2, scaledMassMatrix.m_z * scale2);
	}
}

// Called every frame
void UNewtonRigidBody::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (m_newtonWorld)
	{
		DrawGizmo(DeltaTime);
		SetRelativeTransform(m_localTransform);
		SetComponentToWorld(m_globalTransform);
	}
}

//************************************************
//
// Blueprint interface
// 
//************************************************
float UNewtonRigidBody::GetTimeStep() const
{
	NotifyCallback* const notify = (NotifyCallback*)m_body->GetNotifyCallback();
	return notify ? notify->m_timestep : 0.0f;
}

float UNewtonRigidBody::GetMass() const
{
	return m_body ? m_body->GetMassMatrix().m_w : 0.0f;
}

FVector UNewtonRigidBody::GetInertia() const
{
	FVector tmp(0.0f, 0.0f, 0.0f);
	if (m_body)
	{
		const ndVector i(m_body->GetMassMatrix().Scale(UNREAL_UNIT_SYSTEM * UNREAL_UNIT_SYSTEM));
		tmp.X = i.m_x;
		tmp.Y = i.m_y;
		tmp.Z = i.m_z;
	}
	return tmp;
}

FVector UNewtonRigidBody::GetFrontDir() const
{
	FVector tmp(1.0f, 0.0f, 0.0f);
	if (m_body)
	{
		const ndMatrix matrix(m_body->GetMatrix());
		tmp.X = matrix[0].m_x;
		tmp.Y = matrix[0].m_y;
		tmp.Z = matrix[0].m_z;
	}
	return tmp;
}

FVector UNewtonRigidBody::GetRightDir() const
{
	FVector tmp(0.0f, 1.0f, 0.0f);
	if (m_body)
	{
		const ndMatrix matrix(m_body->GetMatrix());
		tmp.X = matrix[1].m_x;
		tmp.Y = matrix[1].m_y;
		tmp.Z = matrix[1].m_z;
	}
	return tmp;
}

FVector UNewtonRigidBody::GetUpDir() const
{
	FVector tmp(0.0f, 0.0f, 1.0f);
	if (m_body)
	{
		const ndMatrix matrix(m_body->GetMatrix());
		tmp.X = matrix[2].m_x;
		tmp.Y = matrix[2].m_y;
		tmp.Z = matrix[2].m_z;
	}
	return tmp;
}

FVector UNewtonRigidBody::GetVelocity() const
{
	FVector tmp(0.0f, 0.0f, 0.0f);
	if (m_body)
	{
		const ndVector f(m_body->GetVelocity().Scale(UNREAL_UNIT_SYSTEM));
		tmp.X = f.m_x;
		tmp.Y = f.m_y;
		tmp.Z = f.m_z;
	}
	return tmp;
}

void UNewtonRigidBody::SetVelocity(const FVector& veloc) const
{
	if (m_body)
	{
		const ndVector v(ndFloat32(veloc.X), ndFloat32(veloc.Y), ndFloat32(veloc.Z), ndFloat32(0.0f));
		const ndVector tmp(v.Scale(UNREAL_INV_UNIT_SYSTEM));
		m_body->SetVelocity(tmp);
	}
}

FVector UNewtonRigidBody::GetOmega() const
{
	FVector tmp(0.0f, 0.0f, 0.0f);
	if (m_body)
	{
		const ndVector f(m_body->GetOmega());
		tmp.X = f.m_x;
		tmp.Y = f.m_y;
		tmp.Z = f.m_z;
	}
	return tmp;
}

void UNewtonRigidBody::SetOmega(const FVector& omega) const
{
	if (m_body)
	{
		const ndVector w(ndFloat32(omega.X), ndFloat32(omega.Y), ndFloat32(omega.Z), ndFloat32(0.0f));
		m_body->SetOmega(w);
	}
}

FVector UNewtonRigidBody::GetForce() const
{
	FVector tmp(0.0f, 0.0f, 0.0f);
	if (m_body)
	{
		const ndVector f(m_body->GetForce().Scale(UNREAL_UNIT_SYSTEM));
		tmp.X = f.m_x;
		tmp.Y = f.m_y;
		tmp.Z = f.m_z;
	}
	return tmp;
}

FVector UNewtonRigidBody::GetTorque() const
{
	FVector tmp(0.0f, 0.0f, 0.0f);
	if (m_body)
	{
		const ndVector f(m_body->GetTorque().Scale(UNREAL_UNIT_SYSTEM * UNREAL_UNIT_SYSTEM));
		tmp.X = f.m_x;
		tmp.Y = f.m_y;
		tmp.Z = f.m_z;
	}
	return tmp;
}

void UNewtonRigidBody::SetForce(const FVector& force)
{
	if (m_body)
	{
		const ndVector f(ndFloat32(force.X), ndFloat32(force.Y), ndFloat32(force.Z), ndFloat32(0.0f));
		const ndVector tmp(f.Scale(UNREAL_INV_UNIT_SYSTEM));
		m_body->SetForce(tmp);
	}
}

void UNewtonRigidBody::SetTorque(const FVector& torque)
{
	if (m_body)
	{
		const ndVector t(ndFloat32(torque.X), ndFloat32(torque.Y), ndFloat32(torque.Z), ndFloat32(0.0f));
		const ndVector tmp(t.Scale(UNREAL_INV_UNIT_SYSTEM * UNREAL_INV_UNIT_SYSTEM));
		m_body->SetTorque(tmp);

		//ndVector omega(m_body->GetOmega());
		//UE_LOG(LogTemp, Display, TEXT("w(%f %f %f) T(%f %f %f)"), omega.m_x, omega.m_y, omega.m_z, tmp.m_x, tmp.m_y, tmp.m_z);
	}
}
