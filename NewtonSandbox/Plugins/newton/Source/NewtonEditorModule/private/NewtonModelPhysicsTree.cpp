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

#include "NewtonModelPhysicsTree.h"

#include "ISkeletonTree.h"
#include "ISkeletonTreeItem.h"
#include "IEditableSkeleton.h"
#include "UObject/SavePackage.h"
#include "SPositiveActionButton.h"
#include "UICommandList_Pinnable.h"
#include "Widgets/Views/STableRow.h"
#include "Animation/DebugSkelMeshComponent.h"

#include "NewtonModelEditor.h"
#include "IPinnedCommandList.h"
#include "NewtonModelEditorCommon.h"
#include "NewtonModelPhysicsTreeItem.h"
#include "NewtonModelPhysicsTreeItemRow.h"
#include "NewtonModelPhysicsTreeCommands.h"
#include "NewtonModelPhysicsTreeItemBody.h"
#include "NewtonModelPhysicsTreeItemShapeBox.h"
#include "NewtonModelPhysicsTreeItemJointHinge.h"
#include "NewtonModelPhysicsTreeItemShapeSphere.h"
#include "NewtonModelPhysicsTreeItemAcyclicGraphs.h"

#define LOCTEXT_NAMESPACE "FNewtonModelPhysicsTree"

FName FNewtonModelPhysicsTree::m_treeColumnName(TEXT("node name"));
FName FNewtonModelPhysicsTree::m_menuName(TEXT("NewtonModelPhysicsTreeMenu"));
FName FNewtonModelPhysicsTree::m_contextName(TEXT("NewtonModelPhysicsTreeMenuItem"));


FNewtonModelPhysicsTree::UniqueNameId::UniqueNameId()
	:TSet<FName>()
	,m_enumerator(0)
{
}

void FNewtonModelPhysicsTree::UniqueNameId::Reset()
{
	Empty();
	m_enumerator = 0;
}

FName FNewtonModelPhysicsTree::UniqueNameId::GetUniqueName(const FName name)
{
	FString nodeName(name.ToString());
	while (Find(FName(*nodeName)))
	{
		m_enumerator++;
		nodeName = name.ToString() + "_" + FString::FromInt(m_enumerator);
	}
	Add(FName(*nodeName));
	return FName(*nodeName);
}


FNewtonModelPhysicsTree::FNewtonModelPhysicsTree()
{
	m_acyclicGraph = nullptr;
	m_editor = nullptr;
}

FNewtonModelPhysicsTree::~FNewtonModelPhysicsTree()
{
	m_selectedItem = nullptr;
	if (m_acyclicGraph)
	{
		delete m_acyclicGraph;
	}
	m_items.Empty();
}

void FNewtonModelPhysicsTree::Tick(const FGeometry& geometry, const double currentTime, const float deltaTime)
{
	SCompoundWidget::Tick(geometry, currentTime, deltaTime);
}

TSharedRef<ITableRow> FNewtonModelPhysicsTree::OnGenerateRow(TSharedPtr<FNewtonModelPhysicsTreeItem> item, const TSharedRef<STableViewBase>& ownerTable)
{
	check(item.IsValid());

#if 0
	return SNew(STableRow<TSharedPtr<FNewtonModelPhysicsTreeItem>>, ownerTable)
	[
		SNew(STextBlock)
		.Text(FText::FromName(item->GetDisplayName()))
		.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 10))
		.ColorAndOpacity(FLinearColor(0.8, .8, .8, 1))
		.ShadowColorAndOpacity(FLinearColor::Black)
		.ShadowOffset(FIntPoint(-2, 2))
	];
#else
	return SNew(SNewtonModelPhysicsTreeItemRow, ownerTable)
		.m_item(item);
#endif
}

TSharedRef< SWidget > FNewtonModelPhysicsTree::OnCreateNewMenuWidget()
{
	FToolMenuContext menuContext(UICommandList);
	return UToolMenus::Get()->GenerateWidget(m_menuName, menuContext);
}

void FNewtonModelPhysicsTree::OnGetChildren(TSharedPtr<FNewtonModelPhysicsTreeItem> item, TArray<TSharedPtr<FNewtonModelPhysicsTreeItem>>& outChildren)
{
	check(m_acyclicGraph);
	check(item->m_acyclicGraph);

	const FNewtonModelPhysicsTreeItemAcyclicGraph* const root = item->m_acyclicGraph;
	for (int i = 0; i < root->m_children.Num(); ++i)
	{
		TSharedPtr<FNewtonModelPhysicsTreeItem> childItem (root->m_children[i]->m_item);
		if (!childItem->Node->m_hidden)
		{
			outChildren.Push(childItem);
		}
		else if (childItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemJoint")))
		{
			const FNewtonModelPhysicsTreeItemAcyclicGraph* const childItemNode = childItem->m_acyclicGraph;
			check(childItemNode->m_children.Num() == 1);
			outChildren.Push(childItemNode->m_children[0]->m_item);
		}
	}
}

void FNewtonModelPhysicsTree::OnSelectionChanged(TSharedPtr<FNewtonModelPhysicsTreeItem> item, ESelectInfo::Type selectInfo)
{
	m_selectedItem = item;
	TSharedPtr<IDetailsView> detailView(m_editor->m_selectedNodeDetailView);
	check(detailView.IsValid());
	if (m_selectedItem.IsValid())
	{
		m_oldSelectedName = m_selectedItem->GetDisplayName();
		detailView->SetObject(item->Node);
	}
	else
	{
		m_oldSelectedName = FName();
		detailView->SetObject(nullptr);
	}
}

void FNewtonModelPhysicsTree::OnResetSelectedBone()
{
	if (m_selectedItem.IsValid() && m_selectedItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")))
	{
		UNewtonLinkRigidBody* const bodyNode = Cast<UNewtonLinkRigidBody>(m_selectedItem->Node);
		bodyNode->BoneIndex = -1;
		bodyNode->BoneName = TEXT("None");
	}
}

void FNewtonModelPhysicsTree::OnToggleShapeVisibility()
{
	UNewtonAsset* const model = m_editor->GetNewtonModel();
	model->m_hideShapes = !model->m_hideShapes;

	for (const TPair<FNewtonModelPhysicsTreeItem*, TSharedPtr<FNewtonModelPhysicsTreeItem>>& pair : m_items)
	{
		TSharedPtr<FNewtonModelPhysicsTreeItem> item (pair.Value);
		if (item->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemShape")))
		{
			item->Node->m_hidden = model->m_hideShapes;
		}
	}
	RefreshView();
}

void FNewtonModelPhysicsTree::OnToggleJointVisibility()
{
	UNewtonAsset* const model = m_editor->GetNewtonModel();
	model->m_hideJoints = !model->m_hideJoints;

	for (const TPair<FNewtonModelPhysicsTreeItem*, TSharedPtr<FNewtonModelPhysicsTreeItem>>& pair : m_items)
	{
		TSharedPtr<FNewtonModelPhysicsTreeItem> item(pair.Value);
		if (item->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemJoint")))
		{
			item->Node->m_hidden = model->m_hideJoints;
		}
	}
	RefreshView();
}

bool FNewtonModelPhysicsTree::OnCanAddChildRow() const
{
	return m_selectedItem.IsValid() && m_selectedItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody"));
}

void FNewtonModelPhysicsTree::AddShapeRow(const TSharedRef<FNewtonModelPhysicsTreeItem>& item)
{
	m_items.Add(&item.Get(), item);
	new FNewtonModelPhysicsTreeItemAcyclicGraph(item, m_selectedItem->m_acyclicGraph);

	UNewtonLinkCollision* const shapeNodeInfo = Cast<UNewtonLinkCollision>(item->Node);
	check(shapeNodeInfo);
	check(item->m_parent->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")));
	UNewtonLinkRigidBody* const bodyNodeInfo = Cast<UNewtonLinkRigidBody>(item->m_parent->Node);
	check(bodyNodeInfo);
	shapeNodeInfo->Transform = bodyNodeInfo->Transform;
	shapeNodeInfo->Transform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

	RefreshView();
}

void FNewtonModelPhysicsTree::AddJointRow(const TSharedRef<FNewtonModelPhysicsTreeItem>& jointItem)
{
	m_items.Add(&jointItem.Get(), jointItem);
	FNewtonModelPhysicsTreeItemAcyclicGraph* const jointAcyclic = new FNewtonModelPhysicsTreeItemAcyclicGraph(jointItem, m_selectedItem->m_acyclicGraph);

	TSharedRef<FNewtonModelPhysicsTreeItem> bodyItem(MakeShareable(new FNewtonModelPhysicsTreeItemBody(jointItem, TObjectPtr<UNewtonLink>(NewObject<UNewtonLinkRigidBody>()))));
	bodyItem->Node = NewObject<UNewtonLinkRigidBody>();
	bodyItem->Node->Name = m_uniqueNames.GetUniqueName(bodyItem->GetDisplayName());
	m_items.Add(&bodyItem.Get(), bodyItem);
	new FNewtonModelPhysicsTreeItemAcyclicGraph(bodyItem, jointAcyclic);
	
	RefreshView();
}

void FNewtonModelPhysicsTree::OnAddShapeBoxRow()
{
	TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShapeBox(m_selectedItem, TObjectPtr<UNewtonLink>(NewObject<UNewtonLinkCollisionBox>()))));
	item->Node->Name = m_uniqueNames.GetUniqueName(item->GetDisplayName());
	
	AddShapeRow(item);
}

void FNewtonModelPhysicsTree::OnAddShapeSphereRow()
{
	TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShapeSphere(m_selectedItem, TObjectPtr<UNewtonLink>(NewObject<UNewtonLinkCollisionSphere>()))));
	item->Node->Name = m_uniqueNames.GetUniqueName(item->GetDisplayName());
	AddShapeRow(item);
}

void FNewtonModelPhysicsTree::OnAddJointHingeRow()
{
	TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemJointHinge(m_selectedItem, TObjectPtr<UNewtonLink>(NewObject<UNewtonLinkJointHinge>()))));
	item->Node->Name = m_uniqueNames.GetUniqueName(item->GetDisplayName());
	AddJointRow(item);
}

bool FNewtonModelPhysicsTree::CanDeleteSelectedRow() const
{
	//UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
	return m_selectedItem.IsValid() && !m_selectedItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody"));
}

void FNewtonModelPhysicsTree::OnDeleteSelectedRow()
{
	int stack = 1;
	FNewtonModelPhysicsTreeItemAcyclicGraph* nodeStack[TREE_STACK_DEPTH];

	check(m_selectedItem.IsValid());
	check(!m_selectedItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")));

	nodeStack[0] = m_selectedItem->m_acyclicGraph;
	while (stack)
	{
		stack--;
		FNewtonModelPhysicsTreeItemAcyclicGraph* const node = nodeStack[stack];

		m_uniqueNames.Remove(node->m_item->GetDisplayName());
		m_items.FindAndRemoveChecked(node->m_item.Get());

		for (int i = node->m_children.Num() - 1; i >= 0; --i)
		{
			nodeStack[stack] = node->m_children[i];
			stack++;
		}
	}

	m_selectedItem->m_acyclicGraph->m_parent->RemoveChild(m_selectedItem->m_acyclicGraph);
	delete m_selectedItem->m_acyclicGraph;

	RefreshView();
}

void FNewtonModelPhysicsTree::BindCommands()
{
	// This should not be called twice on the same instance
	check(!UICommandList.IsValid());

	UICommandList = MakeShareable(new FUICommandList_Pinnable);

	FUICommandList_Pinnable& commandList = *UICommandList;

	// Grab the list of menu commands to bind...
	const FNewtonModelPhysicsTreeCommands& menuActions = FNewtonModelPhysicsTreeCommands::Get();

	// add the tree options actions
	commandList.MapAction(menuActions.CollisionsVisibility
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnToggleShapeVisibility)
		,FCanExecuteAction()
		,FIsActionChecked::CreateLambda([this]() { return m_editor->GetNewtonModel()->m_hideShapes; }));
		//,FIsActionChecked::CreateSP(this, &FNewtonModelPhysicsTree::OnGetShapeVisibleRowsState));

	commandList.MapAction(menuActions.JointsVisibility
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnToggleJointVisibility)
		,FCanExecuteAction()
		,FIsActionChecked::CreateLambda([this]() { return m_editor->GetNewtonModel()->m_hideJoints; }));

	commandList.MapAction(menuActions.ResetSelectedBone
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnResetSelectedBone));

	// add ohysics tree building actions
	commandList.MapAction(menuActions.AddShapeBox
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnAddShapeBoxRow)
		,FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCanAddChildRow));

	commandList.MapAction(menuActions.AddShapeSphere
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnAddShapeSphereRow)
		,FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCanAddChildRow));

	commandList.MapAction(menuActions.AddJointHinge
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnAddJointHingeRow)
		,FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCanAddChildRow));

	// delete any node action
	commandList.MapAction(menuActions.DeleteSelectedRow 
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnDeleteSelectedRow)
		,FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::CanDeleteSelectedRow));

	PinnedCommands->BindCommandList(UICommandList.ToSharedRef());
}

void FNewtonModelPhysicsTree::RegisterNewMenu()
{
	if (UToolMenus::Get()->IsMenuRegistered(m_menuName))
	{
		return;
	}

	//FToolMenuOwnerScoped OwnerScoped(this);
	const FNewtonModelPhysicsTreeCommands& menuActions = FNewtonModelPhysicsTreeCommands::Get();

	UToolMenu* const menu = UToolMenus::Get()->RegisterMenu(m_menuName);

	//MenuBuilder.BeginSection("NewtonModelPhysicsTreeOptions", LOCTEXT("Visibility", "Visibility options"));
	//	MenuBuilder.AddMenuEntry(actions.HideJoints);
	//	MenuBuilder.AddMenuEntry(actions.HideCollisions);
	//MenuBuilder.EndSection();

	FToolMenuSection& rootSection = menu->AddSection("options0", LOCTEXT("options1", "options2"));
	rootSection.AddMenuEntry(menuActions.ResetSelectedBone);
	rootSection.AddMenuEntry(menuActions.JointsVisibility);
	rootSection.AddMenuEntry(menuActions.CollisionsVisibility);
}

TSharedPtr< SWidget > FNewtonModelPhysicsTree::CreateContextMenu()
{
	FMenuBuilder MenuBuilder(true, UICommandList);
	const FNewtonModelPhysicsTreeCommands& actions = FNewtonModelPhysicsTreeCommands::Get();

	MenuBuilder.BeginSection("NewtonModelPhysicsTreeAddJOints", LOCTEXT("AddJointsActions", "Add joints"));
		MenuBuilder.AddMenuEntry(actions.AddJointHinge);
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("NewtonModelPhysicsTreeAddShape", LOCTEXT("AddShapeActions", "Add collision shape"));
		MenuBuilder.AddMenuEntry(actions.AddShapeBox);
		MenuBuilder.AddMenuEntry(actions.AddShapeSphere);
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("NewtonModelPhysicsTreeDeleteItems", LOCTEXT("DeleteItemAction", "Delete items"));
		MenuBuilder.AddMenuEntry(actions.DeleteSelectedRow);
	MenuBuilder.EndSection();
	
	return MenuBuilder.MakeWidget();
}

void FNewtonModelPhysicsTree::Construct(const FArguments& args, FNewtonModelEditor* const editor)
{
	m_editor = editor;

	// Register and bind all our menu commands
	// Create our pinned commands before we bind commands
	IPinnedCommandListModule& PinnedCommandListModule = FModuleManager::LoadModuleChecked<IPinnedCommandListModule>(TEXT("PinnedCommandList"));
	PinnedCommands = PinnedCommandListModule.CreatePinnedCommandList(m_contextName);

	FNewtonModelPhysicsTreeCommands::Register();
	BindCommands();
	RegisterNewMenu();

	TArray<FName> HiddenColumnsList;
	TSharedRef<SHeaderRow> treeHeaderRow
	(
		SNew(SHeaderRow)
		.CanSelectGeneratedColumn(true)
		.HiddenColumnsList(HiddenColumnsList)

		+ SHeaderRow::Column(m_treeColumnName)
		.ShouldGenerateWidget(true)
		.DefaultLabel(LOCTEXT("NodeNameLabel", "Name"))
		.FillWidth(0.5f)

		//+ SHeaderRow::Column(ISkeletonTree::Columns::Retargeting)
		//.DefaultLabel(LOCTEXT("SkeletonBoneTranslationRetargetingLabel", "Translation Retargeting"))
		//.FillWidth(0.25f)
		//
		//+ SHeaderRow::Column(ISkeletonTree::Columns::DebugVisualization)
		//.DefaultLabel(LOCTEXT("SkeletonBoneDebugVisualizationLabel", "Debug"))
		//.FillWidth(0.25f)
		//
		//+ SHeaderRow::Column(ISkeletonTree::Columns::BlendProfile)
		//.DefaultLabel(LOCTEXT("BlendProfile", "Blend Profile"))
		//.FillWidth(0.25f)
		//.OnGetMenuContent(this, &FNewtonModelPhysicsTree::GetBlendProfileColumnMenuContent)
		//.HeaderContent()
		//[
		//	SNew(SBox)
		//	.HeightOverride(24.f)
		//	.HAlign(HAlign_Left)
		//	[
		//		SAssignNew(BlendProfileHeader, SInlineEditableTextBlock)
		//		.Text_Lambda([this]() -> FText
		//		{
		//			FName CurrentProfile = BlendProfilePicker->GetSelectedBlendProfileName();
		//			return (CurrentProfile != NAME_None) ? FText::FromName(CurrentProfile) : LOCTEXT("NoBlendProfile", "NoBlend");
		//		})
		//		.OnTextCommitted_Lambda([this](const FText& InText, ETextCommit::Type InCommitType)
		//		{
		//			if (bIsCreateNewBlendProfile)
		//			{
		//				BlendProfilePicker->OnCreateNewProfileComitted(InText, InCommitType, NewBlendProfileMode);
		//				bIsCreateNewBlendProfile = false;
		//			}
		//			else if (BlendProfilePicker->GetSelectedBlendProfileName() != NAME_None)
		//			{
		//				if (UBlendProfile* Profile = EditableSkeleton.Pin()->RenameBlendProfile(BlendProfilePicker->GetSelectedBlendProfileName(), FName(InText.ToString())))
		//				{
		//					BlendProfilePicker->SetSelectedProfile(Profile);
		//				}
		//			}
		//		})
		//		.OnVerifyTextChanged_Lambda([](const FText& InNewText, FText& OutErrorMessage) -> bool
		//		{
		//			return FName::IsValidXName(InNewText.ToString(), INVALID_OBJECTNAME_CHARACTERS INVALID_LONGPACKAGE_CHARACTERS, &OutErrorMessage);
		//		})
		//		.IsReadOnly(true)
		//	]
		//]
	);

	m_treeView =
		SNew(STreeView<TSharedPtr<FNewtonModelPhysicsTreeItem>>)

		// For now we only support selecting a single folder in the tree
		.SelectionMode(ESelectionMode::Single)
		.ClearSelectionOnClick(false)		// Don't allow user to select nothing.
		.TreeItemsSource(&m_root)
		.OnGenerateRow(this, &FNewtonModelPhysicsTree::OnGenerateRow)
		.OnGetChildren(this, &FNewtonModelPhysicsTree::OnGetChildren)
		.OnSelectionChanged(this, &FNewtonModelPhysicsTree::OnSelectionChanged)
		.OnContextMenuOpening(this, &FNewtonModelPhysicsTree::CreateContextMenu)
		.HeaderRow(treeHeaderRow)
	;

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			// Add a border if we are being used as a picker
			SNew(SBorder)
				//.Visibility_Lambda([this]() { return Mode == ESkeletonTreeMode::Picker ? EVisibility::Visible : EVisibility::Collapsed; })
				//.BorderImage(FAppStyle::Get().GetBrush("Menu.Background"))
		]
		+ SOverlay::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0.f, 2.f))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(FMargin(6.f, 0.0))
				[
					SNew(SPositiveActionButton)
					.OnGetMenuContent(this, &FNewtonModelPhysicsTree::OnCreateNewMenuWidget)
					.Icon(FAppStyle::Get().GetBrush("Icons.Plus"))
				]
			]

			//+ SVerticalBox::Slot()
			//.Padding(FMargin(0.0f, 2.0f, 0.0f, 0.0f))
			//.AutoHeight()
			//[
			//	PinnedCommands.ToSharedRef()
			//]

			+SVerticalBox::Slot()
			.Padding(FMargin(0.0f, 2.0f, 0.0f, 0.0f))
			[
				m_treeView.ToSharedRef()
			]
		]
	];

	BuildTree();
}

void FNewtonModelPhysicsTree::RefreshView()
{
	if (m_treeView.IsValid())
	{
		for (const TPair<FNewtonModelPhysicsTreeItem*, TSharedPtr<FNewtonModelPhysicsTreeItem>>& pair : m_items)
		{
			m_treeView->SetItemExpansion(pair.Value, true);
		}

		m_treeView->RequestTreeRefresh();
	}
}

void FNewtonModelPhysicsTree::RebuildAcyclicTree()
{
	check(m_root.Num() == 1);
	check(m_root[0].IsValid());
	check(m_root[0]->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBodyRoot")));
	
	if (m_acyclicGraph)
	{
		delete m_acyclicGraph;
	}
	
	int stack = 1;
	FNewtonModelPhysicsTreeItemAcyclicGraph* stackPool[TREE_STACK_DEPTH];
	m_acyclicGraph = new FNewtonModelPhysicsTreeItemAcyclicGraph(m_root[0], nullptr);
	stackPool[0] = m_acyclicGraph;
	
	while (stack)
	{
		stack--;
		FNewtonModelPhysicsTreeItemAcyclicGraph* const node = stackPool[stack];
	
		const TSharedPtr<FNewtonModelPhysicsTreeItem> parent(node->m_item);
		for (const TPair<FNewtonModelPhysicsTreeItem*, TSharedPtr<FNewtonModelPhysicsTreeItem>>& pair : m_items)
		{
			TSharedPtr<FNewtonModelPhysicsTreeItem> item(pair.Value);
			if (item->m_parent == parent)
			{
				new FNewtonModelPhysicsTreeItemAcyclicGraph(item, node);
			}
		}
	
		for (int i = 0; i < node->m_children.Num(); ++i)
		{
			stackPool[stack] = node->m_children[i];
			stack++;
		}
	}

	RefreshView();
}

void FNewtonModelPhysicsTree::DetailViewBoneSelectedUpdated(const TSharedPtr<ISkeletonTreeItem>& item)
{
	if (m_selectedItem.IsValid() && m_selectedItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")))
	{
		UNewtonLinkRigidBody* const selectedNodyNode = Cast<UNewtonLinkRigidBody>(m_selectedItem->Node);
		check(selectedNodyNode);
		if (selectedNodyNode->BoneIndex >= 0)
		{
			return;
		}

		//const TSharedRef<IEditableSkeleton> editSkeleton(item->GetSkeletonTree()->GetEditableSkeleton());
		//const FReferenceSkeleton& referenceSkeleton = editSkeleton->GetSkeleton().GetReferenceSkeleton();
		//const TArray<FMeshBoneInfo>& bonesInfo = referenceSkeleton.GetRefBoneInfo();
		//
		//const FName boneName (item->GetAttachName());
		//for (int boneIndex = 0; boneIndex < bonesInfo.Num(); ++boneIndex)
		//{
		//	const FMeshBoneInfo& info = bonesInfo[boneIndex];
		//	if (info.Name == boneName)
		//	{
		//		for (const TPair<FNewtonModelPhysicsTreeItem*, TSharedPtr<FNewtonModelPhysicsTreeItem>>& pair : m_items)
		//		{
		//			TSharedPtr<FNewtonModelPhysicsTreeItem> bodyItem(pair.Value);
		//			if (bodyItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")))
		//			{
		//				UNewtonLinkRigidBody* const bodyNode = Cast<UNewtonLinkRigidBody>(bodyItem->Node);
		//				if (bodyNode->BoneIndex == boneIndex)
		//				{
		//					return;
		//				}
		//			}
		//		}
		//
		//		UDebugSkelMeshComponent* const meshComponent = m_editor->GetSkelMeshComponent();
		//		UNewtonLinkRigidBody* const bodyNodeInfo = Cast<UNewtonLinkRigidBody>(m_selectedItem->Node);
		//		FTransform boneTM(meshComponent->GetBoneTransform(boneIndex));
		//
		//		bodyNodeInfo->Transform = boneTM;
		//		bodyNodeInfo->BoneName = boneName;
		//		bodyNodeInfo->BoneIndex = boneIndex;
		//		if (m_selectedItem->m_acyclicGraph->m_parent)
		//		{
		//			TSharedPtr<FNewtonModelPhysicsTreeItem> parentItem(m_selectedItem->m_acyclicGraph->m_parent->m_item);
		//			check(parentItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemJoint")));
		//			UNewtonLinkJoint* const jointNodeInfo = Cast<UNewtonLinkJoint>(parentItem->Node);
		//			check(jointNodeInfo);
		//			jointNodeInfo->Transform = bodyNodeInfo->Transform;
		//			jointNodeInfo->Transform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
		//		}
		//
		//		for (int i = m_selectedItem->m_acyclicGraph->m_children.Num() - 1; i >= 0; --i)
		//		{
		//			TSharedPtr<FNewtonModelPhysicsTreeItem> childItem(m_selectedItem->m_acyclicGraph->m_children[i]->m_item);
		//			if (childItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemShape")))
		//			{
		//				UNewtonLinkCollision* const shapeNodeInfo = Cast<UNewtonLinkCollision>(childItem->Node);
		//				check(shapeNodeInfo);
		//				shapeNodeInfo->Transform = bodyNodeInfo->Transform;
		//				shapeNodeInfo->Transform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
		//			}
		//		}
		//		break;
		//	}
		//}

		auto FindBoneInfo = [](const TSharedPtr<ISkeletonTreeItem>& item)
		{
			const TSharedRef<IEditableSkeleton> editSkeleton(item->GetSkeletonTree()->GetEditableSkeleton());
			const FReferenceSkeleton& referenceSkeleton = editSkeleton->GetSkeleton().GetReferenceSkeleton();
			const TArray<FMeshBoneInfo>& bonesInfo = referenceSkeleton.GetRefBoneInfo();

			const FName boneName(item->GetAttachName());
			for (int boneIndex = 0; boneIndex < bonesInfo.Num(); ++boneIndex)
			{
				const FMeshBoneInfo& info = bonesInfo[boneIndex];
				if (info.Name == boneName)
				{
					return boneIndex;
				}
			}
			return -1;
		};

		int boneIndex = FindBoneInfo(item);
		if (boneIndex >= 0)
		{
			//auto IsBoneMapped = [this](int boneIndex)
			//{
			//	for (const TPair<FNewtonModelPhysicsTreeItem*, TSharedPtr<FNewtonModelPhysicsTreeItem>>& pair : m_items)
			//	{
			//		TSharedPtr<FNewtonModelPhysicsTreeItem> bodyItem(pair.Value);
			//		if (bodyItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")))
			//		{
			//			UNewtonLinkRigidBody* const bodyNode = Cast<UNewtonLinkRigidBody>(bodyItem->Node);
			//			if (bodyNode->BoneIndex == boneIndex)
			//			{
			//				return true;
			//			}
			//		}
			//	}
			//	return false;
			//};
			//
			//if (!IsBoneMapped(boneIndex))
			{
				UDebugSkelMeshComponent* const meshComponent = m_editor->GetSkelMeshComponent();
				UNewtonLinkRigidBody* const bodyNodeInfo = Cast<UNewtonLinkRigidBody>(m_selectedItem->Node);
				FTransform boneTM(meshComponent->GetBoneTransform(boneIndex));

				bodyNodeInfo->Transform = boneTM;
				bodyNodeInfo->BoneIndex = boneIndex;
				bodyNodeInfo->BoneName = item->GetAttachName();

				if (m_selectedItem->m_acyclicGraph->m_parent)
				{
					TSharedPtr<FNewtonModelPhysicsTreeItem> parentItem(m_selectedItem->m_acyclicGraph->m_parent->m_item);
					check(parentItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemJoint")));
					UNewtonLinkJoint* const jointNodeInfo = Cast<UNewtonLinkJoint>(parentItem->Node);
					check(jointNodeInfo);
					jointNodeInfo->Transform = bodyNodeInfo->Transform;
					jointNodeInfo->Transform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
				}
				
				for (int i = m_selectedItem->m_acyclicGraph->m_children.Num() - 1; i >= 0; --i)
				{
					TSharedPtr<FNewtonModelPhysicsTreeItem> childItem(m_selectedItem->m_acyclicGraph->m_children[i]->m_item);
					if (childItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemShape")))
					{
						UNewtonLinkCollision* const shapeNodeInfo = Cast<UNewtonLinkCollision>(childItem->Node);
						check(shapeNodeInfo);
						shapeNodeInfo->Transform = bodyNodeInfo->Transform;
						shapeNodeInfo->Transform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
					}
				}
			}
		}
	}
}

void FNewtonModelPhysicsTree::DetailViewPropertiesUpdated(const FPropertyChangedEvent& event)
{
	check(m_selectedItem.IsValid());
	FProperty* const property = event.Property;
	if (property->GetName() == TEXT("Name"))
	{
		m_uniqueNames.Remove(m_oldSelectedName);
		FName newName(m_uniqueNames.GetUniqueName(m_selectedItem->Node->Name));
		m_selectedItem->Node->SetName(*newName.ToString());
		
		m_treeView->RebuildList();
		m_treeView->RequestTreeRefresh();
	}
}

void FNewtonModelPhysicsTree::ResetSkeletalMesh()
{
	check(m_root[0].IsValid());

	if (m_acyclicGraph)
	{
		delete m_acyclicGraph;
	}
	m_acyclicGraph = nullptr;

	m_root.Empty();
	m_uniqueNames.Reset();
	m_oldSelectedName = TEXT("None");
	m_selectedItem = nullptr;
	m_items.Empty();

	UNewtonAsset* const model = m_editor->GetNewtonModel();
	model->RootBody = NewObject<UNewtonLinkRigidBodyRoot>(model);

	BuildTree();
}

void FNewtonModelPhysicsTree::BuildTree()
{
	check(m_items.IsEmpty());
	
	const UNewtonLink* nodeStack[TREE_STACK_DEPTH];
	TSharedPtr<FNewtonModelPhysicsTreeItem> parentStack[TREE_STACK_DEPTH];
	
	const UNewtonAsset* const model = m_editor->GetNewtonModel();
	if (!model->RootBody)
	{
		return;
	}
	
	check(Cast<UNewtonLinkRigidBodyRoot>(model->RootBody));
	
	int stack = 1;
	int labelId = 0;
	
	parentStack[0] = nullptr;
	nodeStack[0] = model->RootBody;
	
	while (stack)
	{
		stack--;
		const UNewtonLink* const node = nodeStack[stack];
		TSharedPtr<FNewtonModelPhysicsTreeItem> parent(parentStack[stack]);
	
		TObjectPtr<UNewtonLink> proxyNode(DuplicateObject(node, nullptr));
		check(!m_uniqueNames.Find(proxyNode->Name));
		m_uniqueNames.GetUniqueName(proxyNode->Name);

		if (Cast<UNewtonLinkRigidBodyRoot>(node))
		{
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemBodyRoot(nullptr, proxyNode)));
			m_root.Push(item);
			m_items.Add(&item.Get(), item);
			parent = item;
		}
		else if (Cast<UNewtonLinkRigidBody>(node))
		{
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemBody(parent, proxyNode)));
			m_items.Add(&item.Get(), item);
			parent = item;
		}
		else if (Cast<UNewtonLinkJointHinge>(node))
		{
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemJointHinge(parent, proxyNode)));
			m_items.Add(&item.Get(), item);
			parent = item;
		}
		else if (Cast<UNewtonLinkCollisionBox>(node))
		{
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShapeBox(parent, proxyNode)));
			m_items.Add(&item.Get(), item);
			parent = item;
		}
		else if (Cast<UNewtonLinkCollisionSphere>(node))
		{
			check(0);
			//TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShapeSphere(parent, Cast<UNewtonLinkCollisionSphere>(node))));
			// item->Node = proxyNode;
			//m_items.Add(&item.Get(), item);
			//parent = item;
		}
		//else if (Cast<UNewtonLinkJoint>(node))
		//{
		//	check(0);
		//	//TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemJoint(parent, node->Name)));
		//	//m_items.Add(&item.Get(), item);
		//	//parent = item;
		//}
		//else if (Cast<UNewtonLinkCollision>(node))
		//{
		//	check(0);
		//	//TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShape(parent, node->Name)));
		//	//m_items.Add(&item.Get(), item);
		//	//parent = item;
		//}
		else
		{
			check(0);
		}
	
		for (int i = 0; i < node->Children.Num(); ++i)
		{
			parentStack[stack] = parent;
			nodeStack[stack] = node->Children[i];
			stack++;
		}
	}
	
	RebuildAcyclicTree();
}

void FNewtonModelPhysicsTree::SaveModel()
{
	int stack = 1;
	UNewtonLink* parentNode[TREE_STACK_DEPTH];
	FNewtonModelPhysicsTreeItemAcyclicGraph* nodeStack[TREE_STACK_DEPTH];

	parentNode[0] = nullptr;
	nodeStack[0] = m_acyclicGraph;
	UNewtonAsset* const model = m_editor->GetNewtonModel();
	while (stack)
	{
		stack--;
		UNewtonLink* const parent = parentNode[stack];
		const FNewtonModelPhysicsTreeItemAcyclicGraph* const root = nodeStack[stack];
		const TSharedPtr<FNewtonModelPhysicsTreeItem>& item = root->m_item;

		UNewtonLink* modelNode = nullptr;
		if (Cast<UNewtonLinkRigidBodyRoot>(item->Node))
		{
			modelNode = DuplicateObject(item->Node, model);
			model->RootBody = Cast<UNewtonLinkRigidBodyRoot>(modelNode);
		}
		else
		{
			modelNode = DuplicateObject(item->Node, parent);
			modelNode->Parent = parent;
			parent->Children.Push(modelNode);
		}
		modelNode->Children.Empty();
		
		for (int i = 0; i < root->m_children.Num(); ++i)
		{
			parentNode[stack] = modelNode;
			nodeStack[stack] = root->m_children[i];
			stack++;
		}
	}

	UPackage* const package = model->GetPackage();
	const FString packageName (package->GetName());
	const FString packageFileName (FPackageName::LongPackageNameToFilename(packageName, FPackageName::GetAssetPackageExtension()));

	FSavePackageArgs saveArgs;
	saveArgs.SaveFlags = SAVE_NoError;
	saveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	UPackage::SavePackage(package, nullptr, *packageFileName, saveArgs);
}

void FNewtonModelPhysicsTree::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	check(m_acyclicGraph);

	FNewtonModelPhysicsTreeItemAcyclicGraph* nodeStack[TREE_STACK_DEPTH];

	int stack = 1;
	nodeStack[0] = m_acyclicGraph;
	while (stack)
	{
		stack--;
		const FNewtonModelPhysicsTreeItemAcyclicGraph* const node = nodeStack[stack];
		node->m_item->DebugDraw(view, viewport, pdi);

		for (int i = 0; i < node->m_children.Num(); ++i)
		{
			nodeStack[stack] = node->m_children[i];
			stack++;
		}
	}
}

bool FNewtonModelPhysicsTree::ShouldDrawWidget() const
{
	return m_selectedItem.IsValid() && m_selectedItem->ShouldDrawWidget();
}

FMatrix FNewtonModelPhysicsTree::GetWidgetMatrix() const
{
	FMatrix matrix(FMatrix::Identity);
	if (m_selectedItem.IsValid())
	{
		return m_selectedItem->GetWidgetMatrix();
	}
	return matrix;
}

void FNewtonModelPhysicsTree::ApplyDeltaTransform(const FVector& inDrag, const FRotator& inRot, const FVector& inScale) const
{
	if (m_selectedItem.IsValid())
	{
		m_selectedItem->ApplyDeltaTransform(inDrag, inRot, inScale);
	}
}

bool FNewtonModelPhysicsTree::HaveSelection() const
{
	return m_selectedItem.IsValid() && m_selectedItem->HaveSelection();
}

#undef LOCTEXT_NAMESPACE