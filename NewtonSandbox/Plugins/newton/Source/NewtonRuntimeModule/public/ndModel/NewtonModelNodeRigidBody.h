


// Fill out your copyright notice in the Description page of Project Settings.





#pragma once


#include "CoreMinimal.h"
#include "NewtonModelNode.h"
#include "NewtonModelNodeRigidBody.generated.h"

/**
 * 
 */
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonModelNodeRigidBody : public UNewtonModelNode
{
	GENERATED_BODY()
	public:
	UNewtonModelNodeRigidBody();

	UPROPERTY(VisibleAnywhere)
	FName BoneName;

	UPROPERTY(VisibleAnywhere)
	int BoneIndex;
};
