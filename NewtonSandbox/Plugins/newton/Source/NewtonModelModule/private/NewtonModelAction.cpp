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


#include "NewtonModelAction.h"

#include "NewtonModelEditor.h"
#include "NewtonModelModule.h"
#include "NewtonModelEditorCommon.h"
#include "Rendering/SkeletalMeshRenderData.h"

FNewtonModelAction::FNewtonModelAction(EAssetTypeCategories::Type assetCategory)
	:FAssetTypeActions_Base()
	,m_assetCategory(assetCategory)
{
}

FNewtonModelAction::~FNewtonModelAction()
{
}

FText FNewtonModelAction::GetName() const
{
	//return FText::FromString(TEXT("ActionName"));
	return FText::FromString(ND_NEWTON_MODEL_DISPLAY_NAME);
}

uint32 FNewtonModelAction::GetCategories()
{
	return m_assetCategory;
}

FColor FNewtonModelAction::GetTypeColor() const
{
	return FColor(255, 255, 255, 255);
}

UClass* FNewtonModelAction::GetSupportedClass() const
{
	return UNewtonAsset::StaticClass();
}

void FNewtonModelAction::GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section)
{
}

void FNewtonModelAction::OpenAssetEditor(const TArray<UObject*>& inObjects, TSharedPtr<class IToolkitHost> editWithinLevelEditor)
{
	EToolkitMode::Type mode = editWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	
	for (int i = 0; i < inObjects.Num(); ++i)
	{
		UNewtonAsset* const newtonAsset = Cast<UNewtonAsset>(inObjects[i]);
		if (newtonAsset && newtonAsset->SkeletalMeshAsset)
		{
			FSkeletalMeshRenderData* const renderData = newtonAsset->SkeletalMeshAsset->GetResourceForRendering();
			if (renderData && (renderData->LODRenderData.Num() == 0))
			{
				newtonAsset->SkeletalMeshAsset = NewObject<USkeletalMesh>(newtonAsset, FName(TEXT("emptyMesh")));
				newtonAsset->SkeletalMeshAsset->SetSkeleton(NewObject<USkeleton>(newtonAsset->SkeletalMeshAsset, FName(TEXT("emptySkeleton"))));
			}
			TSharedRef<FNewtonModelEditor> editor(new FNewtonModelEditor());
			editor->InitEditor(mode, editWithinLevelEditor, newtonAsset);
			break;
		}
	}
}
