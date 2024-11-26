// Copyright Epic Games, Inc. All Rights Reserved.

#include "factories/NewtonModelTabFactoryPhysicTree.h"
//#include "Styling/AppStyle.h"
//#include "IDocumentation.h"
//#include "SkeletonEditor.h"

#include "NewtonModelEditor.h"
#include "NewtonModelPhysicTree.h"
#include "NewtonModelEditorCommon.h"


#define LOCTEXT_NAMESPACE "NewtonModelTabFactoryPhysicTree"
#define TREE_IDENTIFIER TEXT("NewtonModelTabPhysicTreeh")

//FNewtonModelTabFactoryPhysicTree::FNewtonModelTabFactoryPhysicTree(TSharedPtr<FAssetEditorToolkit> InHostingApp, TSharedRef<ISkeletonTree> InSkeletonTree)
FNewtonModelTabFactoryPhysicTree::FNewtonModelTabFactoryPhysicTree(const TSharedPtr<FNewtonModelEditor>& editor)
	: FWorkflowTabFactory(TREE_IDENTIFIER, editor)
	,m_editor(editor)
{
	//EnableTabPadding();
	//bIsSingleton = true;

	TabLabel = FText::FromString(TEXT("Physics Tree"));
	ViewMenuDescription = FText::FromString(TEXT("NewtonModel physics tree"));
	ViewMenuTooltip = FText::FromString(TEXT("Show NewtonModel physics tree"));
	//TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Persona.Tabs.SkeletonTree");
}

FText FNewtonModelTabFactoryPhysicTree::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	const FText name(FText::FromString(TEXT("NewtonModel physics tree view")));
	return name;
}

//TSharedPtr<SToolTip> FNewtonModelTabFactoryPhysicTree::CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const
//{
//	return IDocumentation::Get()->CreateToolTip(LOCTEXT("SkeletonTreeTooltip", "The Skeleton Tree tab lets you see and select bones (and sockets) in the skeleton hierarchy."), NULL, TEXT("Shared/Editors/Persona"), TEXT("SkeletonTree_Window"));
//}

TSharedRef<SWidget> FNewtonModelTabFactoryPhysicTree::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
#if 0
	return TSharedRef<SWidget>
	(
		SNew(SVerticalBox) +
		SVerticalBox::Slot().FillHeight(1.0f).HAlign(HAlign_Fill)
		[SNew(STextBlock).Text(FText::FromString(TEXT("Placeholder")))]
	);
#else

	TSharedPtr<FNewtonModelPhysicTree> physicsTree(m_editor.Pin()->GetNewtonModelPhysicTree());
	if (physicsTree.IsValid())
	{
		return physicsTree.ToSharedRef();
	}
	else
	{
		return SNullWidget::NullWidget;
	}
#endif
}

#undef LOCTEXT_NAMESPACE
