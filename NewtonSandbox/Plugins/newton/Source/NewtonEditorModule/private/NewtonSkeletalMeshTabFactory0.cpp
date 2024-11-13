// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonSkeletalMeshTabFactory0.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"

#include "NewtonSkeletalMesh.h"
#include "NewtonSkeletalMeshEditor.h"

FName NewtonSkeletalMeshTabFactory0::m_primaryTabName("NewtonSkeletalMeshTab0");

NewtonSkeletalMeshTabFactory0::NewtonSkeletalMeshTabFactory0(const TSharedPtr<NewtonSkeletalMeshEditor>& editor)
	:FWorkflowTabFactory(m_primaryTabName, editor)
	,m_editor(editor)
{
	TabLabel = FText::FromString(TEXT("NewtonSkeletalMesh0"));
	ViewMenuDescription = FText::FromString(TEXT("Newton Skeletal Mesh Main Tab"));
	ViewMenuTooltip = FText::FromString(TEXT("Show Newton Skeletal Mesh Main Tab"));
}

NewtonSkeletalMeshTabFactory0::~NewtonSkeletalMeshTabFactory0()
{
}

FText NewtonSkeletalMeshTabFactory0::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	const FText name(FText::FromString(TEXT("edit a Newton Skeletal Mesh component")));
	return name;
}

TSharedRef<SWidget> NewtonSkeletalMeshTabFactory0::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<NewtonSkeletalMeshEditor> editor = m_editor.Pin();
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
	detailView->SetObject(editor->m_skeletalMesh);

	return SNew(SVerticalBox) + SVerticalBox::Slot().FillHeight(1.0f).HAlign(HAlign_Fill)[detailView.ToSharedRef()];
	//return SNew (STextBlock).Text(FText::FromString(TEXT("this a test newton skeletal mesh editor")));
}
