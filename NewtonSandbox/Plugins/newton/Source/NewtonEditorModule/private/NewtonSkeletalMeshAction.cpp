// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonSkeletalMeshAction.h"

#include "NewtonSkeletalMesh.h"
#include "NewtonSkeletalMeshEditor.h"

NewtonSkeletalMeshAction::NewtonSkeletalMeshAction(EAssetTypeCategories::Type assetCategory)
	:FAssetTypeActions_Base()
	,m_assetCategory(assetCategory)
{
}

NewtonSkeletalMeshAction::~NewtonSkeletalMeshAction()
{
}

FText NewtonSkeletalMeshAction::GetName() const
{
	//return FText::FromString(TEXT("NewtonSkeletonMesh"));
	return FText::FromString(TEXT("ActionName"));
}

uint32 NewtonSkeletalMeshAction::GetCategories()
{
	return m_assetCategory;
}

FColor NewtonSkeletalMeshAction::GetTypeColor() const
{
	return FColor(255, 255, 255, 255);
}

UClass* NewtonSkeletalMeshAction::GetSupportedClass() const
{
	return UNewtonSkeletalMesh::StaticClass();
}

void NewtonSkeletalMeshAction::GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section)
{

}

void NewtonSkeletalMeshAction::OpenAssetEditor(const TArray<UObject*>& inObjects, TSharedPtr<class IToolkitHost> editWithinLevelEditor)
{
	EToolkitMode::Type mode = editWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (int i = 0; i < inObjects.Num(); ++i)
	{
		UNewtonSkeletalMesh* const mesh = Cast<UNewtonSkeletalMesh>(inObjects[i]);
		if (mesh)
		{
			TSharedRef<NewtonSkeletalMeshEditor> editor(new NewtonSkeletalMeshEditor());
			editor->InitEditor(mode, editWithinLevelEditor, mesh);
		}
	}
}

