// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Widgets/SCompoundWidget.h"


#if 0
#include "ndTree/NewtonModelPhysicsTreeBuilder.h"

class IPinnedCommandList;
class FUICommandList_Pinnable;
class INewtonModelPhysicsTreeItem;
class FNewtonModelPhysicsTreeEditableSkeleton;


class FNewtonModelPhysicsTreeArgs
{
	public:
	FNewtonModelPhysicsTreeArgs()
		//: Mode(ESkeletonTreeMode::Editor)
		:ContextName(TEXT("PhysicsTree"))
		//, bShowBlendProfiles(true)
		//, bShowFilterMenu(true)
		//, bShowDebugVisualizationOptions(false)
		//, bAllowMeshOperations(true)
		//, bAllowSkeletonOperations(true)
		//, bHideBonesByDefault(false)
	{
	}

	///** Delegate called by the tree when a socket is selected */
	//FOnSkeletonTreeSelectionChanged OnSelectionChanged;
	//
	///** Delegate that allows custom filtering text to be shown on the filter button */
	//FOnGetFilterText OnGetFilterText;
	//
	///** Optional preview scene that we can pair with */
	//TSharedPtr<IPersonaPreviewScene> PreviewScene;
	//
	///** The mode that this skeleton tree is in */
	//ESkeletonTreeMode Mode;
	
	///** Whether to show the blend profiles editor for the skeleton being displayed */
	//bool bShowBlendProfiles;
	//
	///** Whether to show the filter menu to allow filtering of active bones, sockets etc. */
	//bool bShowFilterMenu;
	//
	///** Whether to show the filter option to allow filtering of debug draw elements in the viewport. */
	//bool bShowDebugVisualizationOptions;
	//
	///** Whether to allow operations that modify the mesh */
	//bool bAllowMeshOperations;
	//
	///** Whether to allow operations that modify the skeleton */
	//bool bAllowSkeletonOperations;
	//
	///** Whether to hide bones by default */
	//bool bHideBonesByDefault;

	/** Context name used to persist settings */
	FName ContextName;

	/** Optional builder to allow for custom tree construction */
	TSharedPtr<FNewtonModelPhysicsTreeBuilder> Builder;

	/** Menu extenders applied to context and filter menus */
	TSharedPtr<FExtender> Extenders;
};


class FNewtonModelPhysicsTree : public SCompoundWidget
{
	public:
	class FScopedSavedSelection;
	template <typename ItemType> class FNewtonModelPhysicsTreeView;

	SLATE_BEGIN_ARGS(FNewtonModelPhysicsTree)
		: _IsEditable(true)
		{
		}

		SLATE_ATTRIBUTE(bool, IsEditable)

		SLATE_ARGUMENT(TSharedPtr<FNewtonModelPhysicsTreeBuilder>, Builder)

	SLATE_END_ARGS()

	FNewtonModelPhysicsTree();
	~FNewtonModelPhysicsTree();

	void Construct(const FArguments& InArgs, const TSharedRef<FNewtonModelPhysicsTreeEditableSkeleton>& InEditableSkeleton, const FNewtonModelPhysicsTreeArgs& InSkeletonTreeArgs);

	void BindCommands();
	void RegisterNewMenu();

	/** Apply filtering to the tree */
	void ApplyFilter();
	
	/** Set the initial expansion state of the tree items */
	void SetInitialExpansionState();

	/** Delegate handler for when the tree needs refreshing */
	void HandleTreeRefresh();

	// Delegates 
	/** Called to display the add new menu */
	TSharedRef< SWidget > CreateNewMenuWidget();
	ENewtonModelPhysicsTreeFilterResult HandleFilterSkeletonTreeItem(const FNewtonModelPhysicsTreeFilterArgs& args, const TSharedPtr<INewtonModelPhysicsTreeItem>& item);

	void OnCreateRootBone();

	/** Creates the tree control and then populates */
	void CreateTreeColumns();

	/** Function to build the skeleton tree widgets from the source skeleton tree */
	void CreateFromSkeleton();

	void OnFilterTextChanged(const FText& SearchText);
	TSharedRef<FNewtonModelPhysicsTreeEditableSkeleton> GetEditableSkeleton() const;

	/** Create a widget for an entry in the tree from an info */
	TSharedRef<ITableRow> MakeTreeRowWidget(TSharedPtr<INewtonModelPhysicsTreeItem> InInfo, const TSharedRef<STableViewBase>& OwnerTable);

	/** Get all children for a given entry in the list */
	void GetFilteredChildren(TSharedPtr<INewtonModelPhysicsTreeItem> InInfo, TArray< TSharedPtr<INewtonModelPhysicsTreeItem> >& OutChildren);

	/** Called to display context menu when right clicking on the widget */
	TSharedPtr< SWidget > CreateContextMenu();

	/** Callback function to be called when selection changes in the tree view widget. */
	void OnSelectionChanged(TSharedPtr<INewtonModelPhysicsTreeItem> Selection, ESelectInfo::Type SelectInfo);

	/** Callback function to be called when selection changes to check if the next item is selectable or navigable. */
	bool OnIsSelectableOrNavigable(TSharedPtr<INewtonModelPhysicsTreeItem> InItem) const;

	/** Callback when an item is scrolled into view, handling calls to rename items */
	void OnItemScrolledIntoView(TSharedPtr<INewtonModelPhysicsTreeItem> InItem, const TSharedPtr<ITableRow>& InWidget);

	/** Callback for when the user double-clicks on an item in the tree */
	void OnTreeDoubleClick(TSharedPtr<INewtonModelPhysicsTreeItem> InItem);

	/** Handle recursive expansion/contraction of the tree */
	void SetTreeItemExpansionRecursive(TSharedPtr<INewtonModelPhysicsTreeItem> TreeItem, bool bInExpansionState) const;

	public:
	TWeakPtr<FNewtonModelPhysicsTreeEditableSkeleton> EditableSkeleton;

	/** Context name used to persist settings */
	FName ContextName;

	/** Pinned commands panel */
	TSharedPtr<IPinnedCommandList> PinnedCommands;

	/** Commands that are bound to delegates*/
	TSharedPtr<FUICommandList_Pinnable> UICommandList;

	/** Extenders for menus */
	TSharedPtr<FExtender> Extenders;

	/** Widget user to hold the skeleton tree */
	TSharedPtr<SOverlay> TreeHolder;

	/** Filtered view of the skeleton tree. This is what is actually used in the tree widget */
	TArray<TSharedPtr<INewtonModelPhysicsTreeItem>> FilteredItems;

	/** Widget used to display the skeleton hierarchy */
	TSharedPtr<STreeView<TSharedPtr<INewtonModelPhysicsTreeItem>>> SkeletonTreeView;

	/** Current text typed into NameFilterBox */
	FText FilterText;

	/** A tree of unfiltered items */
	TArray<TSharedPtr<INewtonModelPhysicsTreeItem>> Items;

	/** A "mirror" of the tree as a flat array for easier searching */
	TArray<TSharedPtr<INewtonModelPhysicsTreeItem>> LinearItems;

	/** Optional builder to allow for custom tree construction */
	TSharedPtr<FNewtonModelPhysicsTreeBuilder> Builder;

	/** Compiled filter search terms. */
	TSharedPtr<class FTextFilterExpressionEvaluator> TextFilterPtr;

	static FName m_menuName;
	static FName m_treeColumnName;
};

#else

class FNewtonModelEditor;
class IPinnedCommandList;
class FUICommandList_Pinnable;
class FNewtonModelPhysicsTreeItem;

class FNewtonModelPhysicsTree : public SCompoundWidget
{
	public:
	//I gave not idea what this moronic shit is
	SLATE_BEGIN_ARGS(FNewtonModelPhysicsTree)
	{
	}

	SLATE_END_ARGS()


	FNewtonModelPhysicsTree();
	~FNewtonModelPhysicsTree();

	// Widget constructor
	void Construct(const FArguments& args, FNewtonModelEditor* const editor);
	
	// Rebuilds the category tree from scratch 
	void BuildTree();

	//* SWidget overrides
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	private:
	void BindCommands();
	void RegisterNewMenu();

	// delegates
	void OnCreateItemBodyRoot();
	void OnCreateItemBodyLink();
	void OnCreateItemShapeBox();
	void OnCreateItemShapeShere();
	void OnCreateItemJointHinge();
	TSharedRef<SWidget> OnCreateNewMenuWidget();


	void OnSelectionChanged(TSharedPtr<FNewtonModelPhysicsTreeItem> item, ESelectInfo::Type selectInfo);
	void OnGetChildren(TSharedPtr<FNewtonModelPhysicsTreeItem>, TArray<TSharedPtr<FNewtonModelPhysicsTreeItem>>& outChildren);
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FNewtonModelPhysicsTreeItem> item, const TSharedRef<STableViewBase>& ownerTable);

	FNewtonModelEditor* m_editor;
	TArray<TSharedPtr<FNewtonModelPhysicsTreeItem>> m_roots;
	TArray<TSharedPtr<FNewtonModelPhysicsTreeItem>> m_items;
	TSharedPtr<STreeView<TSharedPtr<FNewtonModelPhysicsTreeItem>>> m_treeView;

	// Commands that are bound to delegates
	TSharedPtr<FUICommandList_Pinnable> UICommandList;

	// Pinned commands panel
	TSharedPtr<IPinnedCommandList> PinnedCommands;

	static FName m_menuName;
	static FName m_contextName;
};


#endif
