// Fill out your copyright notice in the Description page of Project Settings.


#include "ndTree/NewtonModelPhysicsTree.h"

#define TREE_STACK_DEPTH	256


#include "SPositiveActionButton.h"
#include "UICommandList_Pinnable.h"
#include "Widgets/Views/STableRow.h"

#include "NewtonModelEditor.h"
#include "IPinnedCommandList.h"
#include "NewtonModelEditorCommon.h"
#include "ndTree/NewtonModelPhysicsTreeItem.h"
#include "ndTree/NewtonModelPhysicsTreeItemRow.h"
#include "ndTree/NewtonModelPhysicsTreeCommands.h"
#include "ndTree/NewtonModelPhysicsTreeItemAcyclicGraphs.h"

#define LOCTEXT_NAMESPACE "FNewtonModelPhysicsTree"

FName FNewtonModelPhysicsTree::m_treeColumnName(TEXT("node name"));
FName FNewtonModelPhysicsTree::m_menuName(TEXT("NewtonModelPhysicsTreeMenu"));
FName FNewtonModelPhysicsTree::m_contextName(TEXT("NewtonModelPhysicsTreeMenuItem"));


FNewtonModelPhysicsTree::FNewtonModelPhysicsTree()
{
	m_acyclicGraph = nullptr;
	m_hideShapes = false;
	m_hideJoints = false;
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
	if (!item.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("TODO: some is wrong function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
		//return SNew(STableRow<TSharedPtr<FNewtonModelPhysicsTreeItem>>, ownerTable)
		//[
		//	SNew(STextBlock)
		//	.Text(FText::FromString(TEXT("some when wrong ")))
		//];
	}

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
		if (!childItem->m_isHidden)
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
	//UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
	m_selectedItem = item;
}

void FNewtonModelPhysicsTree::OnToggleShapeVisibility()
{
	m_hideShapes = !m_hideShapes;

	for (const TPair<FNewtonModelPhysicsTreeItem*, TSharedPtr<FNewtonModelPhysicsTreeItem>>& pair : m_items)
	{
		TSharedPtr<FNewtonModelPhysicsTreeItem> item (pair.Value);
		if (item->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemShape")))
		{
			item->m_isHidden = m_hideShapes;
		}
	}
	RefreshView();
}

void FNewtonModelPhysicsTree::OnToggleJointVisibility()
{
	m_hideJoints = !m_hideJoints;

	for (const TPair<FNewtonModelPhysicsTreeItem*, TSharedPtr<FNewtonModelPhysicsTreeItem>>& pair : m_items)
	{
		TSharedPtr<FNewtonModelPhysicsTreeItem> item(pair.Value);
		if (item->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemJoint")))
		{
			item->m_isHidden = m_hideJoints;
		}
	}
	RefreshView();
}


bool FNewtonModelPhysicsTree::OnCanAddShapeRow() const
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
	return m_selectedItem.IsValid() && m_selectedItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody"));
}

bool FNewtonModelPhysicsTree::OnCanAddJointRow() const
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
	return m_selectedItem.IsValid() && m_selectedItem->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBody"));
}

void FNewtonModelPhysicsTree::OnAddShapeBoxRow()
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
}

void FNewtonModelPhysicsTree::OnAddShapeSphereRow()
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
}

void FNewtonModelPhysicsTree::OnAddJointHingeRow()
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
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
		m_items.FindAndRemoveChecked(node->m_item.Get());
		for (int i = node->m_children.Num() - 1; i >= 0; --i)
		{
			nodeStack[stack] = node->m_children[i];
			stack++;
		}
	}

	m_selectedItem->m_acyclicGraph->m_parent->RemoveChild(m_selectedItem->m_acyclicGraph);
	delete m_selectedItem->m_acyclicGraph;

	m_editor->m_modelChange = true;
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
		,FIsActionChecked::CreateLambda([this]() { return m_hideShapes; }));
		//,FIsActionChecked::CreateSP(this, &FNewtonModelPhysicsTree::OnGetShapeVisibleRowsState));

	commandList.MapAction(menuActions.JointsVisibility
		, FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnToggleJointVisibility)
		, FCanExecuteAction()
		, FIsActionChecked::CreateLambda([this]() { return m_hideJoints; }));

	// add ohysics tree building actions
	commandList.MapAction(menuActions.AddShapeBox
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnAddShapeBoxRow)
		,FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCanAddShapeRow));

	commandList.MapAction(menuActions.AddShapeSphere
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnAddShapeSphereRow)
		,FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCanAddShapeRow));

	commandList.MapAction(menuActions.AddJointHinge
		,FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnAddJointHingeRow)
		,FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCanAddJointRow));

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

void FNewtonModelPhysicsTree::BuildAcyclicTree()
{
	check(m_root.Num() == 1);
	check(m_root[0].IsValid());
	check(m_root[0]->IsOfRttiByName(TEXT("FNewtonModelPhysicsTreeItemBodyRoot")));
	
	if (m_acyclicGraph)
	{
		check(0);
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
}

void FNewtonModelPhysicsTree::BuildTree()
{
	if (m_acyclicGraph)
	{
		delete m_acyclicGraph;
	}
	check(m_items.IsEmpty());
	
	UNewtonModelNode* nodeStack[TREE_STACK_DEPTH];
	TSharedPtr<FNewtonModelPhysicsTreeItem> parentStack[TREE_STACK_DEPTH];
	
	const UNewtonModel* const model = m_editor->GetNewtonModel();
	if (!model->RootBody)
	{
		return;
	}
	
	check(Cast<UNewtonModelNodeRigidBodyRoot>(model->RootBody));
	
	TSet<FName> uniqueNames;
	
	int stack = 1;
	int labelId = 0;
	
	parentStack[0] = nullptr;
	nodeStack[0] = model->RootBody;
	
	while (stack)
	{
		stack--;
		UNewtonModelNode* const node = nodeStack[stack];
		TSharedPtr<FNewtonModelPhysicsTreeItem> parent(parentStack[stack]);
	
		const FString nodeName(node->Name.ToString());
		while (uniqueNames.Find(node->Name))
		{
			labelId++;
			const FString name(nodeName + "_" + FString::FromInt(labelId));
			node->Name = FName(*name);
		}
		uniqueNames.Add(node->Name);
	
		if (Cast<UNewtonModelNodeRigidBodyRoot>(node))
		{
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemBodyRoot(nullptr, node->Name)));
			m_root.Push(item);
			m_items.Add(&item.Get(), item);
			parent = item;
		}
		else if (Cast<UNewtonModelNodeRigidBody>(node))
		{
			//UE_LOG(LogTemp, Warning, TEXT("TODO: remember emit body:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemBody(parent, node->Name)));
			m_items.Add(&item.Get(), item);
			parent = item;
		}
		else if (Cast<UNewtonModelNodeJoint>(node))
		{
			//UE_LOG(LogTemp, Warning, TEXT("TODO: remember emit body:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemJoint(parent, node->Name)));
			m_items.Add(&item.Get(), item);
			parent = item;
		}
		else if (Cast<UNewtonModelNodeCollision>(node))
		{
			//UE_LOG(LogTemp, Warning, TEXT("TODO: remember emit body:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShape(parent, node->Name)));
			m_items.Add(&item.Get(), item);
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
	
	BuildAcyclicTree();
	RefreshView();
}

#undef LOCTEXT_NAMESPACE