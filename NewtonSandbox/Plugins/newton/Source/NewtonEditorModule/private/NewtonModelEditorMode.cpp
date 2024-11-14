// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelEditorMode.h"

#include "NewtonModelEditor.h"
#include "NewtonModelTabFactoryDetail.h"

FName NewtonModelEditorMode::m_editorModelName(TEXT("NewtonModelMode"));
FName NewtonModelEditorMode::m_editorVersionName(TEXT("NewtonModelModeLayout_v01"));

NewtonModelEditorMode::NewtonModelEditorMode(TSharedPtr<NewtonModelEditor> editor)
	:FApplicationMode(m_editorModelName)
	,m_editor(editor)
{
	m_tabs.RegisterFactory(MakeShareable(new NewtonModelTabFactoryDetail(editor)));

	TabLayout = FTabManager::NewLayout(m_editorVersionName)
	->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewStack()->AddTab(NewtonModelTabFactoryDetail::m_primaryTabName, ETabState::OpenedTab)
		)
	);
}

NewtonModelEditorMode::~NewtonModelEditorMode()
{
}

void NewtonModelEditorMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}

void NewtonModelEditorMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void NewtonModelEditorMode::RegisterTabFactories(TSharedPtr<FTabManager> inTabManager)
{
	TSharedPtr<NewtonModelEditor> editor (m_editor.Pin());
	editor->PushTabFactories(m_tabs);
	FApplicationMode::RegisterTabFactories(inTabManager);
}
