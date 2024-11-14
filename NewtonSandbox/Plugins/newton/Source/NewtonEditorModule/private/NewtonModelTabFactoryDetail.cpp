// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelTabFactoryDetail.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"

#include "NewtonModel.h"
#include "NewtonModelEditor.h"

FName NewtonModelTabFactoryDetail::m_primaryTabName("NewtonModelTab0");

NewtonModelTabFactoryDetail::NewtonModelTabFactoryDetail(const TSharedPtr<NewtonModelEditor>& editor)
	:FWorkflowTabFactory(m_primaryTabName, editor)
	,m_editor(editor)
{
	TabLabel = FText::FromString(TEXT("Details"));
	ViewMenuDescription = FText::FromString(TEXT("Newton Model Detail"));
	ViewMenuTooltip = FText::FromString(TEXT("Show Newton Model Detail"));
}

NewtonModelTabFactoryDetail::~NewtonModelTabFactoryDetail()
{
}

FText NewtonModelTabFactoryDetail::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	const FText name(FText::FromString(TEXT("Newton Model Component Detail")));
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

	return SNew(SVerticalBox) + SVerticalBox::Slot().FillHeight(1.0f).HAlign(HAlign_Fill)[detailView.ToSharedRef()];
	//return SNew (STextBlock).Text(FText::FromString(TEXT("this a test newton skeletal mesh editor")));
}
