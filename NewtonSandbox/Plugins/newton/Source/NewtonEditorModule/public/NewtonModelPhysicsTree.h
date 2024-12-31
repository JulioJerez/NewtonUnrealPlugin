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


#pragma once

#include "CoreMinimal.h"

#include "Widgets/SCompoundWidget.h"

#define TREE_STACK_DEPTH	512

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

	class UniqueNameId : public TSet<FName>
	{
		public: 
		UniqueNameId();

		void Reset();
		FName GetUniqueName(const FName name);

		int m_enumerator;
	};

	FNewtonModelPhysicsTree();
	~FNewtonModelPhysicsTree();

	// Widget constructor
	void Construct(const FArguments& args, FNewtonModelEditor* const editor);

	void SaveModel();
	void ResetSkeletalMesh();
	void NormalizeTransformsScale();
	FName GetUniqueName(const FName name);
	TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>> GetItems() const;
	void DetailViewPropertiesUpdated(const FPropertyChangedEvent& event);
	void DetailViewBoneSelectedUpdated(const TSharedPtr<ISkeletonTreeItem>& item);

	bool HaveSelection() const;
	bool ShouldDrawWidget() const;
	FMatrix GetWidgetMatrix() const;
	void DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const;
	void ApplyDeltaTransform(const FVector& inDrag, const FRotator& inRot, const FVector& inScale) const;
	
	//* SWidget overrides
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	private:
	void BuildTree();
	void RefreshView();
	void BindCommands();
	void RegisterNewMenu();
	void RebuildAcyclicTree();
	void AddLoopRow(const TSharedRef<FNewtonModelPhysicsTreeItem>& item);
	void AddShapeRow(const TSharedRef<FNewtonModelPhysicsTreeItem>& item);
	void AddJointRow(const TSharedRef<FNewtonModelPhysicsTreeItem>& iItem);

	// delegates
	bool OnCanAddChildRow() const;
	bool CanDeleteSelectedRow() const;

	void OnAddShapeBoxRow();
	void OnAddJointHingeRow();
	void OnAddJointSliderRow();
	void OnAddShapeWheelRow();
	void OnAddShapeSphereRow();
	void OnAddShapeCapsuleRow();
	void OnAddShapeCylinderRow();
	void OnAddShapeConvexhullRow();
	void OnAddJointLoopEffector6dofRow();

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
	UniqueNameId m_uniqueNames;
	FNewtonModelEditor* m_editor;
	TArray<TSharedPtr<FNewtonModelPhysicsTreeItem>> m_root;
	
	TSet<TSharedPtr<FNewtonModelPhysicsTreeItem>> m_items;
	TSharedPtr<FNewtonModelPhysicsTreeItem> m_selectedItem;
	FNewtonModelPhysicsTreeItemAcyclicGraph* m_acyclicGraph;
	TSharedPtr<STreeView<TSharedPtr<FNewtonModelPhysicsTreeItem>>> m_treeView;

	// Commands that are bound to delegates
	TSharedPtr<FUICommandList_Pinnable> m_uiCommandList;

	// Pinned commands panel
	TSharedPtr<IPinnedCommandList> m_pinnedCommands;

	bool m_boneMappingMode;

	static FName m_menuName;
	static FName m_contextName;
	static FName m_treeColumnName;
};
