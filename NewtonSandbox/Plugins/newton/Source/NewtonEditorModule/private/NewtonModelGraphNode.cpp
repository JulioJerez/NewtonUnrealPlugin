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

UEdGraphPin* UNewtonModelGraphNode::CreateNodePin(EEdGraphPinDirection direction)
{
	const FName subCategory(TEXT("SNewtonModelGraphPin"));
	const FName name ((direction == EGPD_Input) ? TEXT("parent") : TEXT("child"));
	const FName category ((direction == EGPD_Input) ? TEXT("input") : TEXT("output"));

	UEdGraphPin* const pin = CreatePin(direction, category, name);
	pin->PinType.PinSubCategory = subCategory;
	return pin;
}

void UNewtonModelGraphNode::GetNodeContextMenuActions(class UToolMenu* menu, class UGraphNodeContextMenuContext* context) const
{
	FToolMenuSection& section = menu->AddSection(TEXT("sectionName"), FText::FromString(TEXT("NewtonModel graph node action")));

	//this is fucking moronic
	UNewtonModelGraphNode* const node = (UNewtonModelGraphNode*)this;

	// we do not add or delete pins for the joint graph 
	//section.AddMenuEntry
	//(
	//	TEXT("AddPinEntry"),
	//	FText::FromString(TEXT("Add Pin")),
	//	FText::FromString(TEXT("Create new pin")),
	//	FSlateIcon(ND_MESH_EDITOR_NAME, TEXT("NewtonModelEditor.NodeAddPinIcon")),
	//	FUIAction
	//	(
	//		FExecuteAction::CreateLambda
	//		(
	//			[node]()
	//			{
	//				node->CreateNodePin(EEdGraphPinDirection::EGPD_Output);
	//				node->GetGraph()->NotifyGraphChanged();
	//				node->Modify();
	//			}
	//		)
	//	)
	//);
	//
	//section.AddMenuEntry
	//(
	//	TEXT("DeletePinEntry"),
	//	FText::FromString(TEXT("Delete Pin")),
	//	FText::FromString(TEXT("Delete last pin")),
	//	FSlateIcon(ND_MESH_EDITOR_NAME,TEXT("NewtonModelEditor.NodeDeletePinIcon")),
	//	FUIAction
	//	(
	//		FExecuteAction::CreateLambda
	//		(
	//			[node]()
	//			{
	//				UEdGraphPin* const pin = node->GetPinAt(node->Pins.Num() - 1);
	//				if (pin->Direction == EEdGraphPinDirection::EGPD_Output)
	//				{
	//					node->RemovePin(pin);
	//					node->GetGraph()->NotifyGraphChanged();
	//					node->Modify();
	//				}
	//			}
	//		)
	//	)
	//);

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