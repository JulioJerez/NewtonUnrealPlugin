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
#include "PersonaModule.h"
#include "IPersonaToolkit.h"

#include "NewtonModelEditor.h"
#include "NewtonModelEditorCommon.h"
#include "NewtonModelTabFactoryDetail.h"
#include "NewtonModelPhysicsTreeTabFactory.h"

FName NewtonModelEditorMode::m_editorModelName(TEXT("NewtonModelMode"));
FName NewtonModelEditorMode::m_editorVersionName(TEXT("NewtonModelModeLayout_v1.1"));

NewtonModelEditorMode::NewtonModelEditorMode(TSharedRef<FWorkflowCentricApplication> editorInterface, TSharedRef<ISkeletonTree> skeletonTree)
	:FApplicationMode(m_editorModelName)
	, m_editor(editorInterface)
{
	TSharedRef<FNewtonModelEditor> editor(StaticCastSharedRef<FNewtonModelEditor>(editorInterface));

	FPersonaModule& personaModule = FModuleManager::LoadModuleChecked<FPersonaModule>("Persona");
	ISkeletonEditorModule& skeletonEditorModule = FModuleManager::LoadModuleChecked<ISkeletonEditorModule>("SkeletonEditor");

	FPersonaViewportArgs viewportArgs(editor->GetPersonaToolkit()->GetPreviewScene());
	viewportArgs.bShowStats = false;
	viewportArgs.bShowTurnTable = false;
	viewportArgs.bAlwaysShowTransformToolbar = true;
	viewportArgs.ContextName = TEXT("NewtonModelEditor.Viewport");
	viewportArgs.OnViewportCreated = FOnViewportCreated::CreateSP(editor, &FNewtonModelEditor::OnViewportCreated);

	TSharedRef<FWorkflowTabFactory> viewportTab(personaModule.CreatePersonaViewportTabFactory(editor, viewportArgs));
	TSharedRef<FWorkflowTabFactory> detailTab(MakeShareable(new NewtonModelTabFactoryDetail(editor)));
	TSharedRef<FWorkflowTabFactory> physicsTreeTab(MakeShareable(new FNewtonModelPhysicsTreeTabFactory(editor)));
	TSharedRef<FWorkflowTabFactory> skeletalTreeTab(skeletonEditorModule.CreateSkeletonTreeTabFactory(editor, skeletonTree));

	m_tabs.RegisterFactory(viewportTab);
	m_tabs.RegisterFactory(detailTab);
	m_tabs.RegisterFactory(physicsTreeTab);
	m_tabs.RegisterFactory(skeletalTreeTab);

	auto MakeArea = [](FName identifier)
	{
		TSharedRef<FTabManager::FSplitter> area(FTabManager::NewSplitter());
		area->SetOrientation(Orient_Vertical);
		{
			TSharedRef<FTabManager::FStack> stack(FTabManager::NewStack());
			stack->SetSizeCoefficient(1.0f);
			stack->AddTab(identifier, ETabState::OpenedTab);
			area->Split(stack);
		}
		return area;
	};

	TSharedRef<FTabManager::FSplitter> viewportArea(MakeArea(TEXT("Viewport")));
	TSharedRef<FTabManager::FSplitter> detailArea(MakeArea(detailTab->GetIdentifier()));

	TSharedRef<FTabManager::FSplitter> explorerArea(FTabManager::NewSplitter());
	{
		explorerArea->SetOrientation(Orient_Vertical);
		TSharedRef<FTabManager::FStack> stack(FTabManager::NewStack());
		stack->SetSizeCoefficient(1.0f);

		TSharedRef<FTabManager::FSplitter> physicsTreeArea(MakeArea(physicsTreeTab->GetIdentifier()));
		physicsTreeArea->SetSizeCoefficient(0.6f);
		explorerArea->Split(physicsTreeArea);

		TSharedRef<FTabManager::FSplitter> skeletonArea(MakeArea(skeletalTreeTab->GetIdentifier()));
		skeletonArea->SetSizeCoefficient(0.4f);
		explorerArea->Split(skeletonArea);
	}

	TSharedRef<FTabManager::FArea> mainArea(FTabManager::NewPrimaryArea());
	mainArea->SetOrientation(Orient_Horizontal);
	{
		explorerArea->SetSizeCoefficient(.2f);
		mainArea->Split(explorerArea);

		viewportArea->SetSizeCoefficient(0.6f);
		mainArea->Split(viewportArea);

		detailArea->SetSizeCoefficient(.2f);
		mainArea->Split(detailArea);
	}

	TabLayout = FTabManager::NewLayout(m_editorVersionName);
	TabLayout->AddArea(mainArea);

	personaModule.OnRegisterTabs().Broadcast(m_tabs, editorInterface);
	LayoutExtender = MakeShared<FLayoutExtender>();
	personaModule.OnRegisterLayoutExtensions().Broadcast(*LayoutExtender.Get());
	TabLayout->ProcessExtensions(*LayoutExtender.Get());

	editor->RegisterModeToolbarIfUnregistered(GetModeName());
}

NewtonModelEditorMode::~NewtonModelEditorMode()
{
}

void CreateeLayout()
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
	TSharedPtr<FWorkflowCentricApplication> editor (m_editor.Pin());
	editor->PushTabFactories(m_tabs);

	FApplicationMode::RegisterTabFactories(inTabManager);
}
