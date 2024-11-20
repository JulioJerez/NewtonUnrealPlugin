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
#include "SGraphPanel.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class UNewtonModel;
class UNewtonModelGraphNode;

class ISkeletonTree;
class IPersonaToolkit;
class ISkeletonTreeItem;
class IDetailLayoutBuilder;


/**
 * 
 */
class NEWTONEDITORMODULE_API NewtonModelEditor: public FWorkflowCentricApplication
{
	public:
	NewtonModelEditor();
	~NewtonModelEditor();

	// editor methods
	void CreateSkeletalMeshEditor();
	UEdGraph* GetGraphEditor() const;
	UNewtonModel* GetNewtonModel() const;
	void SetNewtonModel(TObjectPtr<UNewtonModel> model);
	void SetWorkingGraphUi(TSharedPtr<SGraphEditor> workingGraph);
	void SetSelectedNodeDetailView(TSharedPtr<IDetailsView> detailData);

	void OnGraphChanged(const FEdGraphEditAction& action);
	void OnGraphSelectionChanged(const FGraphPanelSelectionSet& selection);
	UNewtonModelGraphNode* GetSelectedNode(const FGraphPanelSelectionSet& selections);
	void InitEditor(const EToolkitMode::Type mode, const TSharedPtr< class IToolkitHost >& initToolkitHost, class UNewtonModel* const newtonModel);

	// Toolkit methods
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool OnRequestClose(EAssetEditorCloseReason InCloseReason) override;
	virtual void OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) override;
	virtual void OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) override;

	void OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& event);
	void OnFinishedChangingProperties(const FPropertyChangedEvent& propertyChangedEvent);
	void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	void HandleOnPreviewSceneSettingsCustomized(IDetailLayoutBuilder& DetailBuilder);
	void HandleSelectionChanged(const TArrayView<TSharedPtr<ISkeletonTreeItem>>& InSelectedItems, ESelectInfo::Type InSelectInfo);

	protected:
	UPROPERTY()
	UEdGraph* m_graphEditor;

	UPROPERTY()
	UNewtonModel* m_newtonModel;

	TSharedPtr<SGraphEditor> m_slateGraphUi;
	TObjectPtr<USkeletalMesh> m_skeletalMeshAsset;
	TSharedPtr<IDetailsView> m_selectedNodeDetailView;

	/** Skeleton tree */
	TSharedPtr<ISkeletonTree> SkeletonTree;

	/** Persona toolkit */
	TSharedPtr<IPersonaToolkit> PersonaToolkit;

	bool m_modelChange;
	static FName m_identifier;
};
