// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/DynamicMeshComponent.h"
#include "NewtonCollision.generated.h"

class ndShape;
class ndVector;
class ndMatrix;
class ndShapeInstance;
class UNewtonRigidBody;

template <typename T>
class ndSharedPtr;

//UCLASS(ClassGroup = NewtonCollision, meta=(BlueprintSpawnableComponent), HideCategories = (RayTracing, Mobile, TextureStreaming, VirtualTexture, MaterialParameters, DynamicMeshComponent, Advanced, Activation, Collision, Lighting, BodySetup, Primitives, HLOD, ComponentTick, Rendering, Physics, Tags, Replication, ComponentReplication, Cooking, Events, LOD, Navigation, AssetUserData))
UCLASS(Abstract, meta=(BlueprintSpawnableComponent), HideCategories = (RayTracing, Mobile, TextureStreaming, VirtualTexture, MaterialParameters, DynamicMeshComponent, Advanced, Activation, Collision, Lighting, BodySetup, Primitives, HLOD, ComponentTick, Rendering, Physics, Tags, Replication, ComponentReplication, Cooking, Events, LOD, Navigation, AssetUserData))
class NEWTONRUNTIMEMODULE_API UNewtonCollision : public UDynamicMeshComponent
{
	GENERATED_BODY()
	class PolygonizeMesh;

	public:	
	// Sets default values for this component's properties
	UNewtonCollision();
	virtual void SetWireFrameColor(const FLinearColor& color);
	virtual ndVector GetVolumePosition(const ndMatrix& bodyMatrix) const;
	virtual void InitStaticMeshCompoment(const USceneComponent* const meshComponent);

	protected:
	UPROPERTY(EditAnywhere, Category = Newton)
	bool BestFit;

	// Called when the game starts
	virtual void OnRegister();
	virtual void OnUnregister();
	virtual void PostLoad() override;
	virtual void OnAttachmentChanged();
	virtual void BeginDestroy() override;
	virtual bool ShouldCreatePhysicsState() const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void BuildNewtonShape();
	virtual void ApplyPropertyChanges();

	bool GetDebugMode() const;
	virtual ndShape* CreateShape() const;
	virtual long long CalculateHash() const;
	virtual ndShapeInstance* CreateInstanceShape() const;
	virtual UNewtonRigidBody* GetRigidBodyParent() const;
	virtual void SetTransform(const USceneComponent* const meshComponent);
	virtual ndShapeInstance* CreateBodyInstanceShape(const ndMatrix& bodyMatrix) const;

	//public:	
	long long m_hash;
	ndShape* m_shape;
	TObjectPtr<UMaterial> m_debugMaterial;
	TSharedPtr<UE::Geometry::FDynamicMesh3> m_visualMesh;
	bool m_propertyChanged;
	bool m_showDebug;
	bool m_debugVisualIsDirty;
	friend class UNewtonRigidBody;
	friend class FNewtonRuntimeModule;
	friend class UNewtonSceneRigidBody;
};
