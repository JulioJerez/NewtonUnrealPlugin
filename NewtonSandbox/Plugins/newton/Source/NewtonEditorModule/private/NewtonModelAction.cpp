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

