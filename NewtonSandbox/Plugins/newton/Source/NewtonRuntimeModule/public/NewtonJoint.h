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
#include "UObject/ObjectMacros.h"
#include "Components/SceneComponent.h"
#include "NewtonJoint.generated.h"

class ndWorld;
class ndBodyKinematic;
class UNewtonRigidBody;
class ANewtonWorldActor;
class ndJointBilateralConstraint;

//UCLASS(ClassGroup=(NewtonJoints), meta=(BlueprintSpawnableComponent), HideCategories = (RayTracing, Mobile, TextureStreaming, VirtualTexture, MaterialParameters, DynamicMeshComponent, Advanced, Activation, Collision, Lighting, BodySetup, Primitives, HLOD, ComponentTick, Rendering, Physics, Tags, Replication, ComponentReplication, Cooking, Events, LOD, Navigation, AssetUserData))
UCLASS(Abstract, meta=(BlueprintSpawnableComponent), HideCategories = (RayTracing, Mobile, TextureStreaming, VirtualTexture, MaterialParameters, DynamicMeshComponent, Advanced, Activation, Collision, Lighting, BodySetup, Primitives, HLOD, ComponentTick, Rendering, Physics, Tags, Replication, ComponentReplication, Cooking, Events, LOD, Navigation, AssetUserData))
class NEWTONRUNTIMEMODULE_API UNewtonJoint : public USceneComponent
{
	GENERATED_BODY()

	public:	
	// Sets default values for this component's properties
	UNewtonJoint();

	void ClearDebug();
	void ActivateDebug();

	// Called every frame
	virtual void TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* tickFunction) override;

	void DestroyJoint();
	void UpdateTransform();
	virtual void DrawGizmo(float timestep) const;
	virtual ndJointBilateralConstraint* CreateJoint() ;

	UPROPERTY(EditAnywhere, Category = Newton)
	bool ShowDebug;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.1f, ClampMax = 4.0f))
	float DebugScale;

	protected:
	UNewtonRigidBody* FindChild() const;
	UNewtonRigidBody* FindParent() const;
	void GetBodyPairs(ndBodyKinematic** body0, ndBodyKinematic** body1) const;

	void ApplyPropertyChanges();
	virtual void PostEditChangeProperty(FPropertyChangedEvent& propertyChangedEvent) override;

	FTransform m_transfrom;
	ndJointBilateralConstraint* m_joint;
	FTransform m_localChildTransfrom;
	FTransform m_localParentTransfrom;
	bool m_propertyChanged;
	
	friend class NewtonWorld;
	friend class UNewtonModel;
	friend class FNewtonRuntimeModule;
};
