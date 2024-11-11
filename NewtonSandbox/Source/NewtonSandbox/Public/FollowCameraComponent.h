// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "FollowCameraComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = NewtonDemo, BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), HideCategories = (Physics, Collision))
class NEWTONSANDBOX_API UFollowCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
	
	virtual void PostLoad() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FTransform m_localTransform;
	bool m_beginPlay = false;
};
