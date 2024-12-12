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



