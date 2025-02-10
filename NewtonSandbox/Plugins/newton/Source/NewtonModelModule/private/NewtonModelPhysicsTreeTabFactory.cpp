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


#include "NewtonModelPhysicsTreeTabFactory.h"

#include "NewtonModelEditor.h"
#include "NewtonModelEditorCommon.h"
#include "NewtonModelPhysicsTree.h"

#define LOCTEXT_NAMESPACE "NewtonModelPhysicsTreeTabFactory"
#define TREE_IDENTIFIER TEXT("NewtonModelTabPhysicsTree")

FNewtonModelPhysicsTreeTabFactory::FNewtonModelPhysicsTreeTabFactory(const TSharedPtr<FNewtonModelEditor>& editor)
	:FWorkflowTabFactory(TREE_IDENTIFIER, editor)
	,m_editor(editor)
{
	//EnableTabPadding();
	//bIsSingleton = true;

	TabLabel = FText::FromString(TEXT("Physics Tree"));
	ViewMenuDescription = FText::FromString(TEXT("NewtonModel physics tree"));
	ViewMenuTooltip = FText::FromString(TEXT("Show NewtonModel physics tree"));
	//TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Persona.Tabs.SkeletonTree");
}

FText FNewtonModelPhysicsTreeTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	const FText name(FText::FromString(TEXT("NewtonModel physics tree view")));
	return name;
}

//TSharedPtr<SToolTip> FNewtonModelPhysicsTreeTabFactory::CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const
//{
//	return IDocumentation::Get()->CreateToolTip(LOCTEXT("SkeletonTreeTooltip", "The Skeleton Tree tab lets you see and select bones (and sockets) in the skeleton hierarchy."), NULL, TEXT("Shared/Editors/Persona"), TEXT("SkeletonTree_Window"));
//}

TSharedRef<SWidget> FNewtonModelPhysicsTreeTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
#if 0
	return TSharedRef<SWidget>
		(
			SNew(SVerticalBox) +
			SVerticalBox::Slot().FillHeight(1.0f).HAlign(HAlign_Fill)
			[SNew(STextBlock).Text(FText::FromString(TEXT("Placeholder")))]
		);
#else

	TSharedPtr<FNewtonModelEditor> editor (m_editor.Pin());
	check(editor.IsValid());
	TSharedPtr<FNewtonModelPhysicsTree> physicsTree(editor->GetNewtonModelPhysicsTree());
	check(physicsTree.IsValid());




	return physicsTree.ToSharedRef();
#endif
}

#undef LOCTEXT_NAMESPACE
