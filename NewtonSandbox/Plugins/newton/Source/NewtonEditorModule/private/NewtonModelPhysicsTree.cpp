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

#include "SSimpleButton.h"
#include "ISkeletonTree.h"
#include "SPrimaryButton.h"
#include "ISkeletonTreeItem.h"
#include "IEditableSkeleton.h"
#include "UObject/SavePackage.h"
#include "SPositiveActionButton.h"
#include "UICommandList_Pinnable.h"
#include "Widgets/Views/STableRow.h"
#include "Animation/DebugSkelMeshComponent.h"

#include "NewtonModelEditor.h"
#include "IPinnedCommandList.h"
#include "NewtonEditorModule.h"
#include "NewtonModelEditorCommon.h"
#include "NewtonModelPhysicsTreeItem.h"
#include "NewtonModelPhysicsTreeItemRow.h"
#include "NewtonModelPhysicsTreeCommands.h"
#include "NewtonModelPhysicsTreeItemBody.h"
#include "NewtonModelPhysicsTreeItemShapeBox.h"
#include "NewtonModelPhysicsTreeItemShapeWheel.h"
#include "NewtonModelPhysicsTreeItemJointHinge.h"
#include "NewtonModelPhysicsTreeItemShapeSphere.h"
#include "NewtonModelPhysicsTreeItemShapeCapsule.h"
#include "NewtonModelPhysicsTreeItemShapeCylinder.h"
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
	m_editor = nullptr;
	m_acyclicGraph = nullptr;
	m_selectedItem = nullptr;
	m_boneMappingMode = false;
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

	DetailViewBoneSelectedUpdated(m_editor->m_selectedBone);
}

void FNewtonModelPhysicsTree::OnResetSelectedBone()
{
	if (m_selectedItem.IsValid() && m_selectedItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")))
	{
		int stack = 1;
		FNewtonModelPhysicsTreeItemAcyclicGraph* nodeStack[TREE_STACK_DEPTH];
		nodeStack[0] = m_selectedItem->m_acyclicGraph;
		while (stack)
		{
			stack--;
			FNewtonModelPhysicsTreeItemAcyclicGraph* const node = nodeStack[stack];

			UNewtonLinkRigidBody* const bodyNode = Cast<UNewtonLinkRigidBody>(node->m_item->Node);
			if (bodyNode)
			{
				bodyNode->BoneIndex = -1;
				bodyNode->BoneName = TEXT("None");
			}

			for (int i = node->m_children.Num() - 1; i >= 0; --i)
			{
				nodeStack[stack] = node->m_children[i];
				stack++;
			}
		}

		m_treeView->RebuildList();
		RefreshView();
		m_editor->m_skeletonTree->DeselectAll();
	}
}

void FNewtonModelPhysicsTree::OnToggleShapeVisibility()
{
	UNewtonAsset* const model = m_editor->GetNewtonModel();
	model->m_hideShapes = !model->m_hideShapes;

	for (TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>>::TConstIterator it(m_items); it; ++it)
	{
		TSharedPtr<FNewtonModelPhysicsTreeItem> item (*it);
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

	for (TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>>::TConstIterator it(m_items); it; ++it)
	{
		TSharedPtr<FNewtonModelPhysicsTreeItem> item(*it);
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

void FNewtonModelPhysicsTree::AddShapeRow(const TSharedRef<FNewtonModelPhysicsTreeItem>& shapeItem)
{
	m_items.Add(shapeItem);
	new FNewtonModelPhysicsTreeItemAcyclicGraph(shapeItem, m_selectedItem->m_acyclicGraph);

	check(shapeItem->m_parent->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")));
	check(shapeItem->m_parent->Node->Transform.GetScale3D().X == 1.0f);
	check(shapeItem->m_parent->Node->Transform.GetScale3D().Y == 1.0f);
	check(shapeItem->m_parent->Node->Transform.GetScale3D().Z == 1.0f);
	shapeItem->Node->Transform = FTransform();

	RefreshView();
}

void FNewtonModelPhysicsTree::AddJointRow(const TSharedRef<FNewtonModelPhysicsTreeItem>& jointItem)
{
	m_items.Add(jointItem);
	FNewtonModelPhysicsTreeItemAcyclicGraph* const jointAcyclic = new FNewtonModelPhysicsTreeItemAcyclicGraph(jointItem, m_selectedItem->m_acyclicGraph);
	
	TSharedRef<FNewtonModelPhysicsTreeItem> bodyItem(MakeShareable(new FNewtonModelPhysicsTreeItemBody(jointItem, TObjectPtr<UNewtonLink>(NewObject<UNewtonLinkRigidBody>()))));
	bodyItem->Node = NewObject<UNewtonLinkRigidBody>();
	bodyItem->Node->Name = m_uniqueNames.GetUniqueName(bodyItem->GetDisplayName());
	m_items.Add(bodyItem);
	new FNewtonModelPhysicsTreeItemAcyclicGraph(bodyItem, jointAcyclic);
	
	RefreshView();
}

void FNewtonModelPhysicsTree::OnAddShapeBoxRow()
{
	TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShapeBox(m_selectedItem, TObjectPtr<UNewtonLink>(NewObject<UNewtonLinkCollisionBox>()))));
	item->Node->Name = m_uniqueNames.GetUniqueName(item->GetDisplayName());
	AddShapeRow(item);
}

void FNewtonModelPhysicsTree::OnAddShapeWheelRow()
{
	TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShapeWheel(m_selectedItem, TObjectPtr<UNewtonLink>(NewObject<UNewtonLinkCollisionWheel>()))));
	item->Node->Name = m_uniqueNames.GetUniqueName(item->GetDisplayName());
	AddShapeRow(item);
}

void FNewtonModelPhysicsTree::OnAddShapeSphereRow()
{
	TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShapeSphere(m_selectedItem, TObjectPtr<UNewtonLink>(NewObject<UNewtonLinkCollisionSphere>()))));
	item->Node->Name = m_uniqueNames.GetUniqueName(item->GetDisplayName());
	AddShapeRow(item);
}

void FNewtonModelPhysicsTree::OnAddShapeCapsuleRow()
{
	TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShapeCapsule(m_selectedItem, TObjectPtr<UNewtonLink>(NewObject<UNewtonLinkCollisionCapsule>()))));
	item->Node->Name = m_uniqueNames.GetUniqueName(item->GetDisplayName());
	AddShapeRow(item);
}

void FNewtonModelPhysicsTree::OnAddShapeCylinderRow()
{
	TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShapeCylinder(m_selectedItem, TObjectPtr<UNewtonLink>(NewObject<UNewtonLinkCollisionCylinder>()))));
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
		//m_items.FindAndRemoveChecked(node->m_item);
		m_items.Remove(node->m_item);

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

	// add physics tree building actions
	commandList.MapAction(menuActions.AddShapeBox
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnAddShapeBoxRow)
		,FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCanAddChildRow));

	commandList.MapAction(menuActions.AddShapeSphere
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnAddShapeSphereRow)
		,FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCanAddChildRow));

	commandList.MapAction(menuActions.AddShapeCapsule
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnAddShapeCapsuleRow)
		,FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCanAddChildRow));

	commandList.MapAction(menuActions.AddShapeCylinder
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnAddShapeCylinderRow)
		,FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCanAddChildRow));

	commandList.MapAction(menuActions.AddShapeWheel
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnAddShapeWheelRow)
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
		MenuBuilder.AddMenuEntry(actions.AddShapeCapsule);
		MenuBuilder.AddMenuEntry(actions.AddShapeCylinder);
		MenuBuilder.AddMenuEntry(actions.AddShapeWheel);
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


	FNewtonEditorModule& module = FModuleManager::GetModuleChecked<FNewtonEditorModule>(TEXT("NewtonEditorModule"));
	const FSlateBrush* const boneMappingBrush = module.GetBrush("boneMapping.png");

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

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(FMargin(6.f, 0.0))
				[
					//SNew(SSimpleButton)
					//SNew(SPrimaryButton)
					SNew(SPositiveActionButton)
					.OnClicked_Lambda([this]() {m_boneMappingMode = !m_boneMappingMode; return FReply::Handled(); })
					.Icon(boneMappingBrush)
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(FMargin(6.f, 0.0))
				[
					SNew(SPrimaryButton)
					.IsEnabled_Lambda([this]() {return m_boneMappingMode; })
					.OnClicked_Lambda([this]() {return FReply::Handled(); })
				]
			]

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
		for (TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>>::TConstIterator it(m_items); it; ++it)
		{
			m_treeView->SetItemExpansion(*it, true);
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
		for (TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>>::TConstIterator it(m_items); it; ++it)
		{
			TSharedPtr<FNewtonModelPhysicsTreeItem> item(*it);
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

void FNewtonModelPhysicsTree::FreezeBoneScale()
{
	int stack = 1;
	FVector scalePool[TREE_STACK_DEPTH];
	TSharedPtr<FNewtonModelPhysicsTreeItem> stackPool[TREE_STACK_DEPTH];

	stackPool[0] = m_root[0];
	scalePool[0] = FVector (1.0f, 1.0f, 1.0f);
	
	while (stack)
	{
		stack--;
		FVector scale(scalePool[stack]);
		TSharedPtr<FNewtonModelPhysicsTreeItem> node(stackPool[stack]);

		node->Node->Transform.SetLocation(node->Node->Transform.GetLocation() * scale);
		if (Cast<UNewtonLinkRigidBody>(node->Node))
		{
			scale = scale * node->Node->Transform.GetScale3D();
			node->Node->Transform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
		}
		else if (Cast<UNewtonLinkJointHinge>(node->Node))
		{
			scale = scale * node->Node->Transform.GetScale3D();
			node->Node->Transform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
		}
		else if (Cast<UNewtonLinkCollision>(node->Node))
		{
			node->Node->Transform.SetScale3D(node->Node->Transform.GetScale3D() * scale);
			scale = FVector(1.0f, 1.0f, 1.0f);
		}
		else
		{
			check(0);
		}

		for (int i = 0; i < node->m_acyclicGraph->m_children.Num(); ++i)
		{
			scalePool[stack] = scale;
			stackPool[stack] = node->m_acyclicGraph->m_children[i]->m_item;
			stack++;
		}
	}
}

void FNewtonModelPhysicsTree::DetailViewBoneSelectedUpdated(const TSharedPtr<ISkeletonTreeItem>& item)
{
	if (!m_boneMappingMode)
	{
		return;
	}

	if (!item.IsValid())
	{
		return;
	}

	if (!m_selectedItem.IsValid())
	{
		return;
	}

	if (!m_selectedItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")))
	{
		return;
	}

	if (Cast<UNewtonLinkRigidBody>(m_selectedItem->Node)->BoneIndex >= 0)
	{
		return;
	}

	if (m_selectedItem->m_parent)
	{
		check(m_selectedItem->m_parent->m_parent);
		UNewtonLinkRigidBody* const parentNodyNode = Cast<UNewtonLinkRigidBody>(m_selectedItem->m_parent->m_parent->Node);
		check(parentNodyNode);
		if (parentNodyNode->BoneIndex < 0)
		{
			return;
		}
	}

	auto FindBoneInfo = [this](const TSharedPtr<ISkeletonTreeItem>& item)
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
				int bodyCount = 1;
				int boneCount = 0;
				int boneGroup[512];
				int bodySequence[512];
				int boneSequence[512];

				bodySequence[0] = boneIndex;
				for (TSharedPtr<FNewtonModelPhysicsTreeItem> parentBody(m_selectedItem->m_parent ? m_selectedItem->m_parent->m_parent : nullptr);
					parentBody.IsValid(); parentBody = parentBody->m_parent ? parentBody->m_parent->m_parent : nullptr)
				{
					UNewtonLinkRigidBody* const node = Cast<UNewtonLinkRigidBody>(parentBody->Node);
					check(node);
					bodySequence[bodyCount] = node->BoneIndex;
					bodyCount++;
					check(bodyCount < sizeof(bodySequence) / sizeof(bodySequence[0]));
				}

				for (int parentBoneIndex = boneIndex; parentBoneIndex >= 0; parentBoneIndex = bonesInfo[parentBoneIndex].ParentIndex)
				{
					boneGroup[boneCount] = 1;
					for (int j = bodyCount; j >= 0; --j)
					{
						if (parentBoneIndex == bodySequence[j])
						{
							boneGroup[boneCount] = 0;
						}
					}
					boneSequence[boneCount] = parentBoneIndex;
					boneCount++;
					check(boneCount < sizeof(boneSequence) / sizeof(boneSequence[0]));
				}

				for (int i = 1; i < boneCount; ++i)
				{
					int j = i;
					int value = boneGroup[i];
					int bone = boneSequence[i];
					for (; (j > 0) && (boneGroup[j - 1] > value); --j)
					{
						boneGroup[j] = boneGroup[j - 1];
						boneSequence[j] = boneSequence[j - 1];
					}
					boneGroup[j] = value;
					boneSequence[j] = bone;
				}

				for (int i = 0; i < bodyCount; ++i)
				{
					if (boneSequence[i] != bodySequence[i])
					{
						return -1;
					}
				}

				for (TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>>::TConstIterator it(m_items); it; ++it)
				{
					TSharedPtr<FNewtonModelPhysicsTreeItem> itemInSet(*it);
					if (itemInSet->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody")))
					{
						UNewtonLinkRigidBody* const node = Cast<UNewtonLinkRigidBody>(itemInSet->Node);
						check(node);
						if (node->BoneIndex == boneIndex)
						{
							boneIndex = -1;
							break;
						}
					}
				}

				return boneIndex;
			}
		}
		return -1;
	};

	int boneIndex = FindBoneInfo(item);
	if (boneIndex >= 0)
	{
		UDebugSkelMeshComponent* const meshComponent = m_editor->GetSkelMeshComponent();
		UNewtonLinkRigidBody* const bodyNodeInfo = Cast<UNewtonLinkRigidBody>(m_selectedItem->Node);

		bodyNodeInfo->BoneIndex = boneIndex;
		bodyNodeInfo->BoneName = item->GetAttachName();
		bodyNodeInfo->Name = m_uniqueNames.GetUniqueName(bodyNodeInfo->BoneName);
		const FTransform boneTM(meshComponent->GetBoneTransform(boneIndex));
		if (m_selectedItem->m_parent)
		{
			const FTransform parentTransform(m_selectedItem->m_parent->m_parent->CalculateGlobalTransform());
			m_selectedItem->m_parent->Node->Transform = boneTM * parentTransform.Inverse();
			bodyNodeInfo->Transform = FTransform();
		}
		else
		{
			bodyNodeInfo->Transform = boneTM;
		}

		FreezeBoneScale();
		m_treeView->RebuildList();
		RefreshView();
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
	else
	{
		m_selectedItem->OnPropertyChange(event);
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
			m_items.Add(item);
			parent = item;
		}
		else if (Cast<UNewtonLinkRigidBody>(node))
		{
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemBody(parent, proxyNode)));
			m_items.Add(item);
			parent = item;
		}
		else if (Cast<UNewtonLinkJointHinge>(node))
		{
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemJointHinge(parent, proxyNode)));
			m_items.Add(item);
			parent = item;
		}
		else if (Cast<UNewtonLinkCollisionBox>(node))
		{
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShapeBox(parent, proxyNode)));
			m_items.Add(item);
			parent = item;
		}
		else if (Cast<UNewtonLinkCollisionSphere>(node))
		{
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShapeSphere(parent, proxyNode)));
			m_items.Add(item);
			parent = item;
		}
		else if (Cast<UNewtonLinkCollisionCapsule>(node))
		{
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShapeCapsule(parent, proxyNode)));
			m_items.Add(item);
			parent = item;
		}
		else if (Cast<UNewtonLinkCollisionCylinder>(node))
		{
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShapeCylinder(parent, proxyNode)));
			m_items.Add(item);
			parent = item;
		}

		else if (Cast<UNewtonLinkCollisionWheel>(node))
		{
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShapeWheel(parent, proxyNode)));
			m_items.Add(item);
			parent = item;
		}
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
		//const FTransform transform(m_selectedItem->CalculateGlobalTransform());
		//matrix = transform.ToMatrixNoScale();
		matrix = m_selectedItem->GetWidgetMatrix();
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