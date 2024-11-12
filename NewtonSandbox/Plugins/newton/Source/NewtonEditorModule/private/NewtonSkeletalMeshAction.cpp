// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonSkeletalMeshAction.h"

#include "NewtonSkeletalMesh.h"

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
	return FText::FromString("NewtonSkeletonMesh");
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

void NewtonSkeletalMeshAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{

}

