// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Widgets/SCompoundWidget.h"


class ISkeletonTreeItem;
class FNewtonModelEditor;
class IPinnedCommandList;
class FUICommandList_Pinnable;
class FNewtonModelPhysicsTreeItem;
class FNewtonModelPhysicsTreeItemAcyclicGraph;

class FNewtonModelPhysicsTree : public SCompoundWidget
{
	public:
	SLATE_BEGIN_ARGS(FNewtonModelPhysicsTree)
	{
	}
	SLATE_END_ARGS()

	FNewtonModelPhysicsTree();
	~FNewtonModelPhysicsTree();

	// Widget constructor
	void Construct(const FArguments& args, FNewtonModelEditor* const editor);

	void SaveModel();
	void ResetSkeletalMesh();
	void DetailViewPropertiesUpdated(const FPropertyChangedEvent& event);
	void DetailViewBoneSelectedUpdated(const TSharedPtr<ISkeletonTreeItem>& item);
	
	//* SWidget overrides
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	private:
	void BuildTree();
	void RefreshView();
	void BindCommands();
	void RegisterNewMenu();
	void RebuildAcyclicTree();
	FName GetUniqueName(const FName name);
	void AddShapeRow(const TSharedRef<FNewtonModelPhysicsTreeItem>& item);
	void AddJointRow(const TSharedRef<FNewtonModelPhysicsTreeItem>& iItem);

	// delegates
	bool OnCanAddChildRow() const;
	bool CanDeleteSelectedRow() const;

	void OnAddShapeBoxRow();
	void OnAddJointHingeRow();
	void OnAddShapeSphereRow();
	void OnDeleteSelectedRow();
	void OnResetSelectedBone();
	void OnToggleShapeVisibility();
	void OnToggleJointVisibility();

	TSharedRef<SWidget> OnCreateNewMenuWidget();
	void OnSelectionChanged(TSharedPtr<FNewtonModelPhysicsTreeItem> item, ESelectInfo::Type selectInfo);
	void OnGetChildren(TSharedPtr<FNewtonModelPhysicsTreeItem>, TArray<TSharedPtr<FNewtonModelPhysicsTreeItem>>& outChildren);

	TSharedPtr<SWidget> CreateContextMenu();
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FNewtonModelPhysicsTreeItem> item, const TSharedRef<STableViewBase>& ownerTable);

	FName m_oldSelectedName;
	TSet<FName> m_uniqueNames;
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
	

	bool m_hideShapes;
	bool m_hideJoints;
	bool m_modelIsDirty;
	int m_nameId;

	static FName m_menuName;
	static FName m_contextName;
	static FName m_treeColumnName;
};



