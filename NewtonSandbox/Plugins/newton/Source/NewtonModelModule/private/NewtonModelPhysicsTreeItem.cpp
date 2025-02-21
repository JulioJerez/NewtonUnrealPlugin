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


#include "NewtonModelPhysicsTreeItem.h"

#include "NewtonModelModule.h"
#include "NewtonModelEditor.h"
#include "NewtonModelPhysicsTree.h"
#include "NewtonModelPhysicsTreeItemBody.h"


FNewtonModelPhysicsTreeItem::FNewtonModelPhysicsTreeItem(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode, FNewtonModelEditor* const editor)
	:TSharedFromThis<FNewtonModelPhysicsTreeItem>()
{
	m_editor = editor;
	m_node = modelNode;
	m_parent = parentNode;
	m_acyclicGraph = nullptr;
}

FNewtonModelPhysicsTreeItem::FNewtonModelPhysicsTreeItem(const FNewtonModelPhysicsTreeItem& src)
	:TSharedFromThis<FNewtonModelPhysicsTreeItem>()
{
	m_parent = nullptr;
	m_node = src.m_node;
	m_editor = src.m_editor;
	m_acyclicGraph = src.m_acyclicGraph;
}

FNewtonModelPhysicsTreeItem::~FNewtonModelPhysicsTreeItem()
{
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItem::Clone() const
{
	return new FNewtonModelPhysicsTreeItem(*this);
}

TObjectPtr<UNewtonLink> FNewtonModelPhysicsTreeItem::GetNode() const
{
	return m_node;
}

TSharedPtr<FNewtonModelPhysicsTreeItem> FNewtonModelPhysicsTreeItem::GetParent() const
{
	return m_parent;
}

FNewtonModelPhysicsTreeItemAcyclicGraph* FNewtonModelPhysicsTreeItem::GetAcyclicGraph() const
{
	return m_acyclicGraph;
}

FName FNewtonModelPhysicsTreeItem::BrushName() const
{
	return TEXT("none");
}

FString FNewtonModelPhysicsTreeItem::GetReferencerName() const
{
	return GetRttiName().ToString();
}

void FNewtonModelPhysicsTreeItem::AddReferencedObjects(FReferenceCollector& collector)
{
	collector.AddReferencedObject(m_node);
}

FName FNewtonModelPhysicsTreeItem::GetDisplayName() const
{
	check(m_node != nullptr);
	return m_node->Name;
}

const FSlateBrush* FNewtonModelPhysicsTreeItem::GetIcon() const
{
	FNewtonModelModule& module = FModuleManager::GetModuleChecked<FNewtonModelModule>(ND_NEWTON_MODEL_MODULE_NAME);
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
	return CalculateGlobalTransform().ToMatrixNoScale();
}

FTransform FNewtonModelPhysicsTreeItem::CalculateGlobalTransform() const
{
	FTransform transform(m_node->Transform);
	for (TSharedPtr<FNewtonModelPhysicsTreeItem> parent(m_parent); parent != nullptr; parent = parent->m_parent)
	{
		const FTransform parentTransform(parent->m_node->Transform);
		transform = transform * parentTransform;
	}
	return transform;
}

void FNewtonModelPhysicsTreeItem::ApplyDeltaTransform(const FVector& inDrag, const FRotator& inRot, const FVector& inScale)
{
	check(0);
}

void FNewtonModelPhysicsTreeItem::PrepareNode() const
{
}

bool FNewtonModelPhysicsTreeItem::HaveSelection() const
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
	return false;
}

void FNewtonModelPhysicsTreeItem::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	const UNewtonLinkJointLoop* const jointNode = Cast<UNewtonLinkJointLoop>(m_node);

	check(jointNode);

	if (jointNode->m_hidden || !jointNode->ShowDebug)
	{
		return;
	}

	float scale = jointNode->DebugScale * 5.0f;
	float thickness = NEWTON_EDITOR_DEBUG_THICKENESS * 5.0f;

	const FTransform parentTransform(CalculateGlobalTransform());
	const FVector positionParent(parentTransform.GetLocation());
	const FVector xAxisParent(parentTransform.GetUnitAxis(EAxis::X));
	const FVector yAxisParent(parentTransform.GetUnitAxis(EAxis::Y));
	const FVector zAxisParent(parentTransform.GetUnitAxis(EAxis::Z));
	pdi->DrawLine(positionParent, positionParent + scale * xAxisParent, FColor::Red, SDPG_Foreground, thickness);
	pdi->DrawLine(positionParent, positionParent + scale * yAxisParent, FColor::Green, SDPG_Foreground, thickness);
	pdi->DrawLine(positionParent, positionParent + scale * zAxisParent, FColor::Blue, SDPG_Foreground, thickness);

	const FTransform childTransform(jointNode->TargetFrame * parentTransform);
	const FVector positionChild(childTransform.GetLocation());
	const FVector xAxisChild(childTransform.GetUnitAxis(EAxis::X));
	const FVector yAxisChild(childTransform.GetUnitAxis(EAxis::Y));
	const FVector zAxisChild(childTransform.GetUnitAxis(EAxis::Z));
	pdi->DrawLine(positionChild, positionChild + scale * xAxisChild, FColor::Red, SDPG_Foreground, thickness);
	pdi->DrawLine(positionChild, positionChild + scale * yAxisChild, FColor::Green, SDPG_Foreground, thickness);
	pdi->DrawLine(positionChild, positionChild + scale * zAxisChild, FColor::Blue, SDPG_Foreground, thickness);
}

void FNewtonModelPhysicsTreeItem::OnPropertyChange(const FPropertyChangedEvent& event)
{
	FProperty* const property = event.Property;
	if (property->GetName() != TEXT("TargetBodyName"))
	{
		return;
	}

	check(GetParent());
	UNewtonLinkJointLoop* const loopNode = Cast<UNewtonLinkJointLoop>(m_node);
	TSharedPtr<FNewtonModelPhysicsTreeItem> parentBodyItem(GetParent());

	TSharedPtr<FNewtonModelPhysicsTreeItem> childBodyItem(nullptr);
	TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>> items(m_editor->GetNewtonModelPhysicsTree()->GetItems());
	for (TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>>::TConstIterator it(items); it; ++it)
	{
		TSharedPtr<FNewtonModelPhysicsTreeItem> itemInSet(*it);
		UNewtonLink* const linkNode = Cast<UNewtonLink>(itemInSet->GetNode());
		if (linkNode->Name == loopNode->TargetBodyName)
		{
			childBodyItem = itemInSet;
			break;
		}
	}

	if (childBodyItem.IsValid())
	{
		loopNode->TargetFrame = childBodyItem->CalculateGlobalTransform() * parentBodyItem->CalculateGlobalTransform().Inverse();
	}
}
