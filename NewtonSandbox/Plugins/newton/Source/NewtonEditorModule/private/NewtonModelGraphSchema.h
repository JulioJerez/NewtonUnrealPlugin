// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "NewtonModelGraphSchema.generated.h"

/**
 * 
 */
UCLASS()
class UNewtonModelGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()
	public:

	UNewtonModelGraphSchema();

	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
};
