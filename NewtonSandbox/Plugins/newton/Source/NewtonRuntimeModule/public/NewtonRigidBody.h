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
#include "Components/SceneComponent.h"
#include "NewtonRigidBody.generated.h"

class ndWorld;
class ndMatrix;
class ndBodyDynamic;
class ndShapeInstance;
class UNewtonCollision;
class ANewtonWorldActor;


USTRUCT()
struct FNewtonRigidBodyInertia
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, Category = Newton)
	bool ShowPrincipalAxis = false;

	UPROPERTY(VisibleAnywhere, Category = Newton)
	FVector PrincipalInertia = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = Newton)
	FVector PrincipalInertiaScaler = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, Category = Newton)
	FRotator PrincipalInertiaAxis = FRotator(0.0f, 0.0f, 0.0f);
};

/**
 * 
 */
UCLASS(ClassGroup = Newton, BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), HideCategories = (Physics, Collision))
class NEWTONRUNTIMEMODULE_API UNewtonRigidBody : public USceneComponent
{
	GENERATED_BODY()
	//GENERATED_UCLASS_BODY()
	class NotifyCallback;

	public:
	// Sets default values for this component's properties
	UNewtonRigidBody();
	void UpdateTransform();
	void CalculateLocalTransform();
	void InterpolateTransform(float param);
	void CreateRigidBody(ANewtonWorldActor* const worldActor, bool overrideAutoSleep);

	// **********************************
	// BluePrint interface
	// **********************************
	UFUNCTION(BlueprintCallable, Category="Newton")
	float GetTimeStep() const;

	UFUNCTION(BlueprintCallable, Category="Newton")
	float GetMass() const;

	UFUNCTION(BlueprintCallable, Category="Newton")
	FVector GetInertia() const;

	UFUNCTION(BlueprintCallable, Category="Newton")
	FVector GetFrontDir() const;

	UFUNCTION(BlueprintCallable, Category="Newton")
	FVector GetRightDir() const;

	UFUNCTION(BlueprintCallable, Category="Newton")
	FVector GetUpDir() const;

	UFUNCTION(BlueprintCallable, Category="Newton")
	FVector GetVelocity() const;

	UFUNCTION(BlueprintCallable, Category="Newton")
	void SetVelocity(const FVector& veloc) const;

	UFUNCTION(BlueprintCallable, Category="Newton")
	FVector GetOmega() const;

	UFUNCTION(BlueprintCallable, Category="Newton")
	void SetOmega(const FVector& omega) const;

	UFUNCTION(BlueprintCallable, Category="Newton")
	FVector GetForce() const;

	UFUNCTION(BlueprintCallable, Category="Newton")
	void SetForce(const FVector& force);
	
	UFUNCTION(BlueprintCallable)
	FVector GetTorque() const;
	
	UFUNCTION(BlueprintCallable)
	void SetTorque(const FVector& torque);

	protected:
	// Called every frame
	virtual void PostLoad() override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginDestroy() override;
	virtual void OnChildAttached(USceneComponent* ChildComponent);
	virtual void OnChildDetached(USceneComponent* ChildComponent);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	void DestroyRigidBody();
	
	void DrawGizmo(float timestep);

	virtual void ClearDebug();
	virtual void ActivateDebug();
	virtual void ApplyPropertyChanges();

	virtual ndShapeInstance* CreateCollision(const ndMatrix& bodyMatrix) const;

	UPROPERTY(EditAnywhere, Category = Newton)
	bool ShowDebug;

	UPROPERTY(EditAnywhere, Category = Newton)
	bool ShowCenterOfMass;

	UPROPERTY(EditAnywhere, Category = Newton)
	bool SelfSkeletonCollidable;

	UPROPERTY(EditAnywhere, Category = Newton)
	bool AutoSleepMode;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.1f, ClampMax = 4.0f))
	float DebugScale;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f))
	float Mass;

	UPROPERTY(EditAnywhere, Category = Newton)
	FNewtonRigidBodyInertia Inertia;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float LinearDamp;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float AngularDamp;

	UPROPERTY(EditAnywhere, Category = Newton)
	FVector InitialVeloc;

	UPROPERTY(EditAnywhere, Category = Newton)
	FVector InitialOmega;

	UPROPERTY(EditAnywhere, Category = Newton)
	FVector CenterOfMass;

	UPROPERTY(EditAnywhere, Category = Newton)
	FVector Gravity;

	FVector m_localScale;
	FVector m_globalScale;
	FTransform m_localTransform;
	FTransform m_globalTransform;
	ndBodyDynamic* m_body;
	ANewtonWorldActor* m_newtonWorld;
	bool m_sleeping;
	bool m_propertyChanged;

	static FLinearColor m_awakeColor;
	static FLinearColor m_sleepingColor;

	friend class UNewtonJoint;
	friend class UNewtonCollision;
	friend class ANewtonWorldActor;
	friend class FNewtonRuntimeModule;
	friend class UNewtonLinkRigidBody;
};
