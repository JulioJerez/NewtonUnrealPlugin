// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NewtonModelNode.generated.h"

/**
 * 
 */
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonModelNode : public UObject
{
	GENERATED_BODY()
	public:
	UNewtonModelNode();


	virtual void AttachNode(UNewtonModelNode* const node);

	UPROPERTY(EditAnywhere)
	FName Name;

	UPROPERTY()
	UNewtonModelNode* Parent;

	UPROPERTY()
	TArray<UNewtonModelNode*> Children;
};
