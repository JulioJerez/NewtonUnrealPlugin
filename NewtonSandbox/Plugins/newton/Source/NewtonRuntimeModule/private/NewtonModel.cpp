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

#include "NewtonModel.h"
#include "GameFramework/Actor.h"

#include "NewtonAsset.h"
#include "NewtonJoint.h"
#include "NewtonJointLoop.h"
#include "NewtonRigidBody.h"
#include "NewtonWorldActor.h"
#include "NewtonJointVehicleMotor.h"
#include "NewtonModelBlueprintBuilder.h"
#include "NewtonJointVehicleDifferential.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

class UNewtonModel::ModelNotify : public ndModelNotify
{
	public:
	ModelNotify(UNewtonModel* const owner, ndModelArticulation* const model)
		:ndModelNotify()
	{
		SetModel(model);
		m_owner = owner;
		AActor* const actorOwner = m_owner->GetOwner();
		FString onModelUpdate(ND_MODEL_UPDATE_EVENT_NAME);
		m_onModelUpdate = actorOwner->FindFunction(FName(*onModelUpdate));
	}

	void Debug(ndConstraintDebugCallback& context) const override
	{
		ndModelNotify::Debug(context);
	}

	void Update(ndWorld* const world, ndFloat32 timestep) override
	{
		ndModelNotify::Update(world, timestep);
		if (m_onModelUpdate)
		{
			AActor* const actorOwner = m_owner->GetOwner();
			actorOwner->ProcessEvent(m_onModelUpdate, nullptr);
		}
	}

	void PostUpdate(ndWorld* const world, ndFloat32 timestep) override
	{
		ndModelNotify::PostUpdate(world, timestep);
	}

	void PostTransformUpdate(ndWorld* const world, ndFloat32 timestep) override
	{
		ndModelNotify::PostTransformUpdate(world, timestep);
	}

	UNewtonModel* m_owner;
	UFunction* m_onModelUpdate;
};

class ndModelVehicleNotify : public UNewtonModel::ModelNotify
{
	public:
	ndModelVehicleNotify(UNewtonModel* const owner, ndMultiBodyVehicle* const vehicle)
		:UNewtonModel::ModelNotify(owner, vehicle)
	{
		m_sleepingState = false;

		check(vehicle->GetRoot());
		vehicle->AddChassis(vehicle->GetRoot()->m_body);

		auto FindJointComponent = [owner](const ndJointBilateralConstraint* const joint)
		{
			for (TSet<UActorComponent*>::TConstIterator it(owner->GetOwner()->GetComponents().CreateConstIterator()); it; ++it)
			{
				const UNewtonJoint* const jointComponent = Cast<UNewtonJoint>(*it);
				if (jointComponent && (jointComponent->m_joint == joint))
				{
					return jointComponent;
				}
			}
			return (const UNewtonJoint*)nullptr;
		};

		// add all tires, differentials and motor
		for (ndMultiBodyVehicle::ndNode* node = vehicle->GetRoot()->GetFirstIterator(); node; node = node->GetNextIterator())
		{
			if (node->m_joint)
			{
				const char* const className = node->m_joint->ClassName();
				//if (!strcmp(className, "ndMultiBodyVehicleTireJoint"))
				if (!strcmp(className, ndMultiBodyVehicleTireJoint::StaticClassName()))
				{
					ndMultiBodyVehicleTireJoint* const joint = (ndMultiBodyVehicleTireJoint*)*node->m_joint;
					//ndMultiBodyVehicleTireJointInfo info(tire->GetInfo());
					joint->SetVehicleOwner(vehicle);
					ndBodyKinematic* const body = node->m_body->GetAsBodyKinematic();

					// make intetia spherical
					ndVector massMatrix(body->GetMassMatrix());
					ndFloat32 maxInertia = ndMax(ndMax(massMatrix.m_x, massMatrix.m_y), massMatrix.m_z);
					massMatrix.m_x = maxInertia;
					massMatrix.m_y = maxInertia;
					massMatrix.m_z = maxInertia;
					body->SetMassMatrix(massMatrix);

					vehicle->AddTire(node->m_body, node->m_joint);
				}
				//else if (!strcmp(className, "ndMultiBodyVehicleMotor"))
				else if (!strcmp(className, ndMultiBodyVehicleMotor::StaticClassName()))
				{
					ndMultiBodyVehicleMotor* const joint = (ndMultiBodyVehicleMotor*)*node->m_joint;
					joint->SetVehicleOwner(vehicle);
					
					const UNewtonJointVehicleMotor* const componentOwner = Cast<UNewtonJointVehicleMotor>(FindJointComponent(joint));
					if (componentOwner)
					{
						// set the procedural shape
						ndBodyKinematic* const motorBody = joint->GetBody0();
						const ndMatrix matrix(vehicle->GetLocalFrame() * motorBody->GetMatrix());
						motorBody->SetMatrix(matrix);
					
						ndShapeInstance motorCollision(new ndShapeSphere(componentOwner->BodyRadio * UNREAL_INV_UNIT_SYSTEM));
						motorCollision.SetCollisionMode(false);
						motorBody->SetCollisionShape(motorCollision);
						motorBody->SetMassMatrix(componentOwner->BodyMass, motorCollision);
					
						ndMatrix localMatrix0;
						ndMatrix localMatrix1;
						node->m_joint->CalculateLocalMatrix(matrix, localMatrix0, localMatrix1);
						node->m_joint->SetLocalMatrix0(localMatrix0);
						node->m_joint->SetLocalMatrix1(localMatrix1);
					
						vehicle->AddMotor(node->m_body, node->m_joint);
					}
				}
				//else if (!strcmp(className, "ndMultiBodyVehicleDifferential"))
				else if (!strcmp(className, ndMultiBodyVehicleDifferential::StaticClassName()))
				{
					ndMultiBodyVehicleDifferential* const joint = (ndMultiBodyVehicleDifferential*)*node->m_joint;
					const UNewtonJointVehicleDifferential* const componentOwner = Cast<UNewtonJointVehicleDifferential>(FindJointComponent(joint));
					ndAssert(componentOwner);
					if (componentOwner)
					{
						// set the procedural shape
						ndBodyKinematic* const differentialBody = joint->GetBody0();
						const ndMatrix matrix(vehicle->GetLocalFrame() * differentialBody->GetMatrix());
						differentialBody->SetMatrix(matrix);
					
						ndShapeInstance diffCollision(new ndShapeSphere(componentOwner->BodyRadio * UNREAL_INV_UNIT_SYSTEM));
						diffCollision.SetCollisionMode(false);
						differentialBody->SetCollisionShape(diffCollision);
						differentialBody->SetMassMatrix(componentOwner->BodyMass, diffCollision);
					
						ndMatrix localMatrix0;
						ndMatrix localMatrix1;
						node->m_joint->CalculateLocalMatrix(matrix, localMatrix0, localMatrix1);
						node->m_joint->SetLocalMatrix0(localMatrix0);
						node->m_joint->SetLocalMatrix1(localMatrix1);
						vehicle->AddDifferential(node->m_body, node->m_joint);
					}
				}
			}
		}

		// add all loops, axles and gear box
		const ndList<ndModelArticulation::ndNode>& loops = vehicle->GetCloseLoops();
		for (ndList<ndModelArticulation::ndNode>::ndNode* node = loops.GetFirst(); node; node = node->GetNext())
		{
			ndModelArticulation::ndNode& vehicleNode = node->GetInfo();
			ndJointBilateralConstraint* const joint = *vehicleNode.m_joint;
			const char* const className = joint->ClassName();
			if (!strcmp(className, "ndMultiBodyVehicleDifferentialAxle"))
			{
				vehicle->AddDifferentialAxle(vehicleNode.m_joint);
			}
			else if (!strcmp(className, "ndMultiBodyVehicleGearBox"))
			{
				vehicle->AddGearBox(vehicleNode.m_joint);
			}
			else
			{
				check(0);
			}
		}

		vehicle->SetVehicleSolverModel(false);
	}

	void Debug(ndConstraintDebugCallback& context) const override
	{
		UNewtonModel::ModelNotify::Debug(context);
	}

	void Update(ndWorld* const world, ndFloat32 timestep) override
	{
		UNewtonModel::ModelNotify::Update(world, timestep);

		ndMultiBodyVehicle* const vehicle = (ndMultiBodyVehicle*)GetModel();
		m_sleepingState = true;
		if (vehicle && !vehicle->IsSleeping())
		{
			m_sleepingState = false;
			//ApplyInputs(world, timestep);
			vehicle->Update(world, timestep);
		}
		ndModelNotify::Update(world, timestep);
	}

	void PostUpdate(ndWorld* const world, ndFloat32 timestep) override
	{
		UNewtonModel::ModelNotify::PostUpdate(world, timestep);
		ndModelNotify::PostUpdate(world, timestep);
		ndMultiBodyVehicle* const vehicle = (ndMultiBodyVehicle*)GetModel();
		if (vehicle && !m_sleepingState)
		{
			vehicle->PostUpdate(world, timestep);
		}
	}

	void PostTransformUpdate(ndWorld* const world, ndFloat32 timestep) override
	{
		UNewtonModel::ModelNotify::PostTransformUpdate(world, timestep);
	}

	bool m_sleepingState;
};

UNewtonModel::UNewtonModel()
	:Super()
{
	ShowDebug = true;
	m_model = nullptr;
	NewtonAsset = nullptr;
	RegenerateBluePrint = false;
	PrimaryComponentTick.bCanEverTick = true;
}

void UNewtonModel::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* tickFunction)
{
	Super::TickComponent(deltaTime, tickType, tickFunction);
}

ndModelArticulation* UNewtonModel::CreateSubClassModel() const
{
	UNewtonAsset* const asset = NewtonAsset;
	if (asset)
	{
		switch (asset->m_modelType)
		{
			case ModelsType::m_vehicleModel:
			{
				ndMatrix localFrame(ndGetIdentityMatrix());
				localFrame.m_up = ndVector(0.0f, 0.0f, 1.0f, 0.0f);
				localFrame.m_right = localFrame.m_front.CrossProduct(localFrame.m_up);
				ndMultiBodyVehicle* const vehicle = new ndMultiBodyVehicle();
				vehicle->SetLocalFrame(localFrame);

				return vehicle;
			}
			case ModelsType::m_baseModel:
			default:
				return new ndModelArticulation();
		}
	}

	return new ndModelArticulation();
}

ndModelArticulation* UNewtonModel::CreateModel(ANewtonWorldActor* const worldActor)
{
	check(!m_model);
	AActor* const owner = GetOwner();
	ndFixSizeArray<UNewtonJoint*, 512> loops;
	ndFixSizeArray<USceneComponent*, ND_STACK_DEPTH> stack;
	ndFixSizeArray<ndModelArticulation::ndNode*, ND_STACK_DEPTH> parentStack;

	ndModelArticulation* const articulatedModel = CreateSubClassModel();
	m_model = articulatedModel;

	stack.PushBack(owner->GetRootComponent());
	parentStack.PushBack(nullptr);
	while (stack.GetCount())
	{
		USceneComponent* const component = stack.Pop();
		ndModelArticulation::ndNode* parent = parentStack.Pop();

		UNewtonRigidBody* const bodyComponent = Cast<UNewtonRigidBody>(component);
		if (bodyComponent)
		{
			check(bodyComponent);
			ndBodyDynamic* const body = bodyComponent->CreateRigidBody(worldActor->AutoSleepMode);
			bodyComponent->m_body = body;
			bodyComponent->m_newtonWorld = worldActor;
			if (parent == nullptr)
			{
				parent = articulatedModel->AddRootBody(body);
			}
			else
			{
				UNewtonJoint* const jointComponent = Cast<UNewtonJoint>(component->GetAttachParent());
				check(jointComponent);
				ndJointBilateralConstraint* const joint = jointComponent->CreateJoint();
				check(joint);
				jointComponent->m_joint = joint;
				parent = articulatedModel->AddLimb(parent, body, joint);
			}
			check(parent);
		}
		else
		{
			UNewtonJointLoop* const loopComponent = Cast<UNewtonJointLoop>(component);
			if (loopComponent)
			{
				check (loopComponent->GetAttachChildren().Num() == 0)
				loops.PushBack(loopComponent);
			}
		}

		const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
		for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
		{
			parentStack.PushBack(parent);
			stack.PushBack(childrenComp[i].Get());
		}
	}

	for (ndInt32 i = loops.GetCount() - 1; i >= 0; --i)
	{
		UNewtonJoint* const loopComponent = loops[i];
		ndJointBilateralConstraint* const joint = loopComponent->CreateJoint();
		loopComponent->m_joint = joint;
		articulatedModel->AddCloseLoop(joint);
	}

	UNewtonAsset* const asset = NewtonAsset;
	if (asset)
	{
		switch (asset->m_modelType)
		{
			case ModelsType::m_vehicleModel:
			{
				check(articulatedModel->GetAsMultiBodyVehicle());
				ndSharedPtr<ndModelNotify> notify(new ndModelVehicleNotify(this, articulatedModel->GetAsMultiBodyVehicle()));
				articulatedModel->SetNotifyCallback(notify);

				ndWorld* world = worldActor->GetNewtonWorld();
				ndSharedPtr<ndJointBilateralConstraint> fixJoint(new ndJointFix6dof(articulatedModel->GetRoot()->m_body->GetMatrix(), articulatedModel->GetRoot()->m_body->GetAsBodyKinematic(), world->GetSentinelBody()));
				world->AddJoint(fixJoint);;
				//articulatedModel->GetRoot()->m_body->GetAsBodyDynamic()->SetMassMatrix(ndVector(0.0f, 0.0f, 0.0f, 0.0f));

				break;
			}

			case ModelsType::m_baseModel:
			default:
			{
				ndSharedPtr<ndModelNotify> notify(new ModelNotify(this, articulatedModel));
				articulatedModel->SetNotifyCallback(notify);
				break;
			}
		}
	}

	return articulatedModel;
}
