// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkinnedMeshComponent.h"
#include "NewtonSkeletalMesh.generated.h"

/**
 * 
 */
//UCLASS(Abstract, meta=(BlueprintSpawnableComponent), HideCategories = (RayTracing, Mobile, TextureStreaming, VirtualTexture, MaterialParameters, DynamicMeshComponent, Advanced, Activation, Collision, Lighting, BodySetup, Primitives, HLOD, ComponentTick, Rendering, Physics, Tags, Replication, ComponentReplication, Cooking, Events, LOD, Navigation, AssetUserData))
UCLASS(ClassGroup = NewtonAsset, BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), HideCategories = (Physics, Collision))
class NEWTONRUNTIMEMODULE_API UNewtonSkeletalMesh : public USkinnedMeshComponent
{
	GENERATED_BODY()

	public:
	UNewtonSkeletalMesh();
	
};
