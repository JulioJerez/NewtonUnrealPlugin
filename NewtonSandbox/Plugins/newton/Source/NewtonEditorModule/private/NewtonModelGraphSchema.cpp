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

	//virtual UEdGraphNode* PerformAction(class UEdGraph* parentGraph, TArray<UEdGraphPin*>& fromPins, const FVector2D location, bool bSelectNewNode = true) override
	virtual UEdGraphNode* PerformAction(class UEdGraph* parentGraph, UEdGraphPin* fromPin, const FVector2D location, bool bSelectNewNode = true) override
	{
		//UEdGraphNode* const node = NewObject<UEdGraphNode>(parentGraph);
		UNewtonModelGraphNode* const node = NewObject<UNewtonModelGraphNode>(parentGraph);
		node->CreateNewGuid();
		node->NodePosX = location.X;
		node->NodePosY = location.Y;

		//node->CreatePin(EEdGraphPinDirection::EGPD_Input, TEXT("InputCategory"), TEXT("InputName"));
		//node->CreatePin(EEdGraphPinDirection::EGPD_Output, TEXT("OutputCategory1"), TEXT("OutputName1"));
		//node->CreatePin(EEdGraphPinDirection::EGPD_Output, TEXT("OutputCategory2"), TEXT("OutputName2"));

		UEdGraphPin* const inputPin = node->CreateNewtonModePin(EEdGraphPinDirection::EGPD_Input, TEXT("pinInput"));
		node->CreateNewtonModePin(EEdGraphPinDirection::EGPD_Output, TEXT("pinOutput"));

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
		UEdGraphNode* const inputNode = b->GetOwningNode();


		bool hasInput = false;
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

