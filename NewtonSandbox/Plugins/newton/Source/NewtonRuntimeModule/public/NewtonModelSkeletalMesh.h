// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "NewtonModelSkeletalMesh.generated.h"


UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonModelSkeletalMesh : public USkeletalMeshComponent
{
	GENERATED_BODY()
	public:
	UNewtonModelSkeletalMesh();
	
	virtual void FinalizeBoneTransform() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<FTransform> m_matrixPallete;
};
