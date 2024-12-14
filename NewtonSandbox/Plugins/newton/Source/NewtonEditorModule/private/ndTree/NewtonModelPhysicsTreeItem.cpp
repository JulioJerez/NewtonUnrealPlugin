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
#include "ndTree/NewtonModelPhysicsTreeItemBody.h"
#include "ndTree/NewtonModelPhysicsTreeItemAcyclicGraphs.h"

FNewtonModelPhysicsTreeItem::FNewtonModelPhysicsTreeItem(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonModelNode> modelNode)
	:TSharedFromThis<FNewtonModelPhysicsTreeItem>()
{
	Node = modelNode;
	m_parent = parentNode;
	m_acyclicGraph = nullptr;
}

FNewtonModelPhysicsTreeItem::FNewtonModelPhysicsTreeItem(const FNewtonModelPhysicsTreeItem& src)
	:TSharedFromThis<FNewtonModelPhysicsTreeItem>()
{
	Node = src.Node;
	m_parent = nullptr;
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

bool FNewtonModelPhysicsTreeItem::ShouldDrawWidget() const
{
	return false;
}

FMatrix FNewtonModelPhysicsTreeItem::GetWidgetMatrix() const
{
	return FMatrix::Identity;
}

void FNewtonModelPhysicsTreeItem::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	check(0);
}

//***********************************************************************************
//
//***********************************************************************************
FNewtonModelPhysicsTreeItemShape::FNewtonModelPhysicsTreeItemShape(const FNewtonModelPhysicsTreeItemShape& src)
	:FNewtonModelPhysicsTreeItem(src)
{
}

FNewtonModelPhysicsTreeItemShape::FNewtonModelPhysicsTreeItemShape(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonModelNode> modelNode)
	:FNewtonModelPhysicsTreeItem(parentNode, modelNode)
{
	UNewtonModelNodeCollision* const shapeNodeInfo = Cast<UNewtonModelNodeCollision>(Node);
	check(shapeNodeInfo);
	shapeNodeInfo->CreateWireFrameMesh(m_wireFrameMesh);
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

void FNewtonModelPhysicsTreeItemShape::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	const UNewtonModelNodeCollision* const shapeNode = Cast<UNewtonModelNodeCollision>(Node);
	check(shapeNode);

	if (shapeNode->m_hidden || !shapeNode->ShowDebug)
	{
		return;
	}

	check(m_parent->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")));
	const UNewtonModelNodeRigidBody* const bodyNode = Cast<UNewtonModelNodeRigidBody>(m_parent->Node);
	check(bodyNode);
	if (bodyNode->BoneIndex < 0)
	{
		return;
	}

	float thickness = 0.2f;
	//FTransform tranform(shapeNode->Transform * bodyNode->Transform);
	const FTransform tranform(shapeNode->Transform);
	for (int i = m_wireFrameMesh.Num() - 2; i >= 0; i -= 2)
	{
		FVector4 p0(tranform.TransformFVector4(m_wireFrameMesh[i + 0]));
		FVector4 p1(tranform.TransformFVector4(m_wireFrameMesh[i + 1]));
		pdi->DrawLine(p0, p1, FColor::Blue, SDPG_Foreground, thickness);
	}
}

bool FNewtonModelPhysicsTreeItemShape::ShouldDrawWidget() const
{
	const UNewtonModelNodeCollision* const shapeNode = Cast<UNewtonModelNodeCollision>(Node);
	check(shapeNode);
	return shapeNode->ShowDebug;
}

FMatrix FNewtonModelPhysicsTreeItemShape::GetWidgetMatrix() const
{
	const UNewtonModelNodeCollision* const shapeNode = Cast<UNewtonModelNodeCollision>(Node);
	check(shapeNode);
	return shapeNode->Transform.ToMatrixNoScale();
}


//***********************************************************************************
//
//***********************************************************************************
FNewtonModelPhysicsTreeItemJoint::FNewtonModelPhysicsTreeItemJoint(const FNewtonModelPhysicsTreeItemJoint& src)
	:FNewtonModelPhysicsTreeItem(src)
{
}

FNewtonModelPhysicsTreeItemJoint::FNewtonModelPhysicsTreeItemJoint(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonModelNode> modelNode)
	:FNewtonModelPhysicsTreeItem(parentNode, modelNode)
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

bool FNewtonModelPhysicsTreeItemJoint::ShouldDrawWidget() const
{
	const UNewtonModelNodeJoint* const jointNode = Cast<UNewtonModelNodeJoint>(Node);
	check(jointNode);
	return jointNode->ShowDebug;
}

FMatrix FNewtonModelPhysicsTreeItemJoint::GetWidgetMatrix() const
{
	//const UNewtonModelNodeJoint* const jointNode = Cast<UNewtonModelNodeJoint>(Node);
	//check(jointNode);

	check(m_acyclicGraph);
	check(m_acyclicGraph->m_children.Num() == 1);
	const FNewtonModelPhysicsTreeItemAcyclicGraph* const childBodyNode = m_acyclicGraph->m_children[0];
	check(childBodyNode->m_item->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")));
	const UNewtonModelNodeRigidBody* const bodyNode = Cast<UNewtonModelNodeRigidBody>(childBodyNode->m_item->Node);
	check(bodyNode);
	return bodyNode->Transform.ToMatrixNoScale();
}
