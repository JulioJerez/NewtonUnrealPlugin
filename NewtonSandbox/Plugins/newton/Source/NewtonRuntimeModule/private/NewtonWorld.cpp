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

#include "NewtonWorld.h"
#include "EngineUtils.h"

#include "NewtonJoint.h"
#include "NewtonModel.h"
#include "NewtonCommons.h"
#include "NewtonRigidBody.h"
#include "NewtonWorldActor.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

class NewtonWorld::PhysicsEngine : public ndWorld
{
	public:
	PhysicsEngine()
		:ndWorld()
	{
		m_initiladed = false;
	}

	void MainUpdate() override
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(newton);
		ndWorld::MainUpdate();
	}

	void WorkerUpdate(ndInt32 threadIndex) override
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(newtonThread);
		ndWorld::WorkerUpdate(threadIndex);
	}

	void PreUpdate(ndFloat32 timestep) override
	{
		ndWorld::PreUpdate(timestep);
		if (!m_initiladed)
		{
			m_initiladed = true;
			check(ValidateScene());
		}
		check(ValidateScene());
	}

	void UpdateTransforms() override
	{
		ndWorld::UpdateTransforms();
	
		const ndArray<ndBodyKinematic*>& bodyList = GetBodyList().GetView();
		for (ndInt32 i = bodyList.GetCount() - 1; i >= 0; --i)
		{
			ndBodyKinematic* const body = bodyList[i];
			ndBodyNotify* const notify = body->GetNotifyCallback();
			if (notify)
			{
				UNewtonRigidBody* const meshComp = Cast<UNewtonRigidBody>((USceneComponent*)notify->GetUserData());
				if (meshComp)
				{
					meshComp->UpdateTransform();
				}
			}
		}
	}

	bool m_initiladed;
};

#ifndef D_USING_UNREAL_TRHEADS

NewtonWorld::NewtonWorld(ANewtonWorldActor* const owner)
	:m_world(nullptr)
	,m_owner(owner)
	,m_timeAccumulator(0.0f)
{
	m_world = new PhysicsEngine;
}

NewtonWorld::~NewtonWorld()
{
	if (m_world)
	{
		delete m_world;
	}
}

ndWorld* NewtonWorld::GetNewtonWorld() const
{
	return m_world;
}

float NewtonWorld::GetAverageUpdateTime() const
{
	return m_world->GetAverageUpdateTime();
}

void NewtonWorld::Sync()
{
	m_world->Sync();
}

void NewtonWorld::ApplySettings() const
{
	m_world->Sync();
	ndWorld::ndSolverModes mode = ndWorld::ndStandardSolver;
	switch(m_owner->SolverMode)
	{
		case SolverModeTypes::Simd:
			mode = ndWorld::ndStandardSolver;
			break;
		case SolverModeTypes::SimdSoa:
			mode = ndWorld::ndSimdSoaSolver;
			break;
		case SolverModeTypes::SimdSoaAvx:
			mode = ndWorld::ndSimdAvx2Solver;
			break;
	};
	
	m_world->SelectSolver(mode);
	m_world->SetSubSteps(m_owner->SolverPasses);
	m_world->SetThreadCount(m_owner->ParallelThreads);
	m_world->SetSolverIterations(m_owner->SolverIterations);
	
	const ndBodyListView& bodyList = m_world->GetBodyList();
	for (ndBodyListView::ndNode* node = bodyList.GetFirst(); node; node = node->GetNext())
	{
		ndBodyKinematic* const body = node->GetInfo()->GetAsBodyKinematic();
		body->SetAutoSleep(m_owner->AutoSleepMode);
	}
}

void NewtonWorld::StartGame()
{
	m_world->Sync();
	
	UWorld* const world = m_owner->GetWorld();
	ndWorld* const newtonWorld = GetNewtonWorld();
	for (TActorIterator<AActor> actorItr(world); actorItr; ++actorItr)
	{
		AActor* const actor = *actorItr;
		UNewtonModel* const modelComponent = Cast<UNewtonModel>(actor->FindComponentByClass(UNewtonModel::StaticClass()));
		if (modelComponent)
		{
			ndModelArticulation* const model = modelComponent->CreateModel(m_owner);
			//modelComponent->m_model = model;
			newtonWorld->AddModel(model);
		}
		else
		{
			const TSet<UActorComponent*>& components = actor->GetComponents();
			for (TSet<UActorComponent*>::TConstIterator it(components.CreateConstIterator()); it; ++it)
			{
				UNewtonRigidBody* const component = Cast<UNewtonRigidBody>(*it);
				if (component)
				{
					ndBodyDynamic* const body = component->CreateRigidBody(m_owner->AutoSleepMode);
					component->m_body = body;
					component->m_newtonWorld = m_owner;
					newtonWorld->AddBody(body);
				}
			}
		}
	}

	for (TActorIterator<AActor> actorItr(world); actorItr; ++actorItr)
	{
		AActor* const actor = *actorItr;
		UNewtonModel* const modelComponent = Cast<UNewtonModel>(actor->FindComponentByClass(UNewtonModel::StaticClass()));
		if (!modelComponent)
		{
			const TSet<UActorComponent*>& components = actor->GetComponents();
			for (TSet<UActorComponent*>::TConstIterator it(components.CreateConstIterator()); it; ++it)
			{
				UNewtonJoint* const component = Cast<UNewtonJoint>(*it);
				if (component)
				{
					ndJointBilateralConstraint* const joint = component->CreateJoint();
					check(joint);
					component->m_joint = joint;
					newtonWorld->AddJoint(joint);
				}
			}
		}
	}
	
	m_timeAccumulator = 0.0f;
}

void NewtonWorld::Update(float timestep)
{
	m_timeAccumulator += timestep;
	PhysicsTick();
	VisualTick();
}

void NewtonWorld::VisualTick()
{
	const ndFloat32 descreteStep = 1.0f / m_owner->UpdateRate;
	ndFloat32 interpolationParam = ndClamp(m_timeAccumulator / descreteStep, ndFloat32(0.0f), ndFloat32(1.0f));
	
	UWorld* const world = m_owner->GetWorld();
	for (TActorIterator<AActor> actorItr(world); actorItr; ++actorItr)
	{
		AActor* const actor = *actorItr;
		if (actor->GetRootComponent())
		{
			ndFixSizeArray<USceneComponent*, ND_STACK_DEPTH> stack;
			stack.PushBack(actor->GetRootComponent());
			while (stack.GetCount())
			{
				USceneComponent* const component = stack.Pop();
				UNewtonRigidBody* const rigidBody = Cast<UNewtonRigidBody>(component);
				if (rigidBody)
				{
					rigidBody->InterpolateTransform(interpolationParam);
					rigidBody->CalculateLocalTransform();
				}
				UNewtonJoint* const joint = Cast<UNewtonJoint>(component);
				if (joint)
				{
					joint->UpdateTransform();
				}

				const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
				for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
				{
					stack.PushBack(childrenComp[i].Get());
				}
			}
		}
	}
}

void NewtonWorld::PhysicsTick()
{
	const ndFloat32 descreteStep = ndFloat32(1.0f) / m_owner->UpdateRate;
	
	if (m_timeAccumulator > descreteStep * ndFloat32(2.0f))
	{
		// truncate slow frame updates 
		m_timeAccumulator = m_timeAccumulator - descreteStep * ndFloor(m_timeAccumulator / descreteStep) + descreteStep;
	}
	
	UWorld* const world = m_owner->GetWorld();
	while (m_timeAccumulator > descreteStep)
	{
		m_world->Update(descreteStep);
		m_world->Sync();
		m_timeAccumulator -= descreteStep;
	
		//UE_LOG(LogTemp, Display, TEXT("loop time step %f(ms)  ticks %d"), ndFloat32(microSecondStep) * 1.0e-3f, thicks1 - thicks0);
	}
}

#else

NewtonWorld::NewtonWorld(ANewtonWorldActor* const owner)
	:FRunnable()
	,m_world(nullptr)
	,m_owner(owner)
	,m_worker(nullptr)
	,m_timeAcc(0.0f)
	,m_active(true)
	,m_terminated(false)
	,m_initialized(false)
{
	m_world = new PhysicsEngine;
	m_worker = FRunnableThread::Create(this, TEXT("newtonUnrealGlue"));
	check(m_worker);

	// block until thread is initialized.
	while (!m_initialized);
}

NewtonWorld::~NewtonWorld()
{
	if (m_world)
	{
		Stop();
		m_worker->WaitForCompletion();
		delete m_worker;
		delete m_world;
	}
}

ndWorld* NewtonWorld::GetNewtonWorld() const
{
	return m_world;
}

float NewtonWorld::GetAverageUpdateTime() const
{
	return m_world->GetAverageUpdateTime();
}

bool NewtonWorld::Init()
{
	return true;
}

bool NewtonWorld::IsTerminated() const
{
	return m_terminated.load();
}

void NewtonWorld::Stop()
{
	m_active.store(false);
	while (!IsTerminated());
}

void NewtonWorld::Exit()
{
	m_terminated.store(true);
}

uint32 NewtonWorld::Run()
{
	ndFloat32 updateRate = m_owner->UpdateRate;
	const ndFloat32 timestep = (1.0f / updateRate);

	ndInt32 thicks0 = 0;
	ndInt32 thicks1 = 0;
	m_initialized = true;
	ndUnsigned64 microSeconds0 = ndGetTimeInMicroseconds();
	ndUnsigned64 deltaMicroSeconds = ndUnsigned64(1.0e6f / updateRate);
	while (m_active.load())
	{
		ndUnsigned64 microSeconds1 = ndGetTimeInMicroseconds();
		ndUnsigned64 microSecondStep = microSeconds1 - microSeconds0;
		if (microSecondStep >= deltaMicroSeconds)
		{
			ndFloat32 timeAccumulator = m_timeAcc.load();
			ndFloat32 timeMark = ndFloat32(microSecondStep) * ndFloat32(1.0e-6f);
			if (timeAccumulator > timeMark * ndFloat32(2.0f))
			{
				// truncate slow frame updates 
				UE_LOG(LogTemp, Display, TEXT("physics fell behind, throwing out a time step"));
				timeAccumulator = timeAccumulator - timestep * ndFloor(timeAccumulator / timestep) + timestep;
			}

			m_world->Update(timestep);
			Sync();

			timeAccumulator -= timestep;
			m_timeAcc.store(timeAccumulator);

			microSeconds0 += deltaMicroSeconds;
			//UE_LOG(LogTemp, Display, TEXT("loop time step %f(ms)  ticks %d"), ndFloat32 (microSecondStep) * 1.0e-3f, thicks1 - thicks0);
			thicks0 = thicks1;
		}

		thicks1++;
		FPlatformProcess::YieldThread();
	}
	return 0;
}

void NewtonWorld::Sync()
{
	m_world->Sync();
}

void NewtonWorld::ApplySettings() const
{
	m_world->Sync();
	ndWorld::ndSolverModes mode = ndWorld::ndStandardSolver;
	switch (m_owner->SolverMode)
	{
		case SolverModeTypes::scalar:
			mode = ndWorld::ndStandardSolver;
			break;
		case SolverModeTypes::soaSimd:
			mode = ndWorld::ndSimdSoaSolver;
			break;
	};

	m_world->SelectSolver(mode);
	m_world->SetSubSteps(m_owner->SolverPasses);
	m_world->SetSolverIterations(m_owner->SolverIterations);
	if (m_world->GetThreadCount() != m_owner->ParallelThreads)
	{
		check(0);
		m_world->SetThreadCount(m_owner->ParallelThreads);
	}

	const ndBodyListView& bodyList = m_world->GetBodyList();
	for (ndBodyListView::ndNode* node = bodyList.GetFirst(); node; node = node->GetNext())
	{
		ndBodyKinematic* const body = node->GetInfo()->GetAsBodyKinematic();
		body->SetAutoSleep(m_owner->AutoSleepMode);
	}
}

void NewtonWorld::StartGame()
{
	m_world->Sync();

	UWorld* const world = m_owner->GetWorld();
	for (TActorIterator<AActor> actorItr(world); actorItr; ++actorItr)
	{
		AActor* const actor = *actorItr;
		const TSet<UActorComponent*>& components = actor->GetComponents();
		for (TSet<UActorComponent*>::TConstIterator it(components.CreateConstIterator()); it; ++it)
		{
			UNewtonRigidBody* const component = Cast<UNewtonRigidBody>(*it);
			if (component)
			{
				component->CreateRigidBody(m_owner, m_owner->AutoSleepMode);
			}
		}

		for (TSet<UActorComponent*>::TConstIterator it(components.CreateConstIterator()); it; ++it)
		{
			UNewtonJoint* const component = Cast<UNewtonJoint>(*it);
			if (component)
			{
				component->CreateJoint(m_owner);
			}
		}
	}

	m_timeAcc.store(0.0f);
}

void NewtonWorld::Intepolate() const
{
	ndFloat32 updateRate = m_owner->UpdateRate;
	ndFloat32 timeAccumulator = m_timeAcc.load();
	const ndFloat32 timestep = (1.0f / updateRate);
	ndFloat32 interpolationParam = ndClamp(timeAccumulator / timestep, ndFloat32(0.0f), ndFloat32(1.0f));

	UWorld* const world = m_owner->GetWorld();

	for (TActorIterator<AActor> actorItr(world); actorItr; ++actorItr)
	{
		AActor* const actor = *actorItr;
		if (actor->GetRootComponent())
		{
			ndFixSizeArray<USceneComponent*, ND_STACK_DEPTH> stack;
			stack.PushBack(actor->GetRootComponent());
			while (stack.GetCount())
			{
				USceneComponent* const component = stack.Pop();
				UNewtonRigidBody* const rigidBody = Cast<UNewtonRigidBody>(component);
				if (rigidBody)
				{
					rigidBody->InterpolateTransform(interpolationParam);
					rigidBody->CalculateLocalTransform();
				}
				UNewtonJoint* const joint = Cast<UNewtonJoint>(component);
				if (joint)
				{
					joint->UpdateTransform();
				}

				const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
				for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
				{
					stack.PushBack(childrenComp[i].Get());
				}
			}
		}
	}
}

void NewtonWorld::Update(float timestep)
{
	m_timeAcc.store(m_timeAcc.load() + timestep);
	Intepolate();
}

#endif