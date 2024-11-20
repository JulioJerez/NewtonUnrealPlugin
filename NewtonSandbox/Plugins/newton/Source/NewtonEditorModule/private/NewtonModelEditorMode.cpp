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


#include "NewtonModelEditorMode.h"

#include "ISkeletonEditorModule.h"

#include "NewtonModelEditor.h"
#include "NewtonModelTabFactoryGraph.h"
#include "NewtonModelTabFactoryDetail.h"

FName NewtonModelEditorMode::m_editorModelName(TEXT("NewtonModelMode"));
FName NewtonModelEditorMode::m_editorVersionName(TEXT("NewtonModelModeLayout_v19"));

NewtonModelEditorMode::NewtonModelEditorMode(TSharedRef<FNewtonModelEditor> editor, TSharedRef<ISkeletonTree> skeletonTree)
	:FApplicationMode(m_editorModelName)
	,m_editor(editor)
{
	ISkeletonEditorModule& skeletonEditorModule = FModuleManager::LoadModuleChecked<ISkeletonEditorModule>("SkeletonEditor");

	TSharedRef<FWorkflowTabFactory> graphTab(MakeShareable(new NewtonModelTabFactoryGraph(editor)));
	TSharedRef<FWorkflowTabFactory> detailTab(MakeShareable(new NewtonModelTabFactoryDetail(editor)));
	TSharedRef<FWorkflowTabFactory> skeletalTreeTab(skeletonEditorModule.CreateSkeletonTreeTabFactory(editor, skeletonTree));

	m_tabs.RegisterFactory(graphTab);
	m_tabs.RegisterFactory(detailTab);
	m_tabs.RegisterFactory(skeletalTreeTab);

	TSharedRef<FTabManager::FSplitter> skeletonArea(FTabManager::NewSplitter());
	skeletonArea->SetOrientation(Orient_Vertical);
	{
		TSharedRef<FTabManager::FStack> stack(FTabManager::NewStack());
		stack->AddTab(skeletalTreeTab->GetIdentifier(), ETabState::OpenedTab);
		stack->SetSizeCoefficient(1.0f);
		skeletonArea->Split(stack);
	}

	TSharedRef<FTabManager::FSplitter> detailArea(FTabManager::NewSplitter());
	detailArea->SetOrientation(Orient_Vertical);
	{
		TSharedRef<FTabManager::FStack> stack(FTabManager::NewStack());
		stack->SetSizeCoefficient(1.0f);
		stack->AddTab(detailTab->GetIdentifier(), ETabState::OpenedTab);
		detailArea->Split(stack);
	}

	TSharedRef<FTabManager::FSplitter> graphArea(FTabManager::NewSplitter());
	detailArea->SetOrientation(Orient_Vertical);
	{
		TSharedRef<FTabManager::FStack> stack(FTabManager::NewStack());
		stack->SetSizeCoefficient(1.0f);
		stack->AddTab(graphTab->GetIdentifier(), ETabState::OpenedTab);
		graphArea->Split(stack);
	}

	TSharedRef<FTabManager::FArea> mainArea(FTabManager::NewPrimaryArea());
	mainArea->SetOrientation(Orient_Horizontal);
	{
		skeletonArea->SetSizeCoefficient(.2f);
		mainArea->Split(skeletonArea);

		graphArea->SetSizeCoefficient(0.6f);
		mainArea->Split(graphArea);

		detailArea->SetSizeCoefficient(.2f);
		mainArea->Split(detailArea);
	}

	TabLayout = FTabManager::NewLayout(m_editorVersionName);
	TabLayout->AddArea(mainArea);

	//skeletalMeshEditor->RegisterModeToolbarIfUnregistered(GetModeName());
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
	TSharedPtr<FNewtonModelEditor> editor (m_editor.Pin());
	editor->PushTabFactories(m_tabs);
	FApplicationMode::RegisterTabFactories(inTabManager);
}
