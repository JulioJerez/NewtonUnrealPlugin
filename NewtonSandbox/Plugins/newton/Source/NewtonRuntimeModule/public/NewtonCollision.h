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
	//virtual ndVector GetVolumePosition(const ndMatrix& bodyMatrix) const;
	virtual FVector4 GetVolumePosition(const ndMatrix& bodyMatrix) const;
	virtual void SetTransform(const USceneComponent* const meshComponent);
	virtual ndShapeInstance* CreateBodyInstanceShape(const ndMatrix& bodyMatrix) const;
	//virtual void GetVolumePosition(const ndMatrix& bodyMatrix, ndVector& positVolume) const;

	//public:	
	long long m_hash;
	ndShape* m_shape;
	TObjectPtr<UMaterial> m_debugMaterial;
	TSharedPtr<UE::Geometry::FDynamicMesh3> m_visualMesh;

	bool m_showDebug;
	bool m_propertyChanged;
	bool m_debugVisualIsDirty;
	friend class UNewtonRigidBody;
	friend class FNewtonRuntimeModule;
	friend class UNewtonSceneRigidBody;
};
