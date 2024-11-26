// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NewtonModelPhysicNode.generated.h"

/**
 * 
 */
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonModelPhysicNode : public UObject
{
	GENERATED_BODY()
	public:
	UNewtonModelPhysicNode();

	UPROPERTY(EditAnywhere)
	FText Title;

	UPROPERTY()
	UNewtonModelPhysicNode* Parent;

	UPROPERTY()
	TArray<UNewtonModelPhysicNode*> Children;
};
