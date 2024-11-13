// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"


/**
 * 
 */
class NEWTONEDITORMODULE_API NewtonSkeletalMeshAction: public FAssetTypeActions_Base
{
	public:
	NewtonSkeletalMeshAction(EAssetTypeCategories::Type assetCategory);
	virtual ~NewtonSkeletalMeshAction();

	virtual FText GetName() const override;
	virtual uint32 GetCategories() override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual void GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section) override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

	EAssetTypeCategories::Type m_assetCategory;
};
