// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelGraphSchema.h"

#include "NewtonModelGraphNode.h"

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

	virtual UEdGraphNode* PerformAction(class UEdGraph* parentGraph, UEdGraphPin* fromPin, const FVector2D location, bool bSelectNewNode = true) override
	{
		UNewtonModelGraphNode* const node = NewObject<UNewtonModelGraphNode>(parentGraph);
		node->CreateNewGuid();
		node->NodePosX = location.X;
		node->NodePosY = location.Y;

		UEdGraphPin* const inputPin = node->CreateNodePin(EEdGraphPinDirection::EGPD_Input);
		node->CreateNodePin(EEdGraphPinDirection::EGPD_Output);

		if (fromPin)
		{
			node->GetSchema()->TryCreateConnection(fromPin, inputPin);
		}

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

const FPinConnectionResponse UNewtonModelGraphSchema::CanCreateConnection(const UEdGraphPin* a, const UEdGraphPin* b) const
{
	if (!(a && b))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT(""));
	}

	if (a->Direction == b->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("concetion do not match"));
	}

	if ((a->Direction == EEdGraphPinDirection::EGPD_Output) && (b->Direction == EEdGraphPinDirection::EGPD_Input))
	{
		bool hasInput = false;
		UEdGraphNode* const inputNode = b->GetOwningNode();
		inputNode->ForEachNodeDirectlyConnectedToInputs
		(
			[&hasInput](UEdGraphNode* node)
			{
				hasInput = true;
			}
		);
		if (!hasInput)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
		}
	}
	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT(""));
}

