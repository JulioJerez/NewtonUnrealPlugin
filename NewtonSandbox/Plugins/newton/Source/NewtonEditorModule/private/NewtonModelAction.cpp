// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelAction.h"

#include "NewtonModel.h"
#include "NewtonModelEditor.h"

NewtonModelAction::NewtonModelAction(EAssetTypeCategories::Type assetCategory)
	:FAssetTypeActions_Base()
	,m_assetCategory(assetCategory)
{
}

NewtonModelAction::~NewtonModelAction()
{
}

FText NewtonModelAction::GetName() const
{
	return FText::FromString(TEXT("ActionName"));
}

uint32 NewtonModelAction::GetCategories()
{
	return m_assetCategory;
}

FColor NewtonModelAction::GetTypeColor() const
{
	return FColor(255, 255, 255, 255);
}

UClass* NewtonModelAction::GetSupportedClass() const
{
	return UNewtonModel::StaticClass();
}

void NewtonModelAction::GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section)
{

}

void NewtonModelAction::OpenAssetEditor(const TArray<UObject*>& inObjects, TSharedPtr<class IToolkitHost> editWithinLevelEditor)
{
	EToolkitMode::Type mode = editWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (int i = 0; i < inObjects.Num(); ++i)
	{
		UNewtonModel* const mesh = Cast<UNewtonModel>(inObjects[i]);
		if (mesh)
		{
			TSharedRef<NewtonModelEditor> editor(new NewtonModelEditor());
			editor->InitEditor(mode, editWithinLevelEditor, mesh);
		}
	}
}

