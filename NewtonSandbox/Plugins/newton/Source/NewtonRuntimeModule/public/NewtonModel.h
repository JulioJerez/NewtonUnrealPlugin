// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "NewtonModel.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = Newton, BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), HideCategories = (Physics, Collision))
class NEWTONRUNTIMEMODULE_API UNewtonModel : public USceneComponent
{
	GENERATED_BODY()
	public:
	UNewtonModel();
	

	UPROPERTY(EditAnywhere, Category = Newton)
	bool ShowDebug;
	
};
