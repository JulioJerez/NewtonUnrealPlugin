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

#include "NewtonWorldActor.h"
#include "EngineUtils.h"
#include "LevelEditor.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/IPluginManager.h"

#include "NewtonWorld.h"
#include "NewtonRuntimeModule.h"
#include "NewtonJoint.h"
#include "NewtonRigidBody.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

// Sets default values
ANewtonWorldActor::ANewtonWorldActor()
	:m_world(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	m_beginPlay = false;
	SolverPasses = 2;
	UpdateRate = 60.0f;
	ParallelThreads = 1;
	SolverIterations = 4;

	ShowDebug = false;
	ClearDebug = false;
	
	AutoSleepMode = true;
	SolverMode = SolverModeTypes::scalar;
}

ndWorld* ANewtonWorldActor::GetNewtonWorld() const
{
	return m_world ? m_world->GetNewtonWorld() : nullptr;
}

// Called when the game starts or when spawned
void ANewtonWorldActor::BeginPlay()
{
	Super::BeginPlay();
	m_beginPlay = true;
	if (!m_world)
	{
		m_world = new NewtonWorld(this);
	}
	ApplySettings();
	m_world->Sync();
}

void ANewtonWorldActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (m_world)
	{
		Cleanup();
		delete m_world;
		//switch (EndPlayReason)
		//{
		//	case EEndPlayReason::Destroyed:
		//		ndAssert(0);
		//		break;
		//	case EEndPlayReason::LevelTransition:
		//		ndAssert(0);
		//		break;
		//	case EEndPlayReason::EndPlayInEditor:
		//		ndAssert(0);
		//		break;
		//	case EEndPlayReason::RemovedFromWorld:
		//		ndAssert(0);
		//		break;
		//	case EEndPlayReason::Quit:
		//		m_world->Sync();
		//		delete m_world;
		//		break;
		//}
	}
}

void ANewtonWorldActor::StartGame()
{
	m_beginPlay = false;
	if (m_world)
	{
		m_world->StartGame();
	}
}

void ANewtonWorldActor::Cleanup()
{
	m_world->Sync();
	UWorld* const world = GetWorld();

	// remove all joints body actors.
	for (TActorIterator<AActor> actorItr(world); actorItr; ++actorItr)
	{
		AActor* const actor = *actorItr;
		if (actor->FindComponentByClass(UNewtonRigidBody::StaticClass()))
		{
			const TSet<UActorComponent*>& components = actor->GetComponents();
			for (TSet<UActorComponent*>::TConstIterator it(components.CreateConstIterator()); it; ++it)
			{
				UNewtonJoint* const joint = Cast<UNewtonJoint>(*it);
				if (joint)
				{
					joint->DestroyJoint();
				}
			}
		}
	}

	// remove all rigid body actors.
	for (TActorIterator<AActor> actorItr(world); actorItr; ++actorItr)
	{
		AActor* const actor = *actorItr;
		if (actor->FindComponentByClass(UNewtonRigidBody::StaticClass()))
		{
			const TSet<UActorComponent*>& components = actor->GetComponents();
			for (TSet<UActorComponent*>::TConstIterator it(components.CreateConstIterator()); it; ++it)
			{
				UNewtonRigidBody* const meshComp = Cast<UNewtonRigidBody>(*it);
				if (meshComp)
				{
					meshComp->DestroyRigidBody();
				}
			}
		}
	}
}

void ANewtonWorldActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	AActor::PostEditChangeProperty(PropertyChangedEvent);
	if (m_world)
	{
		ApplySettings();
	}

	bool clearOption = PropertyChangedEvent.GetPropertyName() == "ClearDebug";
	if (clearOption || (PropertyChangedEvent.GetPropertyName() == "ShowDebug"))
	{
		const UWorld* const world = GetWorld();
		for (TActorIterator<AActor> actorItr(world); actorItr; ++actorItr)
		{
			AActor* const actor = *actorItr;
			const TSet<UActorComponent*>& components = actor->GetComponents();
			for (TSet<UActorComponent*>::TConstIterator it(components.CreateConstIterator()); it; ++it)
			{
				UNewtonRigidBody* const body = Cast<UNewtonRigidBody>(*it);
				if (body)
				{
					clearOption ? body->ClearDebug() : body->ActivateDebug();
				}

				UNewtonJoint* const joint = Cast<UNewtonJoint>(*it);
				if (joint)
				{
					clearOption ? joint->ClearDebug() : joint->ActivateDebug();
				}
			}
		}

		ShowDebug = false;
		ClearDebug = false;
		FLevelEditorModule& levelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		levelEditor.BroadcastComponentsEdited();
		levelEditor.BroadcastRedrawViewports(false);
	}
}

float ANewtonWorldActor::GetSimTime() const
{
	return float (m_world ? m_world->GetAverageUpdateTime(): 0.0f);
}

void ANewtonWorldActor::ApplySettings()
{
	if (m_world)
	{
		m_world->ApplySettings();
	}
}

// surrogate tick function
void ANewtonWorldActor::Update(float timestep)
{
	if (m_world)
	{
		m_world->Update(timestep);
	}
}

bool ANewtonWorldActor::RayCast(struct FNewtonRaycastResult& result, const FVector& origin, const FVector& target, bool filterStatic) const
{
	class ndRayPickingCallback : public ndRayCastClosestHitCallback
	{
		public:
		ndRayPickingCallback()
			:ndRayCastClosestHitCallback()
		{
		}

		ndFloat32 OnRayCastAction(const ndContactPoint& contact, ndFloat32 intersetParam)
		{
			if (contact.m_body0->GetInvMass() == ndFloat32(0.0f))
			{
				return 1.2f;
			}
			return ndRayCastClosestHitCallback::OnRayCastAction(contact, intersetParam);
		}
	};

	const ndVector start(ndFloat32 (origin.X * UNREAL_INV_UNIT_SYSTEM), ndFloat32(origin.Y * UNREAL_INV_UNIT_SYSTEM), ndFloat32(origin.Z * UNREAL_INV_UNIT_SYSTEM), ndFloat32(1.0f));
	const ndVector end(ndFloat32(target.X * UNREAL_INV_UNIT_SYSTEM), ndFloat32(target.Y * UNREAL_INV_UNIT_SYSTEM), ndFloat32(target.Z * UNREAL_INV_UNIT_SYSTEM), ndFloat32(1.0f));

	ndRayPickingCallback rayCaster;

	const ndWorld* const newtonWorld = m_world->GetNewtonWorld();
	if (newtonWorld->RayCast(rayCaster, start, end))
	{
		const ndBodyDynamic* const body = ((ndBody*)rayCaster.m_contact.m_body0)->GetAsBodyDynamic();
		check(body);
		const ndBodyNotify* const notify = body->GetNotifyCallback();
		check(notify);

		const ndVector normal(rayCaster.m_contact.m_normal);
		const ndVector contact(rayCaster.m_contact.m_point.Scale(UNREAL_UNIT_SYSTEM));

		result.HitParam = rayCaster.m_param;
		result.HitBody = (UNewtonRigidBody*)notify->GetUserData();
		result.HitNormal = FVector(normal.m_x, normal.m_y, normal.m_z);
		result.HitPosit = FVector(contact.m_x, contact.m_y, contact.m_z);
		return true;
	}
	return false;
}

