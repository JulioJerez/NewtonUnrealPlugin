// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonSkeletalMeshEditorMode.h"

#include "NewtonSkeletalMeshEditor.h"
#include "NewtonSkeletalMeshTabFactory0.h"


FName NewtonSkeletalMeshEditorMode::m_editorModeName(TEXT("ndSkeletalMeshMode0"));
FName NewtonSkeletalMeshEditorMode::m_editorVersionName(TEXT("ndSkeletalMeshLayout_V01"));


NewtonSkeletalMeshEditorMode::NewtonSkeletalMeshEditorMode(TSharedPtr<NewtonSkeletalMeshEditor> editor)
	:FApplicationMode(m_editorModeName)
	,m_editor(editor)
{
	m_tabs.RegisterFactory(MakeShareable(new NewtonSkeletalMeshTabFactory0(editor)));

	TabLayout = FTabManager::NewLayout(m_editorVersionName)
	->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewStack()->AddTab(NewtonSkeletalMeshTabFactory0::m_primaryTabName, ETabState::OpenedTab)
		)
	);
}

NewtonSkeletalMeshEditorMode::~NewtonSkeletalMeshEditorMode()
{
}

void NewtonSkeletalMeshEditorMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}

void NewtonSkeletalMeshEditorMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void NewtonSkeletalMeshEditorMode::RegisterTabFactories(TSharedPtr<FTabManager> inTabManager)
{
	TSharedPtr<NewtonSkeletalMeshEditor> editor (m_editor.Pin());
	editor->PushTabFactories(m_tabs);
	FApplicationMode::RegisterTabFactories(inTabManager);
}
