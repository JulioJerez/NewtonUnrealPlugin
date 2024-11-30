// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NewtonModelAcyclicGraph.generated.h"

class UNewtonModelAcyclicNode;

/**
 * 
 */
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonModelAcyclicGraph : public UObject
{
	GENERATED_BODY()
	UNewtonModelAcyclicGraph();
	
	UPROPERTY(EditAnywhere)
	FText Title;

	UPROPERTY()
	UNewtonModelAcyclicNode* RootNode;

};
