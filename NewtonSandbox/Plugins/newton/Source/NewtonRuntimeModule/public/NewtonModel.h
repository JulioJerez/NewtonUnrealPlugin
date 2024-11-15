// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Components/SceneComponent.h"
#include "UObject/Object.h"
#include "NewtonModel.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = Newton, BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), HideCategories = (Physics, Collision))
class NEWTONRUNTIMEMODULE_API UNewtonModel : public UObject
{
	GENERATED_BODY()
	public:
	UNewtonModel();

//	UPROPERTY(EditAnywhere, Category = Newton)
//	bool ShowDebug;

	//UPROPERTY(EditAnywhere, Transient, Setter = SetSkeletalMeshAsset, BlueprintSetter = SetSkeletalMeshAsset, Getter = GetSkeletalMeshAsset, BlueprintGetter = GetSkeletalMeshAsset, Category = Mesh)
	UPROPERTY(EditAnywhere, Category = NewtonModel, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMesh> SkeletalMeshAsset;
};
