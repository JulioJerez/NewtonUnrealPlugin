// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Widgets/SCompoundWidget.h"


class FNewtonModelEditor;
class IPinnedCommandList;
class FUICommandList_Pinnable;
class FNewtonModelPhysicsTreeItem;
class FNewtonModelPhysicsTreeItemAcyclicGraph;

class FNewtonModelPhysicsTree : public SCompoundWidget
{
	//class FScopedSavedSelection;
	public:
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
	void RefreshView();
	void BindCommands();
	void RegisterNewMenu();
	void BuildAcyclicTree();

	// delegates
	void OnCreateItemBodyRoot();
	void OnCreateItemBodyLink();
	void OnCreateItemShapeBox();
	void OnCreateItemShapeShere();
	void OnCreateItemJointHinge();
	void OnDeleteSelectedRows();
	bool CanDeleteSelectedRows() const;

	TSharedRef<SWidget> OnCreateNewMenuWidget();
	

	void OnSelectionChanged(TSharedPtr<FNewtonModelPhysicsTreeItem> item, ESelectInfo::Type selectInfo);
	void OnGetChildren(TSharedPtr<FNewtonModelPhysicsTreeItem>, TArray<TSharedPtr<FNewtonModelPhysicsTreeItem>>& outChildren);

	TSharedPtr< SWidget > CreateContextMenu();
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FNewtonModelPhysicsTreeItem> item, const TSharedRef<STableViewBase>& ownerTable);

	FNewtonModelEditor* m_editor;
	TArray<TSharedPtr<FNewtonModelPhysicsTreeItem>> m_root;
	TSharedPtr<FNewtonModelPhysicsTreeItem> m_selectedItem;
	FNewtonModelPhysicsTreeItemAcyclicGraph* m_acyclicGraph;
	TSharedPtr<STreeView<TSharedPtr<FNewtonModelPhysicsTreeItem>>> m_treeView;
	TMap<FNewtonModelPhysicsTreeItem*, TSharedPtr<FNewtonModelPhysicsTreeItem>> m_items;

	// Commands that are bound to delegates
	TSharedPtr<FUICommandList_Pinnable> UICommandList;

	// Pinned commands panel
	TSharedPtr<IPinnedCommandList> PinnedCommands;

	static FName m_menuName;
	static FName m_contextName;
	static FName m_treeColumnName;
};



