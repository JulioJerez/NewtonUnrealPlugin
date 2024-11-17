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
	detailView->SetObject(editor->GetNewtonMode());

	TSharedPtr<IDetailsView> seletedNodeDetailView(propertyEditorModule.CreateDetailView(detailArgumnets));
	seletedNodeDetailView->SetObject(nullptr);
	editor->SetSelectedNodeDetailView(seletedNodeDetailView);

#if 0
	// this is by far, the worse c++ programing style I have ever seen.
	// it is like some one when out of his way to obfuscate the code.
	TSharedRef<SWidget> widget
	(
		SNew(SVerticalBox) + SVerticalBox::Slot().FillHeight(1.0f).HAlign(HAlign_Fill)[detailView.ToSharedRef()]
	);
#else

	SVerticalBox::FSlot::FSlotArguments editorArguments(SVerticalBox::Slot());
	editorArguments.FillHeight(0.25f);
	editorArguments.HAlign(HAlign_Fill)[detailView.ToSharedRef()];

	SVerticalBox::FSlot::FSlotArguments seletedNodeDetailViewArguments(SVerticalBox::Slot());
	seletedNodeDetailViewArguments.FillHeight(.75f);
	seletedNodeDetailViewArguments.HAlign(HAlign_Fill)[seletedNodeDetailView.ToSharedRef()];

	TSharedRef<SWidget> widget(SNew(SVerticalBox) + 
		editorArguments + seletedNodeDetailViewArguments);
#endif

	return widget;
}
