// Fill out your copyright notice in the Description page of Project Settings.


#include "ndTree/NewtonModelPhysicsTree.h"

#if 0
#include "IPinnedCommandList.h"
#include "SPositiveActionButton.h"
#include "UICommandList_Pinnable.h"
#include "IPinnedCommandListModule.h"
#include "Widgets/Layout/SScrollBorder.h"

#include "ndTree/NewtonModelPhysicsTreeItem.h"
#include "ndTree/NewtonModelPhysicsTreeCommands.h"
#include "ndTree/NewtonModelPhysicsTreeMenuContext.h"
#include "ndTree/NewtonModelPhysicsTreeEditableSkeleton.h"

#define LOCTEXT_NAMESPACE "FNewtonModelPhysicsTree"

FName FNewtonModelPhysicsTree::m_treeColumnName(TEXT("node name"));
FName FNewtonModelPhysicsTree::m_menuName(TEXT("NewtonModelPhysicsTreeMenu"));


template <typename ItemType>
class FNewtonModelPhysicsTree::FNewtonModelPhysicsTreeView : public STreeView<ItemType>
{
	public:
	bool Private_CanItemBeSelected(ItemType InItem) const
	{
		//return !(InItem->GetFilterResult() == ESkeletonTreeFilterResult::ShownDescendant && GetMutableDefault<UPersonaOptions>()->bHideParentsWhenFiltering);
		//return !(InItem->GetFilterResult() == ShownDescendant && GetMutableDefault<UPersonaOptions>()->bHideParentsWhenFiltering);
		return true;
	}

	virtual void Private_SelectRangeFromCurrentTo(ItemType InRangeSelectionEnd) override
	{
		if (this->SelectionMode.Get() == ESelectionMode::None)
		{
			return;
		}

		const TArrayView<const ItemType> ItemsSourceRef = this->SListView<ItemType>::GetItems();

		int32 RangeStartIndex = 0;
		if (TListTypeTraits<ItemType>::IsPtrValid(this->RangeSelectionStart))
		{
			RangeStartIndex = ItemsSourceRef.Find(TListTypeTraits<ItemType>::NullableItemTypeConvertToItemType(this->RangeSelectionStart));
		}

		int32 RangeEndIndex = ItemsSourceRef.Find(InRangeSelectionEnd);

		RangeStartIndex = FMath::Clamp(RangeStartIndex, 0, ItemsSourceRef.Num());
		RangeEndIndex = FMath::Clamp(RangeEndIndex, 0, ItemsSourceRef.Num());

		if (RangeEndIndex < RangeStartIndex)
		{
			Swap(RangeStartIndex, RangeEndIndex);
		}

		for (int32 ItemIndex = RangeStartIndex; ItemIndex <= RangeEndIndex; ++ItemIndex)
		{
			// check if this item can actually be selected
			if (Private_CanItemBeSelected(ItemsSourceRef[ItemIndex]))
			{
				this->SelectedItems.Add(ItemsSourceRef[ItemIndex]);
			}
		}

		this->InertialScrollManager.ClearScrollVelocity();
	}

	virtual void Private_SignalSelectionChanged(ESelectInfo::Type SelectInfo) override
	{
		STreeView<ItemType>::Private_SignalSelectionChanged(SelectInfo);

		// the SListView does not know about bHideParentsWhenFiltering and will select the boens regardless of their visible
		// ( For example when using select all )
		// this filter out those ones to only keep the ones that can be selected
		{
			TArray<ItemType> FilteredSelection;
			for (const ItemType& Item : this->SelectedItems)
			{
				if (Private_CanItemBeSelected(Item))
				{
					FilteredSelection.Add(Item);
				}
			}
			if (FilteredSelection.Num() != this->SelectedItems.Num())
			{
				this->ClearSelection();
				this->SetItemSelection(FilteredSelection, true, SelectInfo);
			}
		}
	}
};



FNewtonModelPhysicsTree::FNewtonModelPhysicsTree()
	:SCompoundWidget()
{
}

FNewtonModelPhysicsTree::~FNewtonModelPhysicsTree()
{
}

TSharedRef<FNewtonModelPhysicsTreeEditableSkeleton> FNewtonModelPhysicsTree::GetEditableSkeleton() const
{ 
	return EditableSkeleton.Pin().ToSharedRef(); 
}

void FNewtonModelPhysicsTree::OnCreateRootBone()
{
	check(0);
}

void FNewtonModelPhysicsTree::BindCommands()
{
	// This should not be called twice on the same instance
	check(!UICommandList.IsValid());

	UICommandList = MakeShareable(new FUICommandList_Pinnable);

	FUICommandList_Pinnable& commandList = *UICommandList;
	
	// Grab the list of menu commands to bind...
	const FNewtonModelPhysicsTreeCommands& menuActions = FNewtonModelPhysicsTreeCommands::Get();
	
	//// ...and bind them all
	//
	//commandList.MapAction(
	//	menuActions.FilteringFlattensHierarchy,
	//	FExecuteAction::CreateLambda([this]() { GetMutableDefault<UPersonaOptions>()->bFlattenSkeletonHierarchyWhenFiltering = !GetDefault<UPersonaOptions>()->bFlattenSkeletonHierarchyWhenFiltering; ApplyFilter(); }),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateLambda([]() { return GetDefault<UPersonaOptions>()->bFlattenSkeletonHierarchyWhenFiltering; }));
	//
	//commandList.MapAction(
	//	menuActions.HideParentsWhenFiltering,
	//	FExecuteAction::CreateLambda([this]() { GetMutableDefault<UPersonaOptions>()->bHideParentsWhenFiltering = !GetDefault<UPersonaOptions>()->bHideParentsWhenFiltering; }),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateLambda([]() { return GetDefault<UPersonaOptions>()->bHideParentsWhenFiltering; }));
	//
	//commandList.MapAction(
	//	menuActions.ShowBoneIndex,
	//	FExecuteAction::CreateLambda([this]() { GetMutableDefault<UPersonaOptions>()->bShowBoneIndexes = !GetDefault<UPersonaOptions>()->bShowBoneIndexes; }),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateLambda([]() { return GetDefault<UPersonaOptions>()->bShowBoneIndexes; }));
	//
	//// Bone Filter commands
	//commandList.BeginGroup(TEXT("BoneFilterGroup"));
	//
	//commandList.MapAction(
	//	menuActions.ShowAllBones,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::SetBoneFilter, EBoneFilter::All),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &FNewtonModelPhysicsTree::IsBoneFilter, EBoneFilter::All),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingBones));
	//
	//commandList.MapAction(
	//	menuActions.ShowMeshBones,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::SetBoneFilter, EBoneFilter::Mesh),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &FNewtonModelPhysicsTree::IsBoneFilter, EBoneFilter::Mesh),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingBones));
	//
	//commandList.MapAction(
	//	menuActions.ShowLODBones,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::SetBoneFilter, EBoneFilter::LOD),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &FNewtonModelPhysicsTree::IsBoneFilter, EBoneFilter::LOD),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingBones));
	//
	//commandList.MapAction(
	//	menuActions.ShowWeightedBones,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::SetBoneFilter, EBoneFilter::Weighted),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &FNewtonModelPhysicsTree::IsBoneFilter, EBoneFilter::Weighted),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingBones));
	//
	//commandList.MapAction(
	//	menuActions.HideBones,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::SetBoneFilter, EBoneFilter::None),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &FNewtonModelPhysicsTree::IsBoneFilter, EBoneFilter::None),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingBones));
	//
	//commandList.EndGroup();
	//
	//// Socket filter commands
	//commandList.BeginGroup(TEXT("SocketFilterGroup"));
	//
	//commandList.MapAction(
	//	menuActions.ShowActiveSockets,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::SetSocketFilter, ESocketFilter::Active),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &FNewtonModelPhysicsTree::IsSocketFilter, ESocketFilter::Active),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingSockets));
	//
	//commandList.MapAction(
	//	menuActions.ShowMeshSockets,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::SetSocketFilter, ESocketFilter::Mesh),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &FNewtonModelPhysicsTree::IsSocketFilter, ESocketFilter::Mesh),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingSockets));
	//
	//commandList.MapAction(
	//	menuActions.ShowSkeletonSockets,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::SetSocketFilter, ESocketFilter::Skeleton),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &FNewtonModelPhysicsTree::IsSocketFilter, ESocketFilter::Skeleton),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingSockets));
	//
	//commandList.MapAction(
	//	menuActions.ShowAllSockets,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::SetSocketFilter, ESocketFilter::All),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &FNewtonModelPhysicsTree::IsSocketFilter, ESocketFilter::All),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingSockets));
	//
	//commandList.MapAction(
	//	menuActions.HideSockets,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::SetSocketFilter, ESocketFilter::None),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &FNewtonModelPhysicsTree::IsSocketFilter, ESocketFilter::None),
	//	FIsActionButtonVisible::CreateSP(Builder.Get(), &ISkeletonTreeBuilder::IsShowingSockets));
	//
	//commandList.EndGroup();
	//
	//commandList.MapAction(
	//	menuActions.ShowRetargeting,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnChangeShowingAdvancedOptions),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &FNewtonModelPhysicsTree::IsShowingAdvancedOptions),
	//	FIsActionButtonVisible::CreateLambda([this]() { return Builder->IsShowingBones() && bAllowSkeletonOperations; }));
	//
	//commandList.MapAction(
	//	menuActions.ShowDebugVisualization,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnChangeShowingDebugVisualizationOptions),
	//	FCanExecuteAction(),
	//	FIsActionChecked::CreateSP(this, &FNewtonModelPhysicsTree::IsShowingDebugVisualizationOptions),
	//	FIsActionButtonVisible::CreateLambda([this]() { return bShowDebugVisualizationOptions; }));
	//
	//// Socket manipulation commands
	//commandList.MapAction(
	//	menuActions.AddSocket,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnAddSocket),
	//	FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::IsAddingSocketsAllowed));
	//
	//commandList.MapAction(
	//	FGenericCommands::Get().Rename,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnRenameSelected),
	//	FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::CanRenameSelected));
	//
	//commandList.MapAction(
	//	menuActions.CreateMeshSocket,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCustomizeSocket));
	//
	//commandList.MapAction(
	//	menuActions.RemoveMeshSocket,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnDeleteSelectedRows),
	//	FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::CanDeleteSelectedRows));
	//
	//commandList.MapAction(
	//	menuActions.PromoteSocketToSkeleton,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnPromoteSocket)); // Adding customization just deletes the mesh socket
	//
	//commandList.MapAction(
	//	menuActions.DeleteSelectedRows,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnDeleteSelectedRows),
	//	FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::CanDeleteSelectedRows));
	//
	//commandList.MapAction(
	//	menuActions.CopyBoneNames,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCopyBoneNames));
	//
	//commandList.MapAction(
	//	menuActions.ResetBoneTransforms,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnResetBoneTransforms));
	//
	//commandList.MapAction(
	//	menuActions.CopySockets,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCopySockets),
	//	FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::CanCopySockets));
	//
	//commandList.MapAction(
	//	menuActions.PasteSockets,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnPasteSockets, false),
	//	FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::CanPasteSockets));
	//
	//commandList.MapAction(
	//	menuActions.PasteSocketsToSelectedBone,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnPasteSockets, true),
	//	FCanExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::CanPasteSockets));
	//
	//commandList.MapAction(
	//	menuActions.FocusCamera,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::HandleFocusCamera));
	//
	//commandList.MapAction(
	//	menuActions.CreateTimeBlendProfile,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCreateBlendProfile, EBlendProfileMode::TimeFactor));
	//
	//commandList.MapAction(
	//	menuActions.CreateWeightBlendProfile,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCreateBlendProfile, EBlendProfileMode::WeightFactor));
	//
	//commandList.MapAction(
	//	menuActions.CreateBlendMask,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCreateBlendProfile, EBlendProfileMode::BlendMask));
	//
	//commandList.MapAction(
	//	menuActions.DeleteCurrentBlendProfile,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnDeleteCurrentBlendProfile));
	//
	//commandList.MapAction(
	//	menuActions.RenameBlendProfile,
	//	FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnRenameBlendProfile));

	commandList.MapAction(menuActions.AddRootBody, FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCreateRootBone));


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
	//				TSharedPtr<FNewtonModelPhysicsTree> SkeletonTree = MenuContext->SkeletonTree.Pin();
	//				if (!SkeletonTree.IsValid())
	//				{
	//					return;
	//				}
	//
	//				TSharedRef<SBoneTreeMenu> MenuContent = FNewtonModelPhysicsTree::CreateVirtualBoneMenu(SkeletonTree.Get());
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

//ESkeletonTreeFilterResult SSkeletonTree::HandleFilterSkeletonTreeItem(const FSkeletonTreeFilterArgs& InArgs, const TSharedPtr<class INewtonModelPhysicsTreeItem>& InItem)
ENewtonModelPhysicsTreeFilterResult FNewtonModelPhysicsTree::HandleFilterSkeletonTreeItem(const FNewtonModelPhysicsTreeFilterArgs& args, const TSharedPtr<INewtonModelPhysicsTreeItem>& item)
{
	ENewtonModelPhysicsTreeFilterResult Result = Shown;

	check(0);
	//if (InItem->IsOfType<FSkeletonTreeBoneItem>() || InItem->IsOfType<FSkeletonTreeSocketItem>() || InItem->IsOfType<FSkeletonTreeAttachedAssetItem>() || InItem->IsOfType<FSkeletonTreeVirtualBoneItem>())
	//{
	//
	//	if (InArgs.TextFilter.IsValid())
	//	{
	//		if (InArgs.TextFilter->TestTextFilter(FBasicStringFilterExpressionContext(InItem->GetRowItemName().ToString())))
	//		{
	//			Result = ESkeletonTreeFilterResult::ShownHighlighted;
	//		}
	//		else
	//		{
	//			Result = ESkeletonTreeFilterResult::Hidden;
	//		}
	//	}
	//
	//
	//	if (InItem->IsOfType<FSkeletonTreeBoneItem>())
	//	{
	//		TSharedPtr<FSkeletonTreeBoneItem> BoneItem = StaticCastSharedPtr<FSkeletonTreeBoneItem>(InItem);
	//
	//		if (BoneFilter == EBoneFilter::None)
	//		{
	//			Result = ESkeletonTreeFilterResult::Hidden;
	//		}
	//		else if (GetPreviewScene().IsValid())
	//		{
	//			UDebugSkelMeshComponent* PreviewMeshComponent = GetPreviewScene()->GetPreviewMeshComponent();
	//			if (PreviewMeshComponent)
	//			{
	//				const int32 BoneMeshIndex = PreviewMeshComponent->GetBoneIndex(BoneItem->GetRowItemName());
	//
	//				// Remove non-mesh bones if we're filtering
	//				if ((BoneFilter == EBoneFilter::Mesh || BoneFilter == EBoneFilter::Weighted || BoneFilter == EBoneFilter::LOD) &&
	//					BoneMeshIndex == INDEX_NONE)
	//				{
	//					Result = ESkeletonTreeFilterResult::Hidden;
	//				}
	//
	//				// Remove non-vertex-weighted bones if we're filtering
	//				else if (BoneFilter == EBoneFilter::Weighted && !BoneItem->IsBoneWeighted(BoneMeshIndex, PreviewMeshComponent))
	//				{
	//					Result = ESkeletonTreeFilterResult::Hidden;
	//				}
	//
	//				// Remove non-vertex-weighted bones if we're filtering
	//				else if (BoneFilter == EBoneFilter::LOD && !BoneItem->IsBoneRequired(BoneMeshIndex, PreviewMeshComponent))
	//				{
	//					Result = ESkeletonTreeFilterResult::Hidden;
	//				}
	//			}
	//		}
	//	}
	//	else if (InItem->IsOfType<FSkeletonTreeSocketItem>())
	//	{
	//		TSharedPtr<FSkeletonTreeSocketItem> SocketItem = StaticCastSharedPtr<FSkeletonTreeSocketItem>(InItem);
	//
	//		if (SocketFilter == ESocketFilter::None)
	//		{
	//			Result = ESkeletonTreeFilterResult::Hidden;
	//		}
	//
	//		// Remove non-mesh sockets if we're filtering
	//		else if ((SocketFilter == ESocketFilter::Mesh || SocketFilter == ESocketFilter::None) && SocketItem->GetParentType() == ESocketParentType::Skeleton)
	//		{
	//			Result = ESkeletonTreeFilterResult::Hidden;
	//		}
	//
	//		// Remove non-skeleton sockets if we're filtering
	//		else if ((SocketFilter == ESocketFilter::Skeleton || SocketFilter == ESocketFilter::None) && SocketItem->GetParentType() == ESocketParentType::Mesh)
	//		{
	//			Result = ESkeletonTreeFilterResult::Hidden;
	//		}
	//
	//		else if (SocketFilter == ESocketFilter::Active && SocketItem->GetParentType() == ESocketParentType::Skeleton && SocketItem->IsSocketCustomized())
	//		{
	//			// Don't add the skeleton socket if it's already added for the mesh
	//			Result = ESkeletonTreeFilterResult::Hidden;
	//		}
	//	}
	//}

	return Result;
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
	Builder = skeletonTreeArg.Builder;
	if (!Builder.IsValid())
	{
		Builder = MakeShareable(new FNewtonModelPhysicsTreeBuilder(FNewtonModelPhysicsTreeBuilder()));
	}
	//Builder->Initialize(SharedThis(this), skeletonTreeArg.PreviewScene, FOnFilterNewtonModelPhysicsTreeItem::CreateSP(this, &FNewtonModelPhysicsTree::HandleFilterSkeletonTreeItem));
	Builder->Initialize(SharedThis(this), FOnFilterNewtonModelPhysicsTreeItem::CreateSP(this, &FNewtonModelPhysicsTree::HandleFilterSkeletonTreeItem));
	
	TextFilterPtr = MakeShareable(new FTextFilterExpressionEvaluator(ETextFilterExpressionEvaluatorMode::BasicString));
	
	//SetPreviewComponentSocketFilter();
	//
	//// Register delegates
	//
	//if (PreviewScene.IsValid())
	//{
	//	PreviewScene.Pin()->RegisterOnLODChanged(FSimpleDelegate::CreateSP(this, &FNewtonModelPhysicsTree::OnLODSwitched));
	//	PreviewScene.Pin()->RegisterOnPreviewMeshChanged(FOnPreviewMeshChanged::CreateSP(this, &FNewtonModelPhysicsTree::OnPreviewMeshChanged));
	//	PreviewScene.Pin()->RegisterOnSelectedBoneChanged(FOnSelectedBoneChanged::CreateSP(this, &FNewtonModelPhysicsTree::HandleSelectedBoneChanged));
	//	PreviewScene.Pin()->RegisterOnSelectedSocketChanged(FOnSelectedSocketChanged::CreateSP(this, &FNewtonModelPhysicsTree::HandleSelectedSocketChanged));
	//	PreviewScene.Pin()->RegisterOnDeselectAll(FSimpleDelegate::CreateSP(this, &FNewtonModelPhysicsTree::HandleDeselectAll));
	//
	//	RegisterOnSelectionChanged(FOnSkeletonTreeSelectionChanged::CreateRaw(PreviewScene.Pin().Get(), &IPersonaPreviewScene::HandleSkeletonTreeSelectionChanged));
	//}
	//
	//if (skeletonTreeArg.OnSelectionChanged.IsBound())
	//{
	//	RegisterOnSelectionChanged(skeletonTreeArg.OnSelectionChanged);
	//}
	//
	//FCoreUObjectDelegates::OnPackageReloaded.AddSP(this, &FNewtonModelPhysicsTree::HandlePackageReloaded);
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
			]

			//+ SVerticalBox::Slot()
			//.Padding(FMargin(0.0f, 2.0f, 0.0f, 0.0f))
			//.AutoHeight()
			//[
			//	PinnedCommands.ToSharedRef()
			//]
			
			+ SVerticalBox::Slot()
			.Padding(FMargin(0.0f, 2.0f, 0.0f, 0.0f))
			[
				SAssignNew(TreeHolder, SOverlay)
			]
		]
	];
	
	//SAssignNew(BlendProfilePicker, SBlendProfilePicker, GetEditableSkeleton())
	//	.Standalone(true)
	//	.OnBlendProfileSelected(this, &FNewtonModelPhysicsTree::OnBlendProfileSelected);
	
	CreateTreeColumns();
	
	//SetInitialExpansionState();
	//
	//OnLODSwitched();
}

void FNewtonModelPhysicsTree::OnFilterTextChanged(const FText& SearchText)
{
	FilterText = SearchText;
	check(0);
	//ApplyFilter();
}


TSharedRef<ITableRow> FNewtonModelPhysicsTree::MakeTreeRowWidget(TSharedPtr<INewtonModelPhysicsTreeItem> InInfo, const TSharedRef<STableViewBase>& OwnerTable)
{
	check(InInfo.IsValid());

	return InInfo->MakeTreeRowWidget(OwnerTable, TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateLambda([this]() { return FilterText; })));
}

void FNewtonModelPhysicsTree::GetFilteredChildren(TSharedPtr<INewtonModelPhysicsTreeItem> InInfo, TArray< TSharedPtr<INewtonModelPhysicsTreeItem> >& OutChildren)
{
	check(InInfo.IsValid());
	OutChildren = InInfo->GetFilteredChildren();
}

void FNewtonModelPhysicsTree::OnSelectionChanged(TSharedPtr<INewtonModelPhysicsTreeItem> Selection, ESelectInfo::Type SelectInfo)
{
	TArray<TSharedPtr<INewtonModelPhysicsTreeItem>> SelectedItems = SkeletonTreeView->GetSelectedItems();

	check(0);
	//if (Selection.IsValid())
	//{
	//	// Disable bone proxy ticking on all bone/virtual bones
	//	for (TSharedPtr<INewtonModelPhysicsTreeItem>& Item : LinearItems)
	//	{
	//		if (Item->IsOfType<FSkeletonTreeBoneItem>())
	//		{
	//			StaticCastSharedPtr<FSkeletonTreeBoneItem>(Item)->EnableBoneProxyTick(false);
	//		}
	//		else if (Item->IsOfType<FSkeletonTreeVirtualBoneItem>())
	//		{
	//			StaticCastSharedPtr<FSkeletonTreeVirtualBoneItem>(Item)->EnableBoneProxyTick(false);
	//		}
	//	}
	//
	//	//Get all the selected items
	//	FSkeletonTreeSelection TreeSelection(SelectedItems);
	//
	//	if (GetPreviewScene().IsValid())
	//	{
	//		UDebugSkelMeshComponent* PreviewComponent = GetPreviewScene()->GetPreviewMeshComponent();
	//		if (TreeSelection.SelectedItems.Num() > 0 && PreviewComponent)
	//		{
	//			// pick the first settable bone from the selection
	//			for (TSharedPtr<INewtonModelPhysicsTreeItem> Item : TreeSelection.SelectedItems)
	//			{
	//				if ((Item->IsOfType<FSkeletonTreeBoneItem>() || Item->IsOfType<FSkeletonTreeVirtualBoneItem>()))
	//				{
	//					// enable ticking on the selected bone proxies
	//					if (Item->IsOfType<FSkeletonTreeBoneItem>())
	//					{
	//						StaticCastSharedPtr<FSkeletonTreeBoneItem>(Item)->EnableBoneProxyTick(true);
	//					}
	//					else if (Item->IsOfType<FSkeletonTreeVirtualBoneItem>())
	//					{
	//						StaticCastSharedPtr<FSkeletonTreeVirtualBoneItem>(Item)->EnableBoneProxyTick(true);
	//					}
	//
	//					// Test SelectInfo so we don't end up in an infinite loop due to delegates calling each other
	//					if (SelectInfo != ESelectInfo::Direct)
	//					{
	//						FName BoneName = Item->GetRowItemName();
	//
	//						// Get bone index
	//						int32 BoneIndex = PreviewComponent->GetBoneIndex(BoneName);
	//						if (BoneIndex != INDEX_NONE)
	//						{
	//							GetPreviewScene()->SetSelectedBone(BoneName, SelectInfo);
	//							break;
	//						}
	//					}
	//				}
	//				// Test SelectInfo so we don't end up in an infinite loop due to delegates calling each other
	//				else if (SelectInfo != ESelectInfo::Direct && Item->IsOfType<FSkeletonTreeSocketItem>())
	//				{
	//					TSharedPtr<FSkeletonTreeSocketItem> SocketItem = StaticCastSharedPtr<FSkeletonTreeSocketItem>(Item);
	//					USkeletalMeshSocket* Socket = SocketItem->GetSocket();
	//					FSelectedSocketInfo SocketInfo(Socket, SocketItem->GetParentType() == ESocketParentType::Skeleton);
	//					GetPreviewScene()->SetSelectedSocket(SocketInfo);
	//				}
	//				else if (Item->IsOfType<FSkeletonTreeAttachedAssetItem>())
	//				{
	//					GetPreviewScene()->DeselectAll();
	//				}
	//			}
	//			PreviewComponent->PostInitMeshObject(PreviewComponent->MeshObject);
	//		}
	//	}
	//}
	//else
	//{
	//	if (GetPreviewScene().IsValid())
	//	{
	//		// Tell the preview scene if the user ctrl-clicked the selected bone/socket to de-select it
	//		GetPreviewScene()->DeselectAll();
	//	}
	//}
	//
	//TArrayView<TSharedPtr<INewtonModelPhysicsTreeItem>> ArrayView(SelectedItems);
	//OnSelectionChangedMulticast.Broadcast(ArrayView, SelectInfo);
}

bool FNewtonModelPhysicsTree::OnIsSelectableOrNavigable(TSharedPtr<INewtonModelPhysicsTreeItem> InItem) const
{
	return InItem && InItem->GetFilterResult() == Shown;
}

void FNewtonModelPhysicsTree::OnItemScrolledIntoView(TSharedPtr<INewtonModelPhysicsTreeItem> InItem, const TSharedPtr<ITableRow>& InWidget)
{
	check(0);
	//if (DeferredRenameRequest.IsValid())
	//{
	//	DeferredRenameRequest->RequestRename();
	//	DeferredRenameRequest.Reset();
	//}
}

void FNewtonModelPhysicsTree::OnTreeDoubleClick(TSharedPtr<INewtonModelPhysicsTreeItem> InItem)
{
	InItem->OnItemDoubleClicked();
}

void FNewtonModelPhysicsTree::SetTreeItemExpansionRecursive(TSharedPtr<INewtonModelPhysicsTreeItem> TreeItem, bool bInExpansionState) const
{
	SkeletonTreeView->SetItemExpansion(TreeItem, bInExpansionState);

	// Recursively go through the children.
	for (auto It = TreeItem->GetChildren().CreateIterator(); It; ++It)
	{
		SetTreeItemExpansionRecursive(*It, bInExpansionState);
	}
}

//TSharedPtr< SWidget > FNewtonModelPhysicsTree::CreateContextMenu()
//{
//	const FNewtonModelPhysicsTreeCommands& Actions = FNewtonModelPhysicsTreeCommands::Get();
//
//	TArray<TSharedPtr<INewtonModelPhysicsTreeItem>> SelectedItems = SkeletonTreeView->GetSelectedItems();
//	FSkeletonTreeSelection BoneTreeSelection(SelectedItems);
//
//	const bool CloseAfterSelection = true;
//	FMenuBuilder MenuBuilder(CloseAfterSelection, UICommandList, Extenders);
//	{
//		if (BoneTreeSelection.HasSelectedOfType<FSkeletonTreeAttachedAssetItem>() || BoneTreeSelection.HasSelectedOfType<FSkeletonTreeSocketItem>() || BoneTreeSelection.HasSelectedOfType<FSkeletonTreeVirtualBoneItem>())
//		{
//			MenuBuilder.BeginSection("SkeletonTreeSelectedItemsActions", LOCTEXT("SelectedActions", "Selected Item Actions"));
//			MenuBuilder.AddMenuEntry(Actions.DeleteSelectedRows);
//			MenuBuilder.EndSection();
//		}
//
//
//		const bool bNeedsBoneActionsHeading = BoneTreeSelection.HasSelectedOfType<FSkeletonTreeBoneItem>() || BoneTreeSelection.HasSelectedOfType<FSkeletonTreeVirtualBoneItem>();
//
//		if (bNeedsBoneActionsHeading)
//		{
//			MenuBuilder.BeginSection("SkeletonTreeBonesAction", LOCTEXT("BoneActions", "Selected Bone Actions"));
//		}
//
//		const bool bHasBoneSelected = BoneTreeSelection.HasSelectedOfType<FSkeletonTreeBoneItem>();
//		const bool bHasVirtualBoneSelected = BoneTreeSelection.HasSelectedOfType<FSkeletonTreeVirtualBoneItem>();
//		if (bHasBoneSelected || bHasVirtualBoneSelected)
//		{
//			MenuBuilder.AddMenuEntry(Actions.CopyBoneNames);
//
//			if (bHasBoneSelected)
//			{
//				MenuBuilder.AddMenuEntry(Actions.ResetBoneTransforms);
//			}
//
//			if (BoneTreeSelection.IsSingleOfTypesSelected<FSkeletonTreeBoneItem, FSkeletonTreeVirtualBoneItem>() && bAllowSkeletonOperations)
//			{
//				MenuBuilder.AddMenuEntry(Actions.AddSocket);
//				MenuBuilder.AddMenuEntry(Actions.PasteSockets);
//				MenuBuilder.AddMenuEntry(Actions.PasteSocketsToSelectedBone);
//			}
//		}
//
//		if (bNeedsBoneActionsHeading)
//		{
//			if (bAllowSkeletonOperations)
//			{
//				MenuBuilder.AddSubMenu(LOCTEXT("AddVirtualBone", "Add Virtual Bone"),
//					LOCTEXT("AddVirtualBone_ToolTip", "Adds a virtual bone to the skeleton."),
//					FNewMenuDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder)
//						{
//							TSharedRef<SBoneTreeMenu> MenuContent = SSkeletonTree::CreateVirtualBoneMenu(this);
//							MenuBuilder.AddWidget(MenuContent, FText::GetEmpty(), true);
//						}));
//			}
//
//			MenuBuilder.EndSection();
//		}
//
//		if (bAllowSkeletonOperations)
//		{
//			if (BoneTreeSelection.HasSelectedOfType<FSkeletonTreeBoneItem>())
//			{
//				UBlendProfile* const SelectedBlendProfile = BlendProfilePicker->GetSelectedBlendProfile();
//				if (SelectedBlendProfile && BoneTreeSelection.IsSingleOfTypeSelected<FSkeletonTreeBoneItem>())
//				{
//					TSharedPtr<FSkeletonTreeBoneItem> BoneItem = BoneTreeSelection.GetSelectedItems<FSkeletonTreeBoneItem>()[0];
//
//					FName BoneName = BoneItem->GetAttachName();
//					const USkeleton& Skeleton = GetEditableSkeletonInternal()->GetSkeleton();
//					int32 BoneIndex = Skeleton.GetReferenceSkeleton().FindBoneIndex(BoneName);
//
//					float CurrentBlendScale = SelectedBlendProfile->GetBoneBlendScale(BoneIndex);
//
//					MenuBuilder.BeginSection("SkeletonTreeBlendProfileScales", LOCTEXT("BlendProfileContextOptions", "Blend Profile"));
//					{
//						FUIAction RecursiveSetScales;
//						RecursiveSetScales.ExecuteAction = FExecuteAction::CreateSP(this, &SSkeletonTree::RecursiveSetBlendProfileScales, CurrentBlendScale);
//
//						MenuBuilder.AddMenuEntry
//						(
//							FText::Format(LOCTEXT("RecursiveSetBlendScales_Label", "Recursively Set Blend Scales To {0}"), FText::AsNumber(CurrentBlendScale)),
//							LOCTEXT("RecursiveSetBlendScales_ToolTip", "Sets all child bones to use the same blend profile scale as the selected bone"),
//							FSlateIcon(),
//							RecursiveSetScales
//						);
//					}
//					MenuBuilder.EndSection();
//				}
//
//				if (IsShowingAdvancedOptions())
//				{
//					MenuBuilder.BeginSection("SkeletonTreeBoneTranslationRetargeting", LOCTEXT("BoneTranslationRetargetingHeader", "Bone Translation Retargeting"));
//					{
//						FUIAction RecursiveRetargetingSkeletonAction = FUIAction(FExecuteAction::CreateSP(this, &SSkeletonTree::SetBoneTranslationRetargetingModeRecursive, EBoneTranslationRetargetingMode::Skeleton));
//						FUIAction RecursiveRetargetingAnimationAction = FUIAction(FExecuteAction::CreateSP(this, &SSkeletonTree::SetBoneTranslationRetargetingModeRecursive, EBoneTranslationRetargetingMode::Animation));
//						FUIAction RecursiveRetargetingAnimationScaledAction = FUIAction(FExecuteAction::CreateSP(this, &SSkeletonTree::SetBoneTranslationRetargetingModeRecursive, EBoneTranslationRetargetingMode::AnimationScaled));
//						FUIAction RecursiveRetargetingAnimationRelativeAction = FUIAction(FExecuteAction::CreateSP(this, &SSkeletonTree::SetBoneTranslationRetargetingModeRecursive, EBoneTranslationRetargetingMode::AnimationRelative));
//						FUIAction RecursiveRetargetingOrientAndScaleAction = FUIAction(FExecuteAction::CreateSP(this, &SSkeletonTree::SetBoneTranslationRetargetingModeRecursive, EBoneTranslationRetargetingMode::OrientAndScale));
//
//						MenuBuilder.AddMenuEntry
//						(LOCTEXT("SetTranslationRetargetingSkeletonChildrenAction", "Recursively Set Translation Retargeting Skeleton")
//							, LOCTEXT("BoneTranslationRetargetingSkeletonToolTip", "Use translation from Skeleton.")
//							, FSlateIcon()
//							, RecursiveRetargetingSkeletonAction
//						);
//
//						MenuBuilder.AddMenuEntry
//						(LOCTEXT("SetTranslationRetargetingAnimationChildrenAction", "Recursively Set Translation Retargeting Animation")
//							, LOCTEXT("BoneTranslationRetargetingAnimationToolTip", "Use translation from animation.")
//							, FSlateIcon()
//							, RecursiveRetargetingAnimationAction
//						);
//
//						MenuBuilder.AddMenuEntry
//						(LOCTEXT("SetTranslationRetargetingAnimationScaledChildrenAction", "Recursively Set Translation Retargeting AnimationScaled")
//							, LOCTEXT("BoneTranslationRetargetingAnimationScaledToolTip", "Use translation from animation, scale length by Skeleton's proportions.")
//							, FSlateIcon()
//							, RecursiveRetargetingAnimationScaledAction
//						);
//
//						MenuBuilder.AddMenuEntry
//						(LOCTEXT("SetTranslationRetargetingAnimationRelativeChildrenAction", "Recursively Set Translation Retargeting AnimationRelative")
//							, LOCTEXT("BoneTranslationRetargetingAnimationRelativeToolTip", "Use relative translation from animation similar to an additive animation.")
//							, FSlateIcon()
//							, RecursiveRetargetingAnimationRelativeAction
//						);
//
//						MenuBuilder.AddMenuEntry
//						(LOCTEXT("SetTranslationRetargetingOrientAndScaleChildrenAction", "Recursively Set Translation Retargeting OrientAndScale")
//							, LOCTEXT("BoneTranslationRetargetingOrientAndScaleToolTip", "Orient And Scale Translation.")
//							, FSlateIcon()
//							, RecursiveRetargetingOrientAndScaleAction
//						);
//					}
//					MenuBuilder.EndSection();
//				}
//			}
//		}
//
//		if (bAllowMeshOperations)
//		{
//			MenuBuilder.BeginSection("SkeletonTreeBoneReductionForLOD", LOCTEXT("BoneReductionHeader", "LOD Bone Reduction"));
//			{
//				MenuBuilder.AddSubMenu(
//					LOCTEXT("SkeletonTreeBoneReductionForLOD_RemoveSelectedFromLOD", "Remove Selected..."),
//					FText::GetEmpty(),
//					FNewMenuDelegate::CreateStatic(&SSkeletonTree::CreateMenuForBoneReduction, this, LastCachedLODForPreviewMeshComponent, true)
//				);
//
//				MenuBuilder.AddSubMenu(
//					LOCTEXT("SkeletonTreeBoneReductionForLOD_RemoveChildrenFromLOD", "Remove Children..."),
//					FText::GetEmpty(),
//					FNewMenuDelegate::CreateStatic(&SSkeletonTree::CreateMenuForBoneReduction, this, LastCachedLODForPreviewMeshComponent, false)
//				);
//			}
//			MenuBuilder.EndSection();
//		}
//
//		if (bAllowSkeletonOperations)
//		{
//			if (BoneTreeSelection.HasSelectedOfType<FSkeletonTreeVirtualBoneItem>())
//			{
//				MenuBuilder.BeginSection("SkeletonTreeVirtualBoneActions", LOCTEXT("VirtualBoneActions", "Selected Virtual Bone Actions"));
//
//				if (BoneTreeSelection.IsSingleOfTypeSelected<FSkeletonTreeVirtualBoneItem>())
//				{
//					MenuBuilder.AddMenuEntry(FGenericCommands::Get().Rename, NAME_None, LOCTEXT("RenameVirtualBone_Label", "Rename Virtual Bone"), LOCTEXT("RenameVirtualBone_Tooltip", "Rename this virtual bone"));
//				}
//
//				MenuBuilder.EndSection();
//			}
//
//			if (BoneTreeSelection.HasSelectedOfType<FSkeletonTreeSocketItem>())
//			{
//				MenuBuilder.BeginSection("SkeletonTreeSocketsActions", LOCTEXT("SocketActions", "Selected Socket Actions"));
//
//				MenuBuilder.AddMenuEntry(Actions.CopySockets);
//
//				if (BoneTreeSelection.IsSingleOfTypeSelected<FSkeletonTreeSocketItem>())
//				{
//					MenuBuilder.AddMenuEntry(FGenericCommands::Get().Rename, NAME_None, LOCTEXT("RenameSocket_Label", "Rename Socket"), LOCTEXT("RenameSocket_Tooltip", "Rename this socket"));
//
//					TSharedPtr<FSkeletonTreeSocketItem> SocketItem = BoneTreeSelection.GetSelectedItems<FSkeletonTreeSocketItem>()[0];
//
//					if (SocketItem->IsSocketCustomized() && SocketItem->GetParentType() == ESocketParentType::Mesh)
//					{
//						MenuBuilder.AddMenuEntry(Actions.RemoveMeshSocket);
//					}
//
//					// If the socket is on the skeleton, we have a valid mesh
//					// and there isn't one of the same name on the mesh, we can customize it
//					if (SocketItem->CanCustomizeSocket())
//					{
//						if (SocketItem->GetParentType() == ESocketParentType::Skeleton)
//						{
//							MenuBuilder.AddMenuEntry(Actions.CreateMeshSocket);
//						}
//						else if (SocketItem->GetParentType() == ESocketParentType::Mesh)
//						{
//							// If a socket is on the mesh only, then offer to promote it to the skeleton
//							MenuBuilder.AddMenuEntry(Actions.PromoteSocketToSkeleton);
//						}
//					}
//				}
//
//				MenuBuilder.EndSection();
//			}
//		}
//
//		if (BoneTreeSelection.HasSelectedOfType<FSkeletonTreeBoneItem>() || BoneTreeSelection.HasSelectedOfType<FSkeletonTreeSocketItem>())
//		{
//			MenuBuilder.BeginSection("SkeletonTreeAttachedAssets", LOCTEXT("AttachedAssetsActionsHeader", "Attached Assets Actions"));
//
//			if (BoneTreeSelection.IsSingleItemSelected())
//			{
//				MenuBuilder.AddSubMenu(LOCTEXT("AttachNewAsset", "Add Preview Asset"),
//					LOCTEXT("AttachNewAsset_ToolTip", "Attaches an asset to this part of the skeleton. Assets can also be dragged onto the skeleton from a content browser to attach"),
//					FNewMenuDelegate::CreateSP(this, &SSkeletonTree::FillAttachAssetSubmenu, BoneTreeSelection.GetSingleSelectedItem()));
//			}
//
//			FUIAction RemoveAllAttachedAssets = FUIAction(FExecuteAction::CreateSP(this, &SSkeletonTree::OnRemoveAllAssets),
//				FCanExecuteAction::CreateSP(this, &SSkeletonTree::CanRemoveAllAssets));
//
//			MenuBuilder.AddMenuEntry(LOCTEXT("RemoveAllAttachedAssets", "Remove All Attached Assets"),
//				LOCTEXT("RemoveAllAttachedAssets_ToolTip", "Removes all the attached assets from the skeleton and mesh."),
//				FSlateIcon(), RemoveAllAttachedAssets);
//
//			MenuBuilder.EndSection();
//		}
//
//		// Add an empty section so the menu can be extended when there are no optionally-added entries
//		MenuBuilder.BeginSection("SkeletonTreeContextMenu");
//		MenuBuilder.EndSection();
//	}
//
//	return MenuBuilder.MakeWidget();
//}

void FNewtonModelPhysicsTree::CreateTreeColumns()
{
	TArray<FName> HiddenColumnsList;
	//HiddenColumnsList.Add(ISkeletonTree::Columns::Retargeting);
	//HiddenColumnsList.Add(ISkeletonTree::Columns::BlendProfile);
	//HiddenColumnsList.Add(ISkeletonTree::Columns::DebugVisualization);

	TSharedRef<SHeaderRow> treeHeaderRow =
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
		;


	{
		FScopedSavedSelection ScopedSelection(SharedThis(this));

			SkeletonTreeView = SNew(FNewtonModelPhysicsTreeView<TSharedPtr<INewtonModelPhysicsTreeItem>>)
			.TreeItemsSource(&FilteredItems)
			.OnGenerateRow(this, &FNewtonModelPhysicsTree::MakeTreeRowWidget)
			.OnGetChildren(this, &FNewtonModelPhysicsTree::GetFilteredChildren)
			//.OnContextMenuOpening(this, &FNewtonModelPhysicsTree::CreateContextMenu)
			.OnSelectionChanged(this, &FNewtonModelPhysicsTree::OnSelectionChanged)
			.OnIsSelectableOrNavigable(this, &FNewtonModelPhysicsTree::OnIsSelectableOrNavigable)
			.OnItemScrolledIntoView(this, &FNewtonModelPhysicsTree::OnItemScrolledIntoView)
			.OnMouseButtonDoubleClick(this, &FNewtonModelPhysicsTree::OnTreeDoubleClick)
			.OnSetExpansionRecursive(this, &FNewtonModelPhysicsTree::SetTreeItemExpansionRecursive)
			.HighlightParentNodesForSelection(true)
			.HeaderRow
			(
				treeHeaderRow
			);
		
			TreeHolder->ClearChildren();
			TreeHolder->AddSlot()
			[
				SNew(SScrollBorder, SkeletonTreeView.ToSharedRef())
				[
					SkeletonTreeView.ToSharedRef()
				]
			];
	}

	CreateFromSkeleton();
}


void FNewtonModelPhysicsTree::SetInitialExpansionState()
{
	//for (TSharedPtr<INewtonModelPhysicsTreeItem>& Item : LinearItems)
	for (int i = 0; i < LinearItems.Num(); ++i)
	{
		TSharedPtr<INewtonModelPhysicsTreeItem>& item = LinearItems[i];
		SkeletonTreeView->SetItemExpansion(item, item->IsInitiallyExpanded());
	}
}

void FNewtonModelPhysicsTree::HandleTreeRefresh()
{
	SkeletonTreeView->RequestTreeRefresh();
}

void FNewtonModelPhysicsTree::ApplyFilter()
{
	TextFilterPtr->SetFilterText(FilterText);

	FilteredItems.Empty();

	FNewtonModelPhysicsTreeFilterArgs FilterArgs(!FilterText.IsEmpty() ? TextFilterPtr : nullptr);
	//FilterArgs.bFlattenHierarchyOnFilter = GetDefault<UPersonaOptions>()->bFlattenSkeletonHierarchyWhenFiltering;
	FilterArgs.bFlattenHierarchyOnFilter = false;
	Builder->Filter(FilterArgs, Items, FilteredItems);
	
	if (!FilterText.IsEmpty())
	{
		check(0);
		//for (TSharedPtr<INewtonModelPhysicsTreeItem>& Item : LinearItems)
		for (int i = 0; i < LinearItems.Num(); ++i)
		{
			const TSharedPtr<INewtonModelPhysicsTreeItem>& Item = LinearItems[i];
	//		if (Item->GetFilterResult() > ESkeletonTreeFilterResult::Hidden)
	//		{
				SkeletonTreeView->SetItemExpansion(Item, true);
	//		}
		}
	}
	else
	{
		SetInitialExpansionState();
	}
	
	HandleTreeRefresh();
}

void FNewtonModelPhysicsTree::CreateFromSkeleton()
{
	// save selected items
	FScopedSavedSelection ScopedSelection(SharedThis(this));

	Items.Empty();
	LinearItems.Empty();
	FilteredItems.Empty();

	FFNewtonModelPhysicsTreeBuilderOutput output(Items, LinearItems);
	Builder->Build(output);
	ApplyFilter();
}

#else

#include "SPositiveActionButton.h"
#include "UICommandList_Pinnable.h"
#include "Widgets/Views/STableRow.h"

#include "NewtonModelEditor.h"
#include "IPinnedCommandList.h"
#include "NewtonModelEditorCommon.h"
#include "ndTree/NewtonModelPhysicsTreeItem.h"
#include "ndTree/NewtonModelPhysicsTreeItemRow.h"
#include "ndTree/NewtonModelPhysicsTreeCommands.h"

#define LOCTEXT_NAMESPACE "FNewtonModelPhysicsTree"

FName FNewtonModelPhysicsTree::m_treeColumnName(TEXT("node name"));
FName FNewtonModelPhysicsTree::m_menuName(TEXT("NewtonModelPhysicsTreeMenu"));
FName FNewtonModelPhysicsTree::m_contextName(TEXT("NewtonModelPhysicsTreeMenuItem"));


class FNewtonModelPhysicsTree::FScopedSavedSelection
{
	public:
	FScopedSavedSelection(TSharedPtr<FNewtonModelPhysicsTree> tree)
		:SkeletonTree(tree)
	{
		// record selected items
		if (SkeletonTree.IsValid() && tree->m_treeView.IsValid())
		{
			TArray<TSharedPtr<FNewtonModelPhysicsTreeItem>> SelectedItems = tree->m_treeView->GetSelectedItems();
			for (const TSharedPtr<FNewtonModelPhysicsTreeItem>& SelectedItem : SelectedItems)
			{
				check(0);
				//SavedSelections.Add({ SelectedItem->GetRowItemName(), SelectedItem->GetTypeName(), SelectedItem->GetObject() });
			}
		}
	}

	~FScopedSavedSelection()
	{
		if (SkeletonTree.IsValid() && SkeletonTree->m_treeView.IsValid())
		{
			// restore selection
			for (const TSharedPtr<FNewtonModelPhysicsTreeItem>& Item : SkeletonTree->m_items)
			{
				check(0);
				//if (Item->GetFilterResult() != Hidden)
				//{
				//	for (FSavedSelection& SavedSelection : SavedSelections)
				//	{
				//		if (Item->GetRowItemName() == SavedSelection.ItemName && Item->GetTypeName() == SavedSelection.ItemType && Item->GetObject() == SavedSelection.ItemObject)
				//		{
				//			SkeletonTree->SkeletonTreeView->SetItemSelection(Item, true);
				//			break;
				//		}
				//	}
				//}
			}
		}
	}

	struct FSavedSelection
	{
		/** Name of the selected item */
		FName ItemName;

		/** Type of the selected item */
		FName ItemType;

		/** Object of selected item */
		UObject* ItemObject;
	};

	TArray<FSavedSelection> SavedSelections;
	TSharedPtr<FNewtonModelPhysicsTree> SkeletonTree;
};


FNewtonModelPhysicsTree::FNewtonModelPhysicsTree()
{
}

FNewtonModelPhysicsTree::~FNewtonModelPhysicsTree()
{
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
	for (int i = 0; i < m_items.Num(); ++i)
	{
		TSharedPtr<FNewtonModelPhysicsTreeItem> child(m_items[i]);
		if (child->m_parent == item)
		{
			outChildren.Push(child);
		}
	}
}

void FNewtonModelPhysicsTree::OnSelectionChanged(TSharedPtr<FNewtonModelPhysicsTreeItem> item, ESelectInfo::Type selectInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
}

void FNewtonModelPhysicsTree::OnCreateItemBodyRoot()
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
}

void FNewtonModelPhysicsTree::OnCreateItemBodyLink()
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
}

void FNewtonModelPhysicsTree::OnCreateItemShapeBox()
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
}

void FNewtonModelPhysicsTree::OnCreateItemShapeShere()
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
}

void FNewtonModelPhysicsTree::OnCreateItemJointHinge()
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
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

	FToolMenuSection& rootSection = menu->AddSection("Root Rigid Body", LOCTEXT("RigidBody", "Rigid"));
	rootSection.AddMenuEntry(Actions.AddBodyRoot);

	FToolMenuSection& linkSection = menu->AddSection("Link Rigid Body", LOCTEXT("RigidBody", "Rigid"));
	linkSection.AddMenuEntry(Actions.AddBodyLink);

	FToolMenuSection& shapeSection = menu->AddSection("Collisions Shape", LOCTEXT("Collisions", "Collision"));
	shapeSection.AddMenuEntry(Actions.AddShapeBox);
	shapeSection.AddMenuEntry(Actions.AddShapeSphere);

	FToolMenuSection& jointSection = menu->AddSection("Constraint Joints", LOCTEXT("Constraint", "Constraint"));
	jointSection.AddMenuEntry(Actions.AddJointHinge);
}

void FNewtonModelPhysicsTree::BindCommands()
{
	// This should not be called twice on the same instance
	check(!UICommandList.IsValid());

	UICommandList = MakeShareable(new FUICommandList_Pinnable);

	FUICommandList_Pinnable& commandList = *UICommandList;

	// Grab the list of menu commands to bind...
	const FNewtonModelPhysicsTreeCommands& menuActions = FNewtonModelPhysicsTreeCommands::Get();

	commandList.MapAction(menuActions.AddBodyRoot, FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCreateItemBodyRoot));
	commandList.MapAction(menuActions.AddBodyLink, FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCreateItemBodyLink));
	commandList.MapAction(menuActions.AddShapeBox, FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCreateItemShapeBox));
	commandList.MapAction(menuActions.AddJointHinge, FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCreateItemJointHinge));
	commandList.MapAction(menuActions.AddShapeSphere, FExecuteAction::CreateSP(this, &FNewtonModelPhysicsTree::OnCreateItemShapeShere));

	PinnedCommands->BindCommandList(UICommandList.ToSharedRef());
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

		.TreeItemsSource(&m_roots)
		.OnGenerateRow(this, &FNewtonModelPhysicsTree::OnGenerateRow)
		.OnGetChildren(this, &FNewtonModelPhysicsTree::OnGetChildren)
		.OnSelectionChanged(this, &FNewtonModelPhysicsTree::OnSelectionChanged)
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

void FNewtonModelPhysicsTree::BuildTree()
{
	m_items.Empty();
	m_roots.Empty();

	UNewtonModelNode* nodeStack[512];
	TSharedPtr<FNewtonModelPhysicsTreeItem> parentStack[512];

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
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemBody(nullptr, node->Name)));
			m_items.Add(item);
			m_roots.Add(item);
			parent = item;
		}
		else if (Cast<UNewtonModelNodeRigidBody>(node))
		{
			//UE_LOG(LogTemp, Warning, TEXT("TODO: remember emit body:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemBody(parent, node->Name)));
			m_items.Add(item);
			parent = item;
		}
		else if (Cast<UNewtonModelNodeJoint>(node))
		{
			//UE_LOG(LogTemp, Warning, TEXT("TODO: remember emit body:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemJoint(parent, node->Name)));
			m_items.Add(item);
			parent = item;
		}
		else if (Cast<UNewtonModelNodeCollision>(node))
		{
			//UE_LOG(LogTemp, Warning, TEXT("TODO: remember emit body:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
			TSharedRef<FNewtonModelPhysicsTreeItem> item(MakeShareable(new FNewtonModelPhysicsTreeItemShape(parent, node->Name)));
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

	//Refresh
	if (m_treeView.IsValid())
	{
		for (int i = 0; i < m_items.Num(); ++i)
		{
			TSharedPtr<FNewtonModelPhysicsTreeItem>& item = m_items[i];
			m_treeView->SetItemExpansion(item, true);
		}

		m_treeView->RequestTreeRefresh();
	}
}

#endif

#undef LOCTEXT_NAMESPACE