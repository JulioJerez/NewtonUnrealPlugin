/* Copyright (c) <2024-2024> <Julio Jerez, Newton Game Dynamics>
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
*
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any source distribution.
*/


#include "NewtonModelGraphSchema.h"

#include "NewtonModel.h"
#include "NewtonModelGraphNode.h"
#include "NewtonModelGraphNodeRoot.h"

USTRUCT()
struct FNewtonModelGraphSchemaActionGraphNode : public FEdGraphSchemaAction
{
	FNewtonModelGraphSchemaActionGraphNode()
		:FEdGraphSchemaAction()
	{
	}

	FNewtonModelGraphSchemaActionGraphNode(FText nodeCategory, FText menuDesc, FText toolTip, INT32 grouping)
		:FEdGraphSchemaAction(nodeCategory, menuDesc, toolTip, grouping)
	{
	}

	virtual UEdGraphNode* PerformAction(UEdGraph* parentGraph, UEdGraphPin* fromPin, const FVector2D location, bool bSelectNewNode = true) override
	{
		UNewtonModelGraphNode* const node = NewObject<UNewtonModelGraphNode>(parentGraph);
		node->Initialize(nullptr);
		node->CreateNewGuid();
		node->NodePosX = location.X;
		node->NodePosY = location.Y;

		UEdGraphPin* const inputPin = node->GetInputPin();
		FString defaulResponce(TEXT("output"));
		node->GetNodeInfo()->Responses.Add(FText::FromString(defaulResponce));

		if (fromPin && inputPin)
		{
			node->GetSchema()->TryCreateConnection(fromPin, inputPin);
		}

		parentGraph->AddNode(node, true, true);
		//parentGraph->NotifyGraphChanged();
		return node;
	}
};

// **************************************************************************************
// 
// 
// **************************************************************************************
USTRUCT()
struct FNewtonModelGraphSchemaActionGraphNodeRoot : public FEdGraphSchemaAction
{
	FNewtonModelGraphSchemaActionGraphNodeRoot()
		:FEdGraphSchemaAction()
	{
	}

	FNewtonModelGraphSchemaActionGraphNodeRoot(FText nodeCategory, FText menuDesc, FText toolTip, INT32 grouping)
		:FEdGraphSchemaAction(nodeCategory, menuDesc, toolTip, grouping)
	{
	}

	virtual UEdGraphNode* PerformAction(UEdGraph* parentGraph, UEdGraphPin* fromPin, const FVector2D location, bool bSelectNewNode = true) override
	{
		UNewtonModelGraphNodeRoot* const node = NewObject<UNewtonModelGraphNodeRoot>(parentGraph);
		node->Initialize(nullptr);
		node->CreateNewGuid();
		node->NodePosX = location.X;
		node->NodePosY = location.Y; 

		FString defaulResponce(TEXT("output"));
		node->GetNodeInfo()->Responses.Add(FText::FromString(defaulResponce));

		parentGraph->AddNode(node, true, true);
		//parentGraph->NotifyGraphChanged();
		return node;
	}
};

// **************************************************************************************
// 
// 
// **************************************************************************************
UNewtonModelGraphSchema::UNewtonModelGraphSchema()
	:Super()
{
}

void UNewtonModelGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& contextMenuBuilder) const
{
	Super::GetGraphContextActions(contextMenuBuilder);

	const FText category; 

	const FText classNameGraphNode(FText::FromName(UNewtonModelGraphNode::m_nodeClassName));
	const FText informationGraphNode(FText::FromName(UNewtonModelGraphNode::m_nodeInformation));
	TSharedPtr<FNewtonModelGraphSchemaActionGraphNode> graphNode(new FNewtonModelGraphSchemaActionGraphNode(category, classNameGraphNode, informationGraphNode, 0));
	contextMenuBuilder.AddAction(graphNode);

	const FText classNameGraphNodeRoot(FText::FromName(UNewtonModelGraphNodeRoot::m_nodeClassName));
	const FText informationGraphNodeRoot(FText::FromName(UNewtonModelGraphNodeRoot::m_nodeInformation));
	TSharedPtr<FNewtonModelGraphSchemaActionGraphNodeRoot> graphNodeRoot(new FNewtonModelGraphSchemaActionGraphNodeRoot(category, classNameGraphNodeRoot, informationGraphNodeRoot, 0));
	contextMenuBuilder.AddAction(graphNodeRoot);
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

