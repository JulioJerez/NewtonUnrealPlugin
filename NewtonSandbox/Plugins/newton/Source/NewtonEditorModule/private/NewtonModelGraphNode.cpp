// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelGraphNode.h"

#include "ToolMenu.h"
#include "NewtonEditorModule.h"
#include "Framework/Commands/UIAction.h"

UNewtonModelGraphNode::UNewtonModelGraphNode()
	:Super()
{
}

FText UNewtonModelGraphNode::GetNodeTitle(ENodeTitleType::Type titleType) const
{
	return FText::FromString(TEXT("testNodeTitle"));
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
	FToolMenuSection& section = menu->AddSection(TEXT("secsionName"), FText::FromString(TEXT("NewtoModel graph node action")));

	//this is fucking moronic
	UNewtonModelGraphNode* const node = (UNewtonModelGraphNode*)this;

	section.AddMenuEntry
	(
		TEXT("AddPinEntry"),
		FText::FromString(TEXT("Add Pin")),
		FText::FromString(TEXT("Create new pin")),
		FSlateIcon(ND_MESH_EDITOR_NAME, TEXT("NewtonModelEditor.NodeAddPinIcon")),
		FUIAction
		(
			FExecuteAction::CreateLambda
			(
				[node]()
				{
					node->CreatePin(EEdGraphPinDirection::EGPD_Output, TEXT("Ouput_1"), TEXT("Ouput_2"));
					node->GetGraph()->NotifyGraphChanged();
					node->Modify();
				}
			)
		)
	);

	section.AddMenuEntry
	(
		TEXT("DeletePinEntry"),
		FText::FromString(TEXT("Delete Pin")),
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
						node->RemovePin(pin);
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