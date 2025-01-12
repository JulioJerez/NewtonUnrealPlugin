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
#include "GameFramework/Actor.h"
#include "NewtonWorldActor.generated.h"

class ndWorld;
class NewtonWorld;
class UDynamicMesh;
class UNewtonRigidBody;

UENUM()
enum class SolverModeTypes : uint8
{
	scalar,
	soaSimd,
};


USTRUCT(BlueprintType)
struct FNewtonRaycastResult
{
	GENERATED_BODY()

	FNewtonRaycastResult()
	{
	}

	/** PrimitiveComponent hit by the trace. */
	UPROPERTY(BlueprintReadOnly, Category = "Newton")
	TWeakObjectPtr<UNewtonRigidBody> HitBody;

	UPROPERTY(BlueprintReadOnly, Category = "Newton")
	FVector HitPosit;

	UPROPERTY(BlueprintReadOnly, Category = "Newton")
	FVector HitNormal;

	UPROPERTY(BlueprintReadOnly, Category = "Newton")
	float HitParam;
};

UCLASS( ClassGroup = NewtonActors, meta=(BlueprintSpawnableComponent) )
class NEWTONRUNTIMEMODULE_API ANewtonWorldActor : public AActor
{
	GENERATED_BODY()

	public:	
	// Sets default values for this actor's properties
	ANewtonWorldActor();

	ndWorld* GetNewtonWorld() const;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	void ApplySettings();
	float GetSimTime() const;
	void Update(float timestep);

	UFUNCTION(BlueprintCallable, Category = "Newton")
	bool RayCast(struct FNewtonRaycastResult& result, const FVector& origin, const FVector& target, bool filterStatic) const;

	UPROPERTY(EditAnywhere, Category=Newton)
	float UpdateRate;

	UPROPERTY(EditAnywhere, Category=Newton, meta = (ClampMin = 2, ClampMax = 8))
	int SolverPasses;

	UPROPERTY(EditAnywhere, Category=Newton, meta = (ClampMin = 4, ClampMax = 32))
	int SolverIterations;

	UPROPERTY(EditAnywhere, Category=Newton, meta = (ClampMin = 1, ClampMax = 32))
	int ParallelThreads;

	UPROPERTY(EditAnywhere, Category = Newton, meta=(ValidEnumValues="scalar, soaSimd"))
	SolverModeTypes SolverMode;

	UPROPERTY(EditAnywhere, Category = Newton)
	bool AutoSleepMode;

	UPROPERTY(EditAnywhere, Category=Newton)
	bool ClearDebug;

	UPROPERTY(EditAnywhere, Category=Newton)
	bool ShowDebug;

	private:
	void Cleanup();
	void StartGame();

	NewtonWorld* m_world;
	bool m_beginPlay;

	friend class FNewtonRuntimeModule;
	friend class UNewtonCollision;
};
