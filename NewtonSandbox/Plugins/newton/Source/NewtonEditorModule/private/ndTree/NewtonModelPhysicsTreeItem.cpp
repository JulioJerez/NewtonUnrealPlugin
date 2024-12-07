// Copyright Epic Games, Inc. All Rights Reserved.

#include "ndTree/NewtonModelPhysicsTreeItem.h"

#include "NewtonEditorModule.h"
#include "ndTree/NewtonModelPhysicsTree.h"
#include "ndTree/NewtonModelPhysicsTreeItemAcyclicGraphs.h"


FNewtonModelPhysicsTreeItemJoint::FNewtonModelPhysicsTreeItemJoint(const FNewtonModelPhysicsTreeItemJoint& src)
	:FNewtonModelPhysicsTreeItem(src)
{
}

FNewtonModelPhysicsTreeItemJoint::FNewtonModelPhysicsTreeItemJoint(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode)
	:FNewtonModelPhysicsTreeItem(parentNode)
{
}

FName FNewtonModelPhysicsTreeItemJoint::BrushName() const
{
	return TEXT("jointIcon.png");
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemJoint::Clone() const
{
	return new FNewtonModelPhysicsTreeItemJoint(*this);
}


//***********************************************************************************
//
//***********************************************************************************
FNewtonModelPhysicsTreeItemShape::FNewtonModelPhysicsTreeItemShape(const FNewtonModelPhysicsTreeItemShape& src)
	:FNewtonModelPhysicsTreeItem(src)
{
}

FNewtonModelPhysicsTreeItemShape::FNewtonModelPhysicsTreeItemShape(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode)
	:FNewtonModelPhysicsTreeItem(parentNode)
{
}

FName FNewtonModelPhysicsTreeItemShape::BrushName() const
{
	return TEXT("shapeIcon.png");
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemShape::Clone() const
{
	check(0);
	return new FNewtonModelPhysicsTreeItemShape(*this);
}


//***********************************************************************************
//
//***********************************************************************************
FNewtonModelPhysicsTreeItemBody::FNewtonModelPhysicsTreeItemBody(const FNewtonModelPhysicsTreeItemBody& src)
	:FNewtonModelPhysicsTreeItem(src)
{
}

FNewtonModelPhysicsTreeItemBody::FNewtonModelPhysicsTreeItemBody(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode)
	:FNewtonModelPhysicsTreeItem(parentNode)
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemBody::Clone() const
{
	return new FNewtonModelPhysicsTreeItemBody(*this);
}

FName FNewtonModelPhysicsTreeItemBody::BrushName() const
{
	return TEXT("bodyIcon.png");
}

//***********************************************************************************
//
//***********************************************************************************
FNewtonModelPhysicsTreeItemBodyRoot::FNewtonModelPhysicsTreeItemBodyRoot(const FNewtonModelPhysicsTreeItemBodyRoot& src)
	:FNewtonModelPhysicsTreeItemBody(src)
{
}

FNewtonModelPhysicsTreeItemBodyRoot::FNewtonModelPhysicsTreeItemBodyRoot(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode)
	:FNewtonModelPhysicsTreeItemBody(parentNode)
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemBodyRoot::Clone() const
{
	return new FNewtonModelPhysicsTreeItemBodyRoot(*this);
}

FName FNewtonModelPhysicsTreeItemBodyRoot::BrushName() const
{
	return TEXT("bodyIcon.png");
}

//***********************************************************************************
//
//***********************************************************************************
FNewtonModelPhysicsTreeItem::FNewtonModelPhysicsTreeItem(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode)
	:TSharedFromThis<FNewtonModelPhysicsTreeItem>()
{
	Node = nullptr;
	m_isHidden = false;
	m_parent = parentNode;
	m_acyclicGraph = nullptr;
}

FNewtonModelPhysicsTreeItem::FNewtonModelPhysicsTreeItem(const FNewtonModelPhysicsTreeItem& src)
	:TSharedFromThis<FNewtonModelPhysicsTreeItem>()
{
	Node = src.Node;
	m_parent = nullptr;
	m_isHidden = src.m_isHidden;
	m_acyclicGraph = src.m_acyclicGraph;
}

FNewtonModelPhysicsTreeItem::~FNewtonModelPhysicsTreeItem()
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItem::Clone() const
{
	return new FNewtonModelPhysicsTreeItem(*this);
}

FName FNewtonModelPhysicsTreeItem::BrushName() const
{
	return TEXT("none");
}

FString FNewtonModelPhysicsTreeItem::GetReferencerName() const
{
	return GetRttiName().ToString();
}

void FNewtonModelPhysicsTreeItem::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(Node);
}

FName FNewtonModelPhysicsTreeItem::GetDisplayName() const
{
	check(Node != nullptr);
	//return *Node->GetName();
	return Node->Name;
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
		.ColorAndOpacity(FLinearColor(0.8, .8, .8, 1))
		.Text(FText::FromName(GetDisplayName()))
		.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 10))
		.ShadowColorAndOpacity(FLinearColor::Black)
		.ShadowOffset(FIntPoint(-2, 2))
	];
}



