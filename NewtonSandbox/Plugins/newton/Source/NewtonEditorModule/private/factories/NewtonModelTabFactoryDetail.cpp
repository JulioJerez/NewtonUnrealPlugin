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

#include "factories/NewtonModelTabFactoryDetail.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "PropertyEditorDelegates.h"

#include "NewtonModelEditor.h"
#include "NewtonModelEditorCommon.h"

#define DETAIL_IDENTIFIER TEXT("NewtonModelTabDetail")
//FName NewtonModelTabFactoryDetail::m_tabName("NewtonModelTabDetail");

NewtonModelTabFactoryDetail::NewtonModelTabFactoryDetail(const TSharedPtr<FNewtonModelEditor>& editor)
	:FWorkflowTabFactory(DETAIL_IDENTIFIER, editor)
	,m_editor(editor)
{
	// I prefer "Properties", but unreal uses "Detail" for this tab
	TabLabel = FText::FromString(TEXT("Details"));
	ViewMenuDescription = FText::FromString(TEXT("Newton Model Detail"));
	ViewMenuTooltip = FText::FromString(TEXT("Show Newton Model Detail"));
}

NewtonModelTabFactoryDetail::~NewtonModelTabFactoryDetail()
{
}

FText NewtonModelTabFactoryDetail::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	const FText name(FText::FromString(TEXT("NewtonModel detail view")));
	return name;
}

TSharedRef<SWidget> NewtonModelTabFactoryDetail::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
#if 0
	return TSharedRef<SWidget> 
	(
		SNew(SVerticalBox) +
		SVerticalBox::Slot().FillHeight(1.0f).HAlign(HAlign_Fill)
		[SNew(STextBlock).Text(FText::FromString(TEXT("xxxxx")))]
	);
#else
	TSharedPtr<FNewtonModelEditor> editor = m_editor.Pin();
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
	detailView->SetObject(editor->GetNewtonModel());
	FOnFinishedChangingProperties& propertyChange = detailView->OnFinishedChangingProperties();
	propertyChange.AddSP(editor.Get(), &FNewtonModelEditor::OnFinishedChangingProperties);

	TSharedPtr<IDetailsView> seletedNodeDetailView(propertyEditorModule.CreateDetailView(detailArgumnets));
	seletedNodeDetailView->SetObject(nullptr);
	editor->SetSelectedNodeDetailView(seletedNodeDetailView);

	SVerticalBox::FSlot::FSlotArguments editorArguments(SVerticalBox::Slot());
	editorArguments.FillHeight(0.25f);
	editorArguments.HAlign(HAlign_Fill)[detailView.ToSharedRef()];

	SVerticalBox::FSlot::FSlotArguments selectedNodeDetailViewArguments(SVerticalBox::Slot());
	selectedNodeDetailViewArguments.FillHeight(.75f);
	selectedNodeDetailViewArguments.HAlign(HAlign_Fill)[seletedNodeDetailView.ToSharedRef()];

	TSharedRef<SWidget> widget(SNew(SVerticalBox) + 
		editorArguments + selectedNodeDetailViewArguments);

	return widget;
#endif
}
