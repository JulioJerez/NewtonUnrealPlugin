// Fill out your copyright notice in the Description page of Project Settings.


#include "ndTree/NewtonModelPhysicsTree.h"
#include "IPinnedCommandList.h"
#include "SPositiveActionButton.h"
#include "UICommandList_Pinnable.h"
#include "IPinnedCommandListModule.h"

#include "ndTree/NewtonModelPhysicsTreeCommands.h"
#include "ndTree/NewtonModelPhysicsTreeMenuContext.h"
#include "ndTree/NewtonModelPhysicsTreeEditableSkeleton.h"

#define LOCTEXT_NAMESPACE "FNewtonModelPhysicsTree"


FName FNewtonModelPhysicsTree::m_menuName(TEXT("NewtonModelPhysicsTreeMenu"));

FNewtonModelPhysicsTree::FNewtonModelPhysicsTree()
	:SCompoundWidget()
{
}

FNewtonModelPhysicsTree::~FNewtonModelPhysicsTree()
{
}


void FNewtonModelPhysicsTree::BindCommands()
{
	// This should not be called twice on the same instance
	check(!UICommandList.IsValid());

	UICommandList = MakeShareable(new FUICommandList_Pinnable);

	FUICommandList_Pinnable& CommandList = *UICommandList;
	
	//// Grab the list of menu commands to bind...
	//const FSkeletonTreeCommands& MenuActions = FSkeletonTreeCommands::Get();
	//
	//// ...and bind them all
	//
	//CommandList.MapAction(
	//	MenuActions.FilteringFlattensHierarchy,
	//	FExecuteAction::CreateLambda([this]() { GetMutableDefault<UPersonaOptions>()->bFlattenSkeletonHierarchyWhenFiltering = !GetDefault<UPersonaOptions>()->bFlattenSkeletonHierarchyWhenFiltering; ApplyFilter(); }),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateLambda([]() { return GetDefault<UPersonaOptions>()->bFlattenSkeletonHierarchyWhenFiltering; }));
	//
	//CommandList.MapAction(
	//	MenuActions.HideParentsWhenFiltering,
	//	FExecuteAction::CreateLambda([this]() { GetMutableDefault<UPersonaOptions>()->bHideParentsWhenFiltering = !GetDefault<UPersonaOptions>()->bHideParentsWhenFiltering; }),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateLambda([]() { return GetDefault<UPersonaOptions>()->bHideParentsWhenFiltering; }));
	//
	//CommandList.MapAction(
	//	MenuActions.ShowBoneIndex,
	//	FExecuteAction::CreateLambda([this]() { GetMutableDefault<UPersonaOptions>()->bShowBoneIndexes = !GetDefault<UPersonaOptions>()->bShowBoneIndexes; }),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateLambda([]() { return GetDefault<UPersonaOptions>()->bShowBoneIndexes; }));
	//
	//// Bone Filter commands
	//CommandList.BeginGroup(TEXT("BoneFilterGroup"));
	//
	//CommandList.MapAction(
	//	MenuActions.ShowAllBones,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::SetBoneFilter, EBoneFilter::All),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &SSkeletonTree::IsBoneFilter, EBoneFilter::All),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingBones));
	//
	//CommandList.MapAction(
	//	MenuActions.ShowMeshBones,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::SetBoneFilter, EBoneFilter::Mesh),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &SSkeletonTree::IsBoneFilter, EBoneFilter::Mesh),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingBones));
	//
	//CommandList.MapAction(
	//	MenuActions.ShowLODBones,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::SetBoneFilter, EBoneFilter::LOD),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &SSkeletonTree::IsBoneFilter, EBoneFilter::LOD),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingBones));
	//
	//CommandList.MapAction(
	//	MenuActions.ShowWeightedBones,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::SetBoneFilter, EBoneFilter::Weighted),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &SSkeletonTree::IsBoneFilter, EBoneFilter::Weighted),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingBones));
	//
	//CommandList.MapAction(
	//	MenuActions.HideBones,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::SetBoneFilter, EBoneFilter::None),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &SSkeletonTree::IsBoneFilter, EBoneFilter::None),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingBones));
	//
	//CommandList.EndGroup();
	//
	//// Socket filter commands
	//CommandList.BeginGroup(TEXT("SocketFilterGroup"));
	//
	//CommandList.MapAction(
	//	MenuActions.ShowActiveSockets,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::SetSocketFilter, ESocketFilter::Active),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &SSkeletonTree::IsSocketFilter, ESocketFilter::Active),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingSockets));
	//
	//CommandList.MapAction(
	//	MenuActions.ShowMeshSockets,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::SetSocketFilter, ESocketFilter::Mesh),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &SSkeletonTree::IsSocketFilter, ESocketFilter::Mesh),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingSockets));
	//
	//CommandList.MapAction(
	//	MenuActions.ShowSkeletonSockets,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::SetSocketFilter, ESocketFilter::Skeleton),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &SSkeletonTree::IsSocketFilter, ESocketFilter::Skeleton),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingSockets));
	//
	//CommandList.MapAction(
	//	MenuActions.ShowAllSockets,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::SetSocketFilter, ESocketFilter::All),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &SSkeletonTree::IsSocketFilter, ESocketFilter::All),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingSockets));
	//
	//CommandList.MapAction(
	//	MenuActions.HideSockets,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::SetSocketFilter, ESocketFilter::None),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &SSkeletonTree::IsSocketFilter, ESocketFilter::None),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingSockets));
	//
	//CommandList.EndGroup();
	//
	//CommandList.MapAction(
	//	MenuActions.ShowRetargeting,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnChangeShowingAdvancedOptions),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &SSkeletonTree::IsShowingAdvancedOptions),
	//	FIsActionButtonVisible::CreateLambda([this]() { return Builder->IsShowingBones() && bAllowSkeletonOperations; }));
	//
	//CommandList.MapAction(
	//	MenuActions.ShowDebugVisualization,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnChangeShowingDebugVisualizationOptions),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &SSkeletonTree::IsShowingDebugVisualizationOptions),
	//	FIsActionButtonVisible::CreateLambda([this]() { return bShowDebugVisualizationOptions; }));
	//
	//// Socket manipulation commands
	//CommandList.MapAction(
	//	MenuActions.AddSocket,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnAddSocket),
	//	FCanExecuteAction::CreateSP(this, &SSkeletonTree::IsAddingSocketsAllowed));
	//
	//CommandList.MapAction(
	//	FGenericCommands::Get().Rename,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnRenameSelected),
	//	FCanExecuteAction::CreateSP(this, &SSkeletonTree::CanRenameSelected));
	//
	//CommandList.MapAction(
	//	MenuActions.CreateMeshSocket,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnCustomizeSocket));
	//
	//CommandList.MapAction(
	//	MenuActions.RemoveMeshSocket,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnDeleteSelectedRows),
	//	FCanExecuteAction::CreateSP(this, &SSkeletonTree::CanDeleteSelectedRows));
	//
	//CommandList.MapAction(
	//	MenuActions.PromoteSocketToSkeleton,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnPromoteSocket)); // Adding customization just deletes the mesh socket
	//
	//CommandList.MapAction(
	//	MenuActions.DeleteSelectedRows,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnDeleteSelectedRows),
	//	FCanExecuteAction::CreateSP(this, &SSkeletonTree::CanDeleteSelectedRows));
	//
	//CommandList.MapAction(
	//	MenuActions.CopyBoneNames,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnCopyBoneNames));
	//
	//CommandList.MapAction(
	//	MenuActions.ResetBoneTransforms,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnResetBoneTransforms));
	//
	//CommandList.MapAction(
	//	MenuActions.CopySockets,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnCopySockets),
	//	FCanExecuteAction::CreateSP(this, &SSkeletonTree::CanCopySockets));
	//
	//CommandList.MapAction(
	//	MenuActions.PasteSockets,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnPasteSockets, false),
	//	FCanExecuteAction::CreateSP(this, &SSkeletonTree::CanPasteSockets));
	//
	//CommandList.MapAction(
	//	MenuActions.PasteSocketsToSelectedBone,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnPasteSockets, true),
	//	FCanExecuteAction::CreateSP(this, &SSkeletonTree::CanPasteSockets));
	//
	//CommandList.MapAction(
	//	MenuActions.FocusCamera,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::HandleFocusCamera));
	//
	//CommandList.MapAction(
	//	MenuActions.CreateTimeBlendProfile,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnCreateBlendProfile, EBlendProfileMode::TimeFactor));
	//
	//CommandList.MapAction(
	//	MenuActions.CreateWeightBlendProfile,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnCreateBlendProfile, EBlendProfileMode::WeightFactor));
	//
	//CommandList.MapAction(
	//	MenuActions.CreateBlendMask,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnCreateBlendProfile, EBlendProfileMode::BlendMask));
	//
	//CommandList.MapAction(
	//	MenuActions.DeleteCurrentBlendProfile,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnDeleteCurrentBlendProfile));
	//
	//CommandList.MapAction(
	//	MenuActions.RenameBlendProfile,
	//	FExecuteAction::CreateSP(this, &SSkeletonTree::OnRenameBlendProfile));
	
	PinnedCommands->BindCommandList(UICommandList.ToSharedRef());
}

TSharedRef< SWidget > FNewtonModelPhysicsTree::CreateNewMenuWidget()
{
	FToolMenuContext MenuContext(UICommandList, Extenders);
	UNewtonModelPhysicsTreeMenuContext* SkeletonTreeMenuContext = NewObject<UNewtonModelPhysicsTreeMenuContext>();
	SkeletonTreeMenuContext->SkeletonTree = SharedThis(this);
	MenuContext.AddObject(SkeletonTreeMenuContext);

	return UToolMenus::Get()->GenerateWidget(m_menuName, MenuContext);
}

void FNewtonModelPhysicsTree::RegisterNewMenu()
{
	if (UToolMenus::Get()->IsMenuRegistered(m_menuName))
	{
		return;
	}

	FToolMenuOwnerScoped OwnerScoped(this);

	const FNewtonModelPhysicsTreeCommands& Actions = FNewtonModelPhysicsTreeCommands::Get();

	UToolMenu* const menu = UToolMenus::Get()->RegisterMenu(m_menuName);

	//{
	//	FToolMenuSection& CreateSection = Menu->AddSection("CreateNew", LOCTEXT("SkeletonCreateNew", "Create"));
	//
	//	CreateSection.AddMenuEntry(Actions.AddSocket);
	//	CreateSection.AddSubMenu(
	//		"VirtualBones",
	//		LOCTEXT("AddVirtualBone", "Add Virtual Bone"),
	//		LOCTEXT("AddVirtualBone_ToolTip", "Adds a virtual bone to the skeleton."),
	//		FNewToolMenuDelegate::CreateLambda([](UToolMenu* InMenu)
	//			{
	//				USkeletonTreeMenuContext* MenuContext = InMenu->Context.FindContext<USkeletonTreeMenuContext>();
	//				if (MenuContext == nullptr)
	//				{
	//					return;
	//				}
	//
	//				TSharedPtr<SSkeletonTree> SkeletonTree = MenuContext->SkeletonTree.Pin();
	//				if (!SkeletonTree.IsValid())
	//				{
	//					return;
	//				}
	//
	//				TSharedRef<SBoneTreeMenu> MenuContent = SSkeletonTree::CreateVirtualBoneMenu(SkeletonTree.Get());
	//				FToolMenuEntry WidgetEntry = FToolMenuEntry::InitWidget("VirtualBones", MenuContent, FText());
	//				InMenu->AddMenuEntry(NAME_None, WidgetEntry);
	//			}));
	//}

	
	FToolMenuSection& rootSection = menu->AddSection("Root Rigid Body", LOCTEXT("RigidBody", "Rigid"));
	rootSection.AddMenuEntry(Actions.AddRootBody);

	FToolMenuSection& linkSection = menu->AddSection("Link Rigid Body", LOCTEXT("RigidBody", "Rigid"));
	linkSection.AddMenuEntry(Actions.AddLinkBody);

	FToolMenuSection& shapeSection = menu->AddSection("Collisions Shape", LOCTEXT("Collisions", "Collision"));
	shapeSection.AddMenuEntry(Actions.AddBoxShape);
	shapeSection.AddMenuEntry(Actions.AddSphereShape);

	FToolMenuSection& jointSection = menu->AddSection("Constraint Joints", LOCTEXT("Constraint", "Constraint"));
	jointSection.AddMenuEntry(Actions.AddHingeJoint);
}

void FNewtonModelPhysicsTree::Construct(const FArguments& args, const TSharedRef<FNewtonModelPhysicsTreeEditableSkeleton>& skeletonTree, const FNewtonModelPhysicsTreeArgs& skeletonTreeArg)
{
	//if (skeletonTreeArg.bHideBonesByDefault)
	//{
	//	BoneFilter = EBoneFilter::None;
	//}
	//else
	//{
	//	BoneFilter = EBoneFilter::All;
	//}
	//SocketFilter = ESocketFilter::Active;
	//bSelecting = false;
	
	EditableSkeleton = skeletonTree;
	//PreviewScene = skeletonTreeArg.PreviewScene;
	 
	// 
	//IsEditable = args._IsEditable;
	//Mode = skeletonTreeArg.Mode;
	//bAllowMeshOperations = skeletonTreeArg.bAllowMeshOperations;
	//bAllowSkeletonOperations = skeletonTreeArg.bAllowSkeletonOperations;
	//bShowDebugVisualizationOptions = skeletonTreeArg.bShowDebugVisualizationOptions;

	Extenders = skeletonTreeArg.Extenders;
	ContextName = skeletonTreeArg.ContextName;

	//OnGetFilterText = skeletonTreeArg.OnGetFilterText;
	//Builder = skeletonTreeArg.Builder;
	//if (!Builder.IsValid())
	//{
	//	Builder = MakeShareable(new FSkeletonTreeBuilder(FSkeletonTreeBuilderArgs()));
	//}
	//
	//Builder->Initialize(SharedThis(this), skeletonTreeArg.PreviewScene, FOnFilterSkeletonTreeItem::CreateSP(this, &SSkeletonTree::HandleFilterSkeletonTreeItem));

	
	//TextFilterPtr = MakeShareable(new FTextFilterExpressionEvaluator(ETextFilterExpressionEvaluatorMode::BasicString));
	//
	//SetPreviewComponentSocketFilter();
	//
	//// Register delegates
	//
	//if (PreviewScene.IsValid())
	//{
	//	PreviewScene.Pin()->RegisterOnLODChanged(FSimpleDelegate::CreateSP(this, &SSkeletonTree::OnLODSwitched));
	//	PreviewScene.Pin()->RegisterOnPreviewMeshChanged(FOnPreviewMeshChanged::CreateSP(this, &SSkeletonTree::OnPreviewMeshChanged));
	//	PreviewScene.Pin()->RegisterOnSelectedBoneChanged(FOnSelectedBoneChanged::CreateSP(this, &SSkeletonTree::HandleSelectedBoneChanged));
	//	PreviewScene.Pin()->RegisterOnSelectedSocketChanged(FOnSelectedSocketChanged::CreateSP(this, &SSkeletonTree::HandleSelectedSocketChanged));
	//	PreviewScene.Pin()->RegisterOnDeselectAll(FSimpleDelegate::CreateSP(this, &SSkeletonTree::HandleDeselectAll));
	//
	//	RegisterOnSelectionChanged(FOnSkeletonTreeSelectionChanged::CreateRaw(PreviewScene.Pin().Get(), &IPersonaPreviewScene::HandleSkeletonTreeSelectionChanged));
	//}
	//
	//if (skeletonTreeArg.OnSelectionChanged.IsBound())
	//{
	//	RegisterOnSelectionChanged(skeletonTreeArg.OnSelectionChanged);
	//}
	//
	//FCoreUObjectDelegates::OnPackageReloaded.AddSP(this, &SSkeletonTree::HandlePackageReloaded);
	//
	// Create our pinned commands before we bind commands
	IPinnedCommandListModule& PinnedCommandListModule = FModuleManager::LoadModuleChecked<IPinnedCommandListModule>(TEXT("PinnedCommandList"));
	PinnedCommands = PinnedCommandListModule.CreatePinnedCommandList(ContextName);
	
	// Register and bind all our menu commands
	FNewtonModelPhysicsTreeCommands::Register();
	BindCommands();
	
	//RegisterBlendProfileMenu();
	RegisterNewMenu();
	//RegisterFilterMenu();

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
					.OnGetMenuContent(this, &FNewtonModelPhysicsTree::CreateNewMenuWidget)
					.Icon(FAppStyle::Get().GetBrush("Icons.Plus"))
				]

				//+SHorizontalBox::Slot()
				//.FillWidth(1.0f)
				//[
				//	SAssignNew( NameFilterBox, SSearchBox )
				//	.SelectAllTextWhenFocused( true )
				//	.OnTextChanged( this, &SSkeletonTree::OnFilterTextChanged )
				//	.HintText( LOCTEXT( "SearchBoxHint", "Search Skeleton Tree...") )
				//	.AddMetaData<FTagMetaData>(TEXT("SkelTree.Search"))
				//]

				//+SHorizontalBox::Slot()
				//.AutoWidth()
				//.Padding(FMargin(6.f, 0.0))
				//.VAlign(VAlign_Center)
				//[
				//	SAssignNew(FilterComboButton, SComboButton)
				//	.Visibility(InSkeletonTreeArgs.bShowFilterMenu ? EVisibility::Visible : EVisibility::Collapsed)
				//	.ComboButtonStyle(&FAppStyle::Get().GetWidgetStyle<FComboButtonStyle>("SimpleComboButton"))
				//	.ForegroundColor(FSlateColor::UseStyle())
				//	.ContentPadding(2.0f)
				//	.OnGetMenuContent( this, &SSkeletonTree::CreateFilterMenuWidget )
				//	.ToolTipText( this, &SSkeletonTree::GetFilterMenuTooltip )
				//	.AddMetaData<FTagMetaData>(TEXT("SkelTree.Bones"))
				//	.HasDownArrow(true)
				//	.ButtonContent()
				//	[
				//		SNew(SImage)
				//		.Image(FAppStyle::Get().GetBrush("Icons.Settings"))
				//		.ColorAndOpacity(FSlateColor::UseForeground())
				//	]
				//]
			]

			//+ SVerticalBox::Slot()
			//.Padding(FMargin(0.0f, 2.0f, 0.0f, 0.0f))
			//.AutoHeight()
			//[
			//	PinnedCommands.ToSharedRef()
			//]
			//
			//+ SVerticalBox::Slot()
			//.Padding(FMargin(0.0f, 2.0f, 0.0f, 0.0f))
			//[
			//	SAssignNew(TreeHolder, SOverlay)
			//]
		]
	];
	
	//SAssignNew(BlendProfilePicker, SBlendProfilePicker, GetEditableSkeleton())
	//	.Standalone(true)
	//	.OnBlendProfileSelected(this, &SSkeletonTree::OnBlendProfileSelected);
	//
	//
	//CreateTreeColumns();
	//
	//SetInitialExpansionState();
	//
	//OnLODSwitched();
}

#undef LOCTEXT_NAMESPACE