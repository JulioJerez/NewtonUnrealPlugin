// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelGraphSchema.h"


USTRUCT()
struct FNewtonModelGraphSchemaAction : public FEdGraphSchemaAction
{
	FNewtonModelGraphSchemaAction()
		:FEdGraphSchemaAction()
	{
	}

	FNewtonModelGraphSchemaAction(FText nodeCategory, FText menuDesc, FText toolTip, INT32 grouping)
		:FEdGraphSchemaAction(nodeCategory, menuDesc, toolTip, grouping)
	{
	}

	virtual UEdGraphNode* PerformAction(class UEdGraph* parentGraph, TArray<UEdGraphPin*>& fromPins, const FVector2D location, bool bSelectNewNode = true) override
	{
		UEdGraphNode* const node = NewObject<UEdGraphNode>(parentGraph);
		node->NodePosX = location.X;
		node->NodePosY = location.Y;

		node->CreatePin(EEdGraphPinDirection::EGPD_Input, TEXT("InputCategory"), TEXT("InputName"));

		node->CreatePin(EEdGraphPinDirection::EGPD_Output, TEXT("OutputCategory1"), TEXT("OutputName1"));
		node->CreatePin(EEdGraphPinDirection::EGPD_Output, TEXT("OutputCategory2"), TEXT("OutputName2"));

		parentGraph->NotifyGraphChanged();
		parentGraph->AddNode(node, true, true);
		return node;
	}
};


UNewtonModelGraphSchema::UNewtonModelGraphSchema()
	:Super()
{
}

void UNewtonModelGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& contextMenuBuilder) const
{
	Super::GetGraphContextActions(contextMenuBuilder);

	const FText tip(FText::FromString(TEXT("tip")));
	const FText desc(FText::FromString(TEXT("description")));
	const FText category(FText::FromString(TEXT("category0")));
	TSharedPtr<FNewtonModelGraphSchemaAction> node (new FNewtonModelGraphSchemaAction(category, desc, tip, 0));
	contextMenuBuilder.AddAction(node);
}