// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
//#include "PhysicsEngine/ShapeElem.h"
#include "Animation/DebugSkelMeshComponent.h"
#include "NewtonModelEditorSkeletalMeshComponent.generated.h"

//class FPrimitiveDrawInterface;
//class UMaterialInterface;
//class UMaterialInstanceDynamic;

class FNewtonModelEditor;

UCLASS()
class UNewtonModelEditorSkeletalMeshComponent : public UDebugSkelMeshComponent
{
	GENERATED_UCLASS_BODY()

#if 0
	/** Data and methods shared across multiple classes */


	// Draw colors

	FColor BoneUnselectedColor;
	FColor NoCollisionColor;
	FColor FixedColor;
	FColor ConstraintBone1Color;
	FColor ConstraintBone2Color;
	FColor HierarchyDrawColor;
	FColor AnimSkelDrawColor;
	float COMRenderSize;
	float InfluenceLineLength;
	FColor InfluenceLineColor;

	// Materials

	UPROPERTY(transient)
	TObjectPtr<UMaterialInstanceDynamic> ElemSelectedMaterial;
	UPROPERTY(transient)
	TObjectPtr<UMaterialInstanceDynamic> BoneUnselectedMaterial;
	UPROPERTY(transient)
	TObjectPtr<UMaterialInstanceDynamic> BoneNoCollisionMaterial;

	UPROPERTY(transient)
	TObjectPtr<UMaterialInterface> BoneMaterialHit;

	/** Mesh-space matrices showing state of just animation (ie before physics) - useful for debugging! */
	TArray<FTransform> AnimationSpaceBases;

	/** UDebugSkelMeshComponent interface */
	virtual TObjectPtr<UAnimPreviewInstance> CreatePreviewInstance() override;

	/** UPrimitiveComponent interface */
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual void AddImpulseAtLocation(FVector Impulse, FVector Location, FName BoneName = NAME_None) override;
	virtual bool ShouldCreatePhysicsState() const override;

	/** USkinnedMeshComponent interface */
	virtual void RefreshBoneTransforms(FActorComponentTickFunction* TickFunction = nullptr) override;
	
	/** Debug drawing */


	/** Accessors/helper methods */
	FTransform GetPrimitiveTransform(const FTransform& BoneTM, const int32 BodyIndex, const EAggCollisionShape::Type PrimType, const int32 PrimIndex, const float Scale) const;
	FColor GetPrimitiveColor(const int32 BodyIndex, const EAggCollisionShape::Type PrimitiveType, const int32 PrimitiveIndex) const;
	UMaterialInterface* GetPrimitiveMaterial(const int32 BodyIndex, const EAggCollisionShape::Type PrimitiveType, const int32 PrimitiveIndex) const;

	/** Manipulator methods */
	virtual void Grab(FName InBoneName, const FVector& Location, const FRotator& Rotation, bool bRotationConstrained);
	virtual void Ungrab();
	virtual void UpdateHandleTransform(const FTransform& NewTransform);
	virtual void UpdateDriveSettings(bool bLinearSoft, float LinearStiffness, float LinearDamping);

	/** Sim setup */
	virtual void CreateSimulationFloor(FBodyInstance* FloorBodyInstance, const FTransform& Transform);

public:
	virtual bool CanOverrideCollisionProfile() const override { return false;  }

private:
	void UpdateSkinnedLevelSets();
#else

	UNewtonModelEditorSkeletalMeshComponent();

	void DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi);

	FNewtonModelEditor* m_editor;
	//class FPhysicsAssetEditorSharedData* SharedData;
#endif
};
