// Fill out your copyright notice in the Description page of Project Settings.

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
struct FInertiaStruct
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
UCLASS(ClassGroup = Newton, meta=(BlueprintSpawnableComponent), HideCategories = (Physics, Collision), MinimalAPI)
class UNewtonRigidBody : public USceneComponent
{
	GENERATED_BODY()
	class NotifyCallback;

	public:
	// Sets default values for this component's properties
	UNewtonRigidBody();
	void UpdateTransform();
	void CalculateLocalTransform();
	void InterpolateTransform(float param);
	void CreateRigidBody(ANewtonWorldActor* const worldActor, bool overrideAutoSleep);

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
	bool AutoSleepMode;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.1f, ClampMax = 4.0f))
	float DebugScale;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f))
	float Mass;

	UPROPERTY(EditAnywhere, Category = Newton)
	FInertiaStruct Inertia;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float LinearDamp;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float AngularDamp;

	UPROPERTY(EditAnywhere, Category = Newton)
	//UPROPERTY(EditAnywhere, Category = Newton, ReplicatedUsing=OnRep_Transform)
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
	friend class FnewtonModule;
	friend class UNewtonCollision;
	friend class ANewtonWorldActor;
};
