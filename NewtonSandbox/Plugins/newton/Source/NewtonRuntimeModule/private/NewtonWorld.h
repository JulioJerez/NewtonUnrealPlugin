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

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "atomic"

class ndWorld;
class ANewtonWorldActor;

#define D_USING_UNREAL_TRHEADS

#ifndef D_USING_UNREAL_TRHEADS

class NewtonWorld
{
	class PhysicsEngine;
	public:
	NewtonWorld(ANewtonWorldActor* const owner);
	virtual ~NewtonWorld();

	void Sync();
	void ApplySettings() const;
	ndWorld* GetNewtonWorld() const;
	float GetAverageUpdateTime() const;

	void StartGame();
	void Update(float timestep);

	private:
	void VisualTick();
	void PhysicsTick();

	PhysicsEngine* m_world;
	ANewtonWorldActor* m_owner;
	float m_timeAccumulator;
};

#else

class NewtonWorld: public FRunnable
{
	class PhysicsEngine;
	public:
	class EngineMainThread;

	NewtonWorld(ANewtonWorldActor* const owner);
	~NewtonWorld();

	virtual bool Init() override;
	virtual void Stop() override;
	virtual void Exit() override;
	virtual uint32 Run() override;

	void Sync();
	void ApplySettings() const;
	ndWorld* GetNewtonWorld() const;
	float GetAverageUpdateTime() const;

	void StartGame();
	void Update(float timestep);

	private:
	void Intepolate() const;
	bool IsTerminated() const;

	PhysicsEngine* m_world;
	ANewtonWorldActor* m_owner;
	FRunnableThread* m_worker;
	std::atomic<float> m_timeAcc;
	std::atomic<bool> m_active;
	std::atomic<bool> m_terminated;
	bool m_initialized;
};

#endif