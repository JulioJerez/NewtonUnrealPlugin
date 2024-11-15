// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelEditor.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

#include "NewtonModel.h"
#include "NewtonModelEditorMode.h"
#include "NewtonModelGraphSchema.h"

NewtonModelEditor::NewtonModelEditor()
	:FWorkflowCentricApplication()
{
	m_graphEditor = nullptr;
	m_newtonModel = nullptr;
}

NewtonModelEditor::~NewtonModelEditor()
{
}

void NewtonModelEditor::InitEditor(const EToolkitMode::Type mode, const TSharedPtr< class IToolkitHost >& initToolkitHost, class UNewtonModel* const newtonModel)
{
	TArray<UObject*> objectsToEdit;

	m_newtonModel = newtonModel;
	objectsToEdit.Push(m_newtonModel);

	const FName graphName(TEXT("NewtonModelGraph"));
	//m_graphEditor = FBlueprintEditorUtils::CreateNewGraph(m_newtonModel, graphName, UEdGraph::StaticClass(), UEdGraphSchema::StaticClass());
	m_graphEditor = FBlueprintEditorUtils::CreateNewGraph(m_newtonModel, graphName, UEdGraph::StaticClass(), UNewtonModelGraphSchema::StaticClass());
	
	InitAssetEditor(mode, initToolkitHost, TEXT("NewtonModelEditor"), FTabManager::FLayout::NullLayout, true, true, objectsToEdit);

	AddApplicationMode(NewtonModelEditorMode::m_editorModelName, MakeShareable(new NewtonModelEditorMode(SharedThis(this))));
	SetCurrentMode(NewtonModelEditorMode::m_editorModelName);
}

FName NewtonModelEditor::GetToolkitFName() const
{
	const FName name(TEXT("NewtonModelEditor"));
	return name;
}

FText NewtonModelEditor::GetBaseToolkitName() const
{
	const FText name(FText::FromString("NewtonModelEditor"));
	return name;
}

FString NewtonModelEditor::GetWorldCentricTabPrefix() const
{
	return FString("nd");
}

FLinearColor NewtonModelEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(1.0f, 1.0f, 1.0f);
}

void NewtonModelEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& manager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(manager);
}

void NewtonModelEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& manager)
{
}

void NewtonModelEditor::OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit)
{
}

void NewtonModelEditor::OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit)
{
}
