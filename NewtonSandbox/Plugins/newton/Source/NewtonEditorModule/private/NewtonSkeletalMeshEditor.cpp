// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonSkeletalMeshEditor.h"

#include "NewtonSkeletalMesh.h"
#include "NewtonSkeletalMeshEditorMode.h"

NewtonSkeletalMeshEditor::NewtonSkeletalMeshEditor()
	:FWorkflowCentricApplication()
{
}

NewtonSkeletalMeshEditor::~NewtonSkeletalMeshEditor()
{
}

FName NewtonSkeletalMeshEditor::GetToolkitFName() const
{
	const FName name(TEXT("NewtonSkeletalMeshEditor"));
	return name;
}

FText NewtonSkeletalMeshEditor::GetBaseToolkitName() const
{
	const FText name(FText::FromString("NewtonSkeletalMeshEditor"));
	return name;
}

FString NewtonSkeletalMeshEditor::GetWorldCentricTabPrefix() const
{
	return FString("nd");
}

FLinearColor NewtonSkeletalMeshEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(1.0f, 1.0f, 1.0f);
}

void NewtonSkeletalMeshEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& manager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(manager);
}

void NewtonSkeletalMeshEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& manager)
{

}

void NewtonSkeletalMeshEditor::InitEditor(const EToolkitMode::Type mode, const TSharedPtr< class IToolkitHost >& initToolkitHost, class UNewtonSkeletalMesh* const mesh)
{
	TArray<UObject*> objectsToEdit;
	objectsToEdit.Push(mesh);
	InitAssetEditor(mode, initToolkitHost, TEXT("SkeletaMeshEditor"), FTabManager::FLayout::NullLayout, true, true, objectsToEdit);
	//InitAssetEditor(mode, initToolkitHost, TEXT("yyyyyyyyy"), FTabManager::FLayout::NullLayout, true, true, objectsToEdit);

	AddApplicationMode(NewtonSkeletalMeshEditorMode::m_editorModeName, MakeShareable(new NewtonSkeletalMeshEditorMode(SharedThis(this))));
	SetCurrentMode(NewtonSkeletalMeshEditorMode::m_editorModeName);
}

void NewtonSkeletalMeshEditor::OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit)
{
}

void NewtonSkeletalMeshEditor::OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit)
{
}
