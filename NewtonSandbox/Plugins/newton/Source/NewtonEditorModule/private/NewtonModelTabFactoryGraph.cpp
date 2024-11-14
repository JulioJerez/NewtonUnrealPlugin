// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelTabFactoryGraph.h"
#include "GraphEditor.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"

#include "NewtonModel.h"
#include "NewtonModelEditor.h"

FName NewtonModelTabFactoryGraph::m_tabName("NewtonModelTabGraph");

NewtonModelTabFactoryGraph::NewtonModelTabFactoryGraph(const TSharedPtr<NewtonModelEditor>& editor)
	:FWorkflowTabFactory(m_tabName, editor)
	,m_editor(editor)
{
	TabLabel = FText::FromString(TEXT("Graph"));
	ViewMenuDescription = FText::FromString(TEXT("Newton Model Graph"));
	ViewMenuTooltip = FText::FromString(TEXT("Show Newton Model Graph"));
}

NewtonModelTabFactoryGraph::~NewtonModelTabFactoryGraph()
{
}

FText NewtonModelTabFactoryGraph::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	const FText name(FText::FromString(TEXT("NewtonModelComponent Graph view")));
	return name;
}

TSharedRef<SWidget> NewtonModelTabFactoryGraph::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<NewtonModelEditor> editor = m_editor.Pin();
	
#if 0
	// this is by far, the worse c++ programing style I have ever seen.
	// it is like some one when out of his way to obfuscate the code.
	TSharedRef<SWidget> widget
	(
		SNew(SVerticalBox) +
		SVerticalBox::Slot().FillHeight(1.0f).HAlign(HAlign_Fill)
		[SNew(STextBlock).Text(FText::FromString(TEXT("xxxxx")))]
	);
#else
	//const FText msg(FText::FromString(TEXT("this is a lot more complicated that it needed to be.")));
	//TSharedRef<STextBlock> editorRef (SNew(STextBlock));
	//editorRef->SetText(msg);
	TSharedRef<SGraphEditor> editorRef(SNew(SGraphEditor).IsEditable(true).GraphToEdit(editor->m_graphEditor));

	SVerticalBox::FSlot::FSlotArguments arg (SVerticalBox::Slot());
	arg.FillHeight(1.0);
	arg.HAlign(HAlign_Fill)[editorRef];
	TSharedRef<SWidget> widget(SNew(SVerticalBox) + arg);
#endif	
	return widget;
}
