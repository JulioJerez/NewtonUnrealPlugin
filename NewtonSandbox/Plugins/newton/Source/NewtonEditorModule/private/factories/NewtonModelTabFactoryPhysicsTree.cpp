// Copyright Epic Games, Inc. All Rights Reserved.

#include "factories/NewtonModelTabFactoryPhysicsTree.h"

#include "NewtonModelEditor.h"
#include "NewtonModelEditorCommon.h"
#include "ndTree/NewtonModelPhysicsTree.h"

#define LOCTEXT_NAMESPACE "NewtonModelTabFactoryPhysicsTree"
#define TREE_IDENTIFIER TEXT("NewtonModelTabPhysicsTree")

FNewtonModelTabFactoryPhysicsTree::FNewtonModelTabFactoryPhysicsTree(const TSharedPtr<FNewtonModelEditor>& editor)
	:FWorkflowTabFactory(TREE_IDENTIFIER, editor)
	,m_editor(editor)
{
	//EnableTabPadding();
	//bIsSingleton = true;

	TabLabel = FText::FromString(TEXT("Physics Tree"));
	ViewMenuDescription = FText::FromString(TEXT("NewtonModel physics tree"));
	ViewMenuTooltip = FText::FromString(TEXT("Show NewtonModel physics tree"));
	//TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Persona.Tabs.SkeletonTree");
}

FText FNewtonModelTabFactoryPhysicsTree::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	const FText name(FText::FromString(TEXT("NewtonModel physics tree view")));
	return name;
}

//TSharedPtr<SToolTip> FNewtonModelTabFactoryPhysicsTree::CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const
//{
//	return IDocumentation::Get()->CreateToolTip(LOCTEXT("SkeletonTreeTooltip", "The Skeleton Tree tab lets you see and select bones (and sockets) in the skeleton hierarchy."), NULL, TEXT("Shared/Editors/Persona"), TEXT("SkeletonTree_Window"));
//}

TSharedRef<SWidget> FNewtonModelTabFactoryPhysicsTree::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
#if 0
	return TSharedRef<SWidget>
		(
			SNew(SVerticalBox) +
			SVerticalBox::Slot().FillHeight(1.0f).HAlign(HAlign_Fill)
			[SNew(STextBlock).Text(FText::FromString(TEXT("Placeholder")))]
		);
#else

	TSharedPtr<FNewtonModelPhysicsTree> physicsTree(m_editor.Pin()->GetNewtonModelPhysicsTree());
	check(physicsTree.IsValid());
	return physicsTree.ToSharedRef();
#endif
}

#undef LOCTEXT_NAMESPACE
