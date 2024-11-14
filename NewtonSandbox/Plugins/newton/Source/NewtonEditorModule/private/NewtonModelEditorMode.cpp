// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelEditorMode.h"

#include "NewtonModelEditor.h"
#include "NewtonModelTabFactoryGraph.h"
#include "NewtonModelTabFactoryDetail.h"

FName NewtonModelEditorMode::m_editorModelName(TEXT("NewtonModelMode"));
FName NewtonModelEditorMode::m_editorVersionName(TEXT("NewtonModelModeLayout_v00"));

NewtonModelEditorMode::NewtonModelEditorMode(TSharedPtr<NewtonModelEditor> editor)
	:FApplicationMode(m_editorModelName)
	,m_editor(editor)
{
	m_tabs.RegisterFactory(MakeShareable(new NewtonModelTabFactoryGraph(editor)));
	m_tabs.RegisterFactory(MakeShareable(new NewtonModelTabFactoryDetail(editor)));

#if 0
	// this is by far, the worse c++ programing style I have ever seen.
	// it is like some one when out of his way to obfuscate the code.

	TabLayout = FTabManager::NewLayout(m_editorVersionName)
	->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewStack()->AddTab(NewtonModelTabFactoryDetail::m_tabName, ETabState::OpenedTab)
		)
	);
#else

	TSharedRef<FTabManager::FStack> graphStack(FTabManager::NewStack());
	graphStack->AddTab(NewtonModelTabFactoryGraph::m_tabName, ETabState::OpenedTab);
	graphStack->SetSizeCoefficient(0.75f);

	TSharedRef<FTabManager::FStack> detailStack(FTabManager::NewStack());
	detailStack->AddTab(NewtonModelTabFactoryDetail::m_tabName, ETabState::OpenedTab);
	detailStack->SetSizeCoefficient(0.25f);

	TSharedRef<FTabManager::FArea> area(FTabManager::NewPrimaryArea());

	area->Split(graphStack);
	area->SetOrientation(Orient_Horizontal);

	area->Split(detailStack);
	area->SetOrientation(Orient_Horizontal);

	TabLayout = FTabManager::NewLayout(m_editorVersionName);
	TabLayout->AddArea(area);

#endif
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
