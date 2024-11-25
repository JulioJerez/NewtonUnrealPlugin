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
#include "IHasPersonaToolkit.h"
#include "PersonaAssetEditorToolkit.h"

class UNewtonModel;
class UNewtonModelGraphNode;

class ISkeletonTree;
class IPersonaToolkit;
class IPersonaViewport;
class ISkeletonTreeItem;
class IDetailLayoutBuilder;
class FObjectPreSaveContext;
class FNewtonModelEditorBinding;

class INewtonModelEditor : public FPersonaAssetEditorToolkit, public IHasPersonaToolkit
{
	public:
	virtual TSharedPtr<FNewtonModelEditorBinding> GetBinding() = 0;
};


/**
 * 
 */
class NEWTONEDITORMODULE_API FNewtonModelEditor : public INewtonModelEditor
{
	public:
	FNewtonModelEditor();
	~FNewtonModelEditor();

	// editor methods
	void BuildAsset();
	void BindCommands();
	void CreateSkeletalMeshEditor();
	UEdGraph* GetGraphEditor() const;
	UNewtonModel* GetNewtonModel() const;
	void SetNewtonModel(TObjectPtr<UNewtonModel> model);
	TSharedRef<class ISkeletonTree> GetSkeletonTree() const;
	TSharedRef<class IPersonaToolkit> GetPersonaToolkit() const;
	void SetWorkingGraphUi(TSharedPtr<SGraphEditor> workingGraph);
	void SetSelectedNodeDetailView(TSharedPtr<IDetailsView> detailData);
	virtual TSharedPtr<FNewtonModelEditorBinding> GetBinding() override;
	UNewtonModelGraphNode* GetSelectedNode(const FGraphPanelSelectionSet& selections);
	void InitEditor(const EToolkitMode::Type mode, const TSharedPtr< class IToolkitHost >& initToolkitHost, class UNewtonModel* const newtonModel);

	// Delegates
	void OnGraphChanged(const FEdGraphEditAction& action);
	void OnGraphSelectionChanged(const FGraphPanelSelectionSet& selection);
	void HandleViewportCreated(const TSharedRef<IPersonaViewport>& viewport);
	void OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& event);
	void OnObjectSaved(UObject* savedObject, FObjectPreSaveContext saveContext);
	void HandleOnPreviewSceneSettingsCustomized(IDetailLayoutBuilder& betailBuilder);
	void OnFinishedChangingProperties(const FPropertyChangedEvent& propertyChangedEvent);
	void HandleSelectionChanged(const TArrayView<TSharedPtr<ISkeletonTreeItem>>& InSelectedItems, ESelectInfo::Type InSelectInfo);


	// Toolkit methods
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool OnRequestClose(EAssetEditorCloseReason InCloseReason) override;
	virtual void OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) override;
	virtual void OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) override;
	
	void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	

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

	/** Viewport */
	TSharedPtr<class IPersonaViewport> Viewport;

	/** Persona toolkit */
	TSharedPtr<IPersonaToolkit> PersonaToolkit;

	// Binding to send/receive skeletal mesh modifications
	TSharedPtr<FNewtonModelEditorBinding> Binding;

	bool m_modelChange;
	static FName m_identifier;
};

