// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "NewtonModelGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class UNewtonModelGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

	public:
	UNewtonModelGraphNode();

	virtual bool CanUserDeleteNode() const;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type titleType) const override;
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	
};
