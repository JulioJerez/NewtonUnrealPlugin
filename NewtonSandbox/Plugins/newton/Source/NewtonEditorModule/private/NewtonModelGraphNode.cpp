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

#include "NewtonModelEditorCommon.h"

const FName UNewtonModelGraphNode::m_subCategory(TEXT("NewtonModelLinkNode"));
const FName UNewtonModelGraphNode::m_nodeClassName(TEXT("Newton Model Link Node"));
const FName UNewtonModelGraphNode::m_nodeInformation(TEXT("create a child acyclic Node"));

UNewtonModelGraphNode::UNewtonModelGraphNode()
	:Super()
{
	m_inputPin = nullptr;
	m_ouputPin = nullptr;
	m_nodeInfo = nullptr;
}

UNewtonModelInfo* UNewtonModelGraphNode::GetNodeInfo() const
{
	return m_nodeInfo;
}

UEdGraphPin* UNewtonModelGraphNode::GetInputPin() const
{
	return m_inputPin;
}

UEdGraphPin* UNewtonModelGraphNode::GetOutputPin() const
{
	return m_ouputPin;
}

FText UNewtonModelGraphNode::GetNodeTitle(ENodeTitleType::Type titleType) const
{
	check(m_nodeInfo);
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

#if 0
	// we do not add or delete pins for the joint graph 
	section.AddMenuEntry
	(
		TEXT("AddPinEntry"),
		FText::FromString(TEXT("Add Response")),
		FText::FromString(TEXT("Create new pin")),
		FSlateIcon(ND_MESH_EDITOR_NAME, TEXT("FNewtonModelEditor.NodeAddPinIcon")),
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
		FSlateIcon(ND_MESH_EDITOR_NAME,TEXT("FNewtonModelEditor.NodeDeletePinIcon")),
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
#endif

	section.AddMenuEntry
	(
		TEXT("DeleteNodeEntry"),
		FText::FromString(TEXT("Delete Node")),
		FText::FromString(TEXT("Delete this node")),
		FSlateIcon(ND_MESH_EDITOR_NAME, TEXT("FNewtonModelEditor.NodeDeleteNodeIcon")),
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

void UNewtonModelGraphNode::Initialize(const UNewtonModelInfo* const srcInfo)
{
	m_inputPin = CreateNodePin(EEdGraphPinDirection::EGPD_Input);
	m_ouputPin = CreateNodePin(EEdGraphPinDirection::EGPD_Output);

	m_nodeInfo = NewObject<UNewtonModelInfo>(this);
	if (srcInfo)
	{
		m_nodeInfo->Initialize(srcInfo);
	}
}

void UNewtonModelGraphNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	GetGraph()->NotifyGraphChanged();
}

UEdGraphPin* UNewtonModelGraphNode::CreateNodePin(EEdGraphPinDirection direction)
{
	const FName name((direction == EGPD_Input) ? TEXT("parent") : TEXT("child"));
	const FName category((direction == EGPD_Input) ? TEXT("input") : TEXT("output"));

	UEdGraphPin* const pin = CreatePin(direction, category, name);
	pin->PinType.PinSubCategory = m_subCategory;
	return pin;
}

void UNewtonModelGraphNode::SyncPinsWithResponses()
{
	// at this time we are not changing the pin connections, since they are fixed
	//const TArray<UEdGraphPin*>& pins = GetAllPins();
	//const UNewtonModelInfo* const nodeInfo = GetNodeInfo();
	//check(pins.Num() == nodeInfo->Responses.Num());
	//
	//const TArray<FText>& reponses = nodeInfo->Responses;
	//pins[0]->PinName = FName(reponses[0].ToString());
	//pins[1]->PinName = FName(reponses[1].ToString());
}