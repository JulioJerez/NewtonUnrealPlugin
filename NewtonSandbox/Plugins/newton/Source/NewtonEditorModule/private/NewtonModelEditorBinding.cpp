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

#include "NewtonModelEditorBinding.h"
#include "ISkeletonTree.h"
#include "Algo/Transform.h"
#include "ISkeletonTreeItem.h"


#include "NewtonModelEditor.h"


FNewtonModelEditorNotifier::FNewtonModelEditorNotifier(TSharedRef<FNewtonModelEditor> editor)
	:ISkeletalMeshNotifier()
	,Editor(editor)
{
}

void FNewtonModelEditorNotifier::HandleNotification(const TArray<FName>& BoneNames, const ESkeletalMeshNotifyType InNotifyType)
{
	if (Notifying() || !Editor.IsValid())
	{
		return;
	}

	check(0);
	//TSharedRef<ISkeletonTree> SkeletonTree = Editor.Pin()->GetSkeletonTree();
	//
	//switch (InNotifyType)
	//{
	//	case ESkeletalMeshNotifyType::BonesAdded:
	//		SkeletonTree->DeselectAll();
	//		SkeletonTree->Refresh();
	//		break;
	//	case ESkeletalMeshNotifyType::BonesRemoved:
	//		SkeletonTree->DeselectAll();
	//		SkeletonTree->Refresh();
	//		break;
	//	case ESkeletalMeshNotifyType::BonesMoved:
	//		break;
	//	case ESkeletalMeshNotifyType::BonesSelected:
	//		SkeletonTree->DeselectAll();
	//		if (!BoneNames.IsEmpty())
	//		{
	//			for (const FName BoneName : BoneNames)
	//			{
	//				SkeletonTree->SetSelectedBone(BoneName, ESelectInfo::Direct);
	//			}
	//		}
	//		break;
	//	case ESkeletalMeshNotifyType::BonesRenamed:
	//		SkeletonTree->DeselectAll();
	//		SkeletonTree->Refresh();
	//		if (!BoneNames.IsEmpty())
	//		{
	//			SkeletonTree->SetSelectedBone(BoneNames[0], ESelectInfo::Direct);
	//		}
	//		break;
	//	case ESkeletalMeshNotifyType::HierarchyChanged:
	//		SkeletonTree->Refresh();
	//		break;
	//}
}


FNewtonModelEditorBinding::FNewtonModelEditorBinding(TSharedRef<FNewtonModelEditor> InEditor)
	: Editor(InEditor)
	, Notifier(InEditor)
{}

ISkeletalMeshNotifier& FNewtonModelEditorBinding::GetNotifier()
{
	return Notifier;
}

ISkeletalMeshEditorBinding::NameFunction FNewtonModelEditorBinding::GetNameFunction()
{
	return [this](HHitProxy* InHitProxy) -> TOptional<FName>
	{
		check(0);
		//if (const HPersonaBoneHitProxy* BoneHitProxy = HitProxyCast<HPersonaBoneHitProxy>(InHitProxy))
		//{
		//	return BoneHitProxy->BoneName;
		//}

		static const TOptional<FName> Dummy;
		return Dummy;
	};
}

TArray<FName> FNewtonModelEditorBinding::GetSelectedBones() const
{
	TArray<FName> Selection;

	if (!Editor.IsValid())
	{
		return Selection;
	}

	const TArray<TSharedPtr<ISkeletonTreeItem>> SelectedItems = Editor.Pin()->GetSkeletonTree()->GetSelectedItems();

	Algo::TransformIf(SelectedItems, Selection,
		[](const TSharedPtr<ISkeletonTreeItem>& InItem) { return InItem->IsOfTypeByName("FSkeletonTreeBoneItem") && InItem->GetRowItemName() != NAME_None; },
		[](const TSharedPtr<ISkeletonTreeItem>& InItem) { return InItem->GetRowItemName(); });

	return Selection;
}
