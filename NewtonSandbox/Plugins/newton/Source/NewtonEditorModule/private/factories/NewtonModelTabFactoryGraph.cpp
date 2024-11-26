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

#include "factories/NewtonModelTabFactoryGraph.h"
#include "GraphEditor.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"

#include "NewtonModel.h"
#include "NewtonModelEditor.h"
#include "NewtonModelEditorCommon.h"

#define GRAPH_IDENTIFIER TEXT("NewtonModelTabGraph")

NewtonModelTabFactoryGraph::NewtonModelTabFactoryGraph(const TSharedPtr<FNewtonModelEditor>& editor)
	:FWorkflowTabFactory(GRAPH_IDENTIFIER, editor)
	,m_editor(editor)
{
	TabLabel = FText::FromString(TEXT("Graph"));
	ViewMenuDescription = FText::FromString(TEXT("NewtonModel Graph"));
	ViewMenuTooltip = FText::FromString(TEXT("Show NewtonModel Graph"));
}

NewtonModelTabFactoryGraph::~NewtonModelTabFactoryGraph()
{
}

FText NewtonModelTabFactoryGraph::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	const FText name(FText::FromString(TEXT("NewtonModel Graph view")));
	return name;
}

TSharedRef<SWidget> NewtonModelTabFactoryGraph::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
#ifdef ND_INCLUDE_GRAPH_EDITOR
	TSharedPtr<FNewtonModelEditor> editor (m_editor.Pin());
	
	SGraphEditor::FGraphEditorEvents graphEvents;
	graphEvents.OnSelectionChanged.BindRaw(editor.Get(), &FNewtonModelEditor::OnGraphSelectionChanged);
	
	TSharedPtr<SGraphEditor> editorGraph(
		SNew(SGraphEditor)
		.IsEditable(true)
		.GraphEvents(graphEvents)
		.GraphToEdit(editor->GetGraphEditor())
	);
	editor->SetWorkingGraphUi(editorGraph);
	
	SVerticalBox::FSlot::FSlotArguments editorGraphArg (SVerticalBox::Slot());
	editorGraphArg.FillHeight(1.0);
	editorGraphArg.HAlign(HAlign_Fill)[editorGraph.ToSharedRef()];
	TSharedRef<SWidget> widget(SNew(SVerticalBox) + editorGraphArg);
	
	return widget;
#else
	//check(0);
	return TSharedRef<SWidget>
	(
		SNew(SVerticalBox) +
		SVerticalBox::Slot().FillHeight(1.0f).HAlign(HAlign_Fill)
		[SNew(STextBlock).Text(FText::FromString(TEXT("dummyGraph")))]
	);
#endif
}
