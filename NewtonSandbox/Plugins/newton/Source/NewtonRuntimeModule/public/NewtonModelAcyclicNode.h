// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NewtonModelAcyclicNode.generated.h"

/**
 * 
 */
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonModelAcyclicNode : public UObject
{
	GENERATED_BODY()
	public:
	UNewtonModelAcyclicNode();

	UPROPERTY(EditAnywhere)
	FText Title;

	UPROPERTY()
	UNewtonModelAcyclicNode* Parent;

	UPROPERTY()
	TArray<UNewtonModelAcyclicNode*> Children;
};
