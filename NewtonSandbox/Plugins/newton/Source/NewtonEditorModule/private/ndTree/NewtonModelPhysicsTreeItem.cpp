// Copyright Epic Games, Inc. All Rights Reserved.

#include "ndTree/NewtonModelPhysicsTreeItem.h"

#include "NewtonEditorModule.h"
#include "ndTree/NewtonModelPhysicsTree.h"
#include "ndTree/NewtonModelPhysicsTreeItemAcyclicGraphs.h"

FNewtonModelPhysicsTreeItem::FNewtonModelPhysicsTreeItem(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, const FName& displayName)
	:TSharedFromThis<FNewtonModelPhysicsTreeItem>()
{
	m_parent = parentNode;
	m_acyclicGraph = nullptr;
	m_displayName = displayName;
}

FNewtonModelPhysicsTreeItem::~FNewtonModelPhysicsTreeItem()
{
}

const FName& FNewtonModelPhysicsTreeItem::GetDisplayName() const
{
	return m_displayName;
}

const FSlateBrush* FNewtonModelPhysicsTreeItem::GetIcon() const
{
	FNewtonEditorModule& module = FModuleManager::GetModuleChecked<FNewtonEditorModule>(TEXT("NewtonEditorModule"));
	return module.GetBrush(BrushName());
}

void FNewtonModelPhysicsTreeItem::GenerateWidgetForNameColumn(TSharedPtr<SHorizontalBox> box, FIsSelected isSelected)
{
	box->AddSlot()
	.AutoWidth()
	.Padding(FMargin(0.0f, 2.0f))
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	[
		SNew(SImage)
		//.ColorAndOpacity(this, &FNewtonModelPhysicsTreeItemBody::GetBoneTextColor, InIsSelected)
		.ColorAndOpacity(FLinearColor(0.8, .8, .8, 1))
		.Image(this, &FNewtonModelPhysicsTreeItem::GetIcon)
	];
	
	//FText ToolTip = GetBoneToolTip();
	box->AddSlot()
	.AutoWidth()
	.Padding(4, 0, 0, 0)
	.VAlign(VAlign_Center)
	[
		SNew(STextBlock)
		//.ColorAndOpacity(this, &FNewtonModelPhysicsTreeItemBody::GetBoneTextColor, InIsSelected)
		.ColorAndOpacity(FLinearColor(0.8, .8, .8, 1))
		.Text(FText::FromName(GetDisplayName()))
		//.HighlightText(FilterText)
		.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 10))
		//.ToolTipText(ToolTip)
		.ShadowColorAndOpacity(FLinearColor::Black)
		.ShadowOffset(FIntPoint(-2, 2))
	];
}
