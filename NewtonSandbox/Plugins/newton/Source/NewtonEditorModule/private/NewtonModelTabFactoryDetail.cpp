// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelTabFactoryDetail.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"

#include "NewtonModel.h"
#include "NewtonModelEditor.h"

FName NewtonModelTabFactoryDetail::m_tabName("NewtonModelTabDetail");

NewtonModelTabFactoryDetail::NewtonModelTabFactoryDetail(const TSharedPtr<NewtonModelEditor>& editor)
	:FWorkflowTabFactory(m_tabName, editor)
	,m_editor(editor)
{
	// I prefer "Properties", but unreal uses "Detail" for this tab
	TabLabel = FText::FromString(TEXT("Details"));
	//TabLabel = FText::FromString(TEXT("Properties"));
	ViewMenuDescription = FText::FromString(TEXT("Newton Model Detail"));
	ViewMenuTooltip = FText::FromString(TEXT("Show Newton Model Detail"));
}

NewtonModelTabFactoryDetail::~NewtonModelTabFactoryDetail()
{
}

FText NewtonModelTabFactoryDetail::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	const FText name(FText::FromString(TEXT("NewtonModelComponent detail view")));
	return name;
}

TSharedRef<SWidget> NewtonModelTabFactoryDetail::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<NewtonModelEditor> editor = m_editor.Pin();
	FPropertyEditorModule& propertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));

	FDetailsViewArgs detailArgumnets;

	detailArgumnets.bLockable = false;
	detailArgumnets.bShowOptions = true;
	detailArgumnets.bAllowSearch = false;
	detailArgumnets.bShowScrollBar = true;
	detailArgumnets.bHideSelectionTip = false;
	detailArgumnets.bSearchInitialKeyFocus = true;
	detailArgumnets.bUpdatesFromSelection = false;
	detailArgumnets.bShowModifiedPropertiesOption = false;
	detailArgumnets.NotifyHook = nullptr;

	TSharedPtr<IDetailsView> detailView(propertyEditorModule.CreateDetailView(detailArgumnets));
	detailView->SetObject(editor->m_newtonModel);

#if 0
	// this is by far, the worse c++ programing style I have ever seen.
	// it is like some one when out of his way to obfuscate the code.
	TSharedRef<SWidget> widget
	(
		SNew(SVerticalBox) + SVerticalBox::Slot().FillHeight(1.0f).HAlign(HAlign_Fill)[detailView.ToSharedRef()]
	);
#else

	//const FText msg(FText::FromString(TEXT("this is a lot more complicated that it needed to be.")));
	//TSharedRef<STextBlock> editorRef (SNew(STextBlock));
	//editorRef->SetText(msg);
	TSharedRef<SWidget> editorRef(detailView.ToSharedRef());

	SVerticalBox::FSlot::FSlotArguments arguments(SVerticalBox::Slot());
	arguments.FillHeight(1.0);
	arguments.HAlign(HAlign_Fill)[editorRef];
	TSharedRef<SWidget> widget(SNew(SVerticalBox) + arguments);
#endif

	return widget;
}
