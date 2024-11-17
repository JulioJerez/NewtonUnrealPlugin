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


#include "NewtonModelGraphNode.h"

#include "ToolMenu.h"
#include "NewtonEditorModule.h"
#include "Framework/Commands/UIAction.h"

#include "NewtonModel.h"

const FName UNewtonModelGraphNode::m_subCategory(TEXT("NewtonModelLinkNode"));
const FName UNewtonModelGraphNode::m_nodeClassName(TEXT("Newton Model Link Node"));
const FName UNewtonModelGraphNode::m_nodeInformation(TEXT("create a child acyclic Node"));

UNewtonModelGraphNode::UNewtonModelGraphNode()
	:Super()
	,m_nodeInfo(nullptr)
{
}

UNewtonModelInfo* UNewtonModelGraphNode::GetNodeInfo() const
{
	return m_nodeInfo;
}

void UNewtonModelGraphNode::SetNodeInfo(UNewtonModelInfo* const info)
{
	m_nodeInfo = info;
}

FText UNewtonModelGraphNode::GetNodeTitle(ENodeTitleType::Type titleType) const
{
	//if (m_nodeInfo->Title.IsEmpty())
	//{
	//	return FText::FromString(TEXT("new node"));
	//}
	return m_nodeInfo->Title;
}

FLinearColor UNewtonModelGraphNode::GetNodeTitleColor() const
{
	return FLinearColor::Blue;
}

bool UNewtonModelGraphNode::CanUserDeleteNode() const
{
	return false;
}

void UNewtonModelGraphNode::GetNodeContextMenuActions(class UToolMenu* menu, class UGraphNodeContextMenuContext* context) const
{
	FToolMenuSection& section = menu->AddSection(TEXT("sectionName"), FText::FromString(TEXT("NewtonModel graph node action")));

	//this is fucking moronic
	UNewtonModelGraphNode* const node = (UNewtonModelGraphNode*)this;

	// we do not add or delete pins for the joint graph 
	section.AddMenuEntry
	(
		TEXT("AddPinEntry"),
		FText::FromString(TEXT("Add Response")),
		FText::FromString(TEXT("Create new pin")),
		FSlateIcon(ND_MESH_EDITOR_NAME, TEXT("NewtonModelEditor.NodeAddPinIcon")),
		FUIAction
		(
			FExecuteAction::CreateLambda
			(
				[node]()
				{
					//node->CreateNodePin(EEdGraphPinDirection::EGPD_Output);
					node->GetNodeInfo()->Responses.Add(FText::FromString("Response"));
					node->SyncPinsWithResponses();
					node->GetGraph()->NotifyGraphChanged();
					node->Modify();
				}
			)
		)
	);
	
	section.AddMenuEntry
	(
		TEXT("DeletePinEntry"),
		FText::FromString(TEXT("Delete Response")),
		FText::FromString(TEXT("Delete last pin")),
		FSlateIcon(ND_MESH_EDITOR_NAME,TEXT("NewtonModelEditor.NodeDeletePinIcon")),
		FUIAction
		(
			FExecuteAction::CreateLambda
			(
				[node]()
				{
					UEdGraphPin* const pin = node->GetPinAt(node->Pins.Num() - 1);
					if (pin->Direction == EEdGraphPinDirection::EGPD_Output)
					{
						//node->RemovePin(pin);
						node->GetNodeInfo()->Responses.Pop();
						node->SyncPinsWithResponses();
						node->GetGraph()->NotifyGraphChanged();
						node->Modify();
					}
				}
			)
		)
	);

	section.AddMenuEntry
	(
		TEXT("DeleteNodeEntry"),
		FText::FromString(TEXT("Delete Node")),
		FText::FromString(TEXT("Delete this node")),
		FSlateIcon(ND_MESH_EDITOR_NAME, TEXT("NewtonModelEditor.NodeDeleteNodeIcon")),
		FUIAction
		(
			FExecuteAction::CreateLambda
			(
				[node]()
				{
					node->GetGraph()->RemoveNode(node);
				}
			)
		)
	);
}


void UNewtonModelGraphNode::SyncPinsWithResponses()
{
	UNewtonModelInfo* const nodeInfo = GetNodeInfo();

	int numGraphPins = Pins.Num() - 1;
	int numInfoPins = nodeInfo->Responses.Num();;

	while (numGraphPins > numInfoPins)
	{
		RemovePinAt(numGraphPins - 1, EEdGraphPinDirection::EGPD_Output);
		numGraphPins--;
	}
	while (numInfoPins > numInfoPins)
	{
		//CreateNodePin(EEdGraphPinDirection::EGPD_Output, FName(nodeInfo->Responses[numGraphPins].ToString());
		CreateNodePin(EEdGraphPinDirection::EGPD_Output);
		numGraphPins++;
	}

	int index = 1;
	for (const FText& options : nodeInfo->Responses)
	{
		GetPinAt(index)->PinName = FName(options.ToString());
	}
}


UEdGraphPin* UNewtonModelGraphNode::CreateNodePin(EEdGraphPinDirection direction)
{
	const FName name((direction == EGPD_Input) ? TEXT("parent") : TEXT("child"));
	const FName category((direction == EGPD_Input) ? TEXT("input") : TEXT("output"));

	UEdGraphPin* const pin = CreatePin(direction, category, name);
	pin->PinType.PinSubCategory = m_subCategory;
	return pin;
}