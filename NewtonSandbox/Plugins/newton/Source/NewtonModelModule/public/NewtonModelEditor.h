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

#include "NewtonModelEditorCommon.h"

struct HActor;
class UNewtonAsset;
class ISkeletonTree;
class IPersonaToolkit;
class IPersonaViewport;
class ISkeletonTreeItem;
class IPinnedCommandList;
class IPersonaPreviewScene;
class IDetailLayoutBuilder;
class FObjectPreSaveContext;
class FNewtonModelPhysicsTree;
class FNewtonModelEditorBinding;

class NEWTONMODELMODULE_API FNewtonModelEditor : public FPersonaAssetEditorToolkit, public IHasPersonaToolkit
{
	public:
	FNewtonModelEditor();
	~FNewtonModelEditor();

	// editor methods
	void BindCommands();
	UNewtonAsset* GetNewtonModel() const;
	TSharedRef<ISkeletonTree> GetSkeletonTree() const;
	void SetNewtonModel(TObjectPtr<UNewtonAsset> model);
	UDebugSkelMeshComponent* GetSkelMeshComponent() const;
	TSharedRef<IPersonaToolkit> GetPersonaToolkit() const;
	TSharedRef<IPersonaPreviewScene> GetPreviewScene() const;
	
	TSharedRef<FNewtonModelPhysicsTree> GetNewtonModelPhysicsTree() const;

	bool HaveSelection() const;
	bool ShouldDrawWidget() const;
	FMatrix GetWidgetMatrix() const;
	void DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const;
	void ApplyDeltaTransform(const FVector& inDrag, const FRotator& inRot, const FVector& inScale) const;
	
	virtual TSharedPtr<FNewtonModelEditorBinding> GetBinding();
	void SetSelectedNodeDetailView(TSharedPtr<IDetailsView> detailData);
	void InitEditor(const EToolkitMode::Type mode, const TSharedPtr< class IToolkitHost >& initToolkitHost, class UNewtonAsset* const newtonAsset);

	// Delegates
	void OnMeshClick(HActor* hitProxy, const FViewportClick& click);
	void OnViewportCreated(const TSharedRef<IPersonaViewport>& viewport);
	void OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& event);
	void OnPreviewSceneSettingsCustomized(IDetailLayoutBuilder& detailBuilder);
	void OnFinishedChangingProperties(const FPropertyChangedEvent& propertyChangedEvent);
	
	void OnPreviewSceneCreated(const TSharedRef<IPersonaPreviewScene>& personaPreviewScene);
	
	void OnSkeletalMeshSelectionChanged(const TArrayView<TSharedPtr<ISkeletonTreeItem>>& InSelectedItems, ESelectInfo::Type InSelectInfo);

	// Toolkit methods
	virtual void OnClose() override;
	virtual void SaveAsset_Execute() override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual void OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) override;
	virtual void OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) override;
	
	void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	protected:
	UNewtonAsset* m_newtonModel;
	
	TSharedPtr<IDetailsView> m_selectedNodeDetailView;
	TObjectPtr<USkeletalMesh> m_skeletalMeshAssetCached;

	// Skeleton tree
	TSharedPtr<ISkeletonTree> m_skeletonTree;
	TSharedPtr<ISkeletonTreeItem> m_selectedBone;

	// Skeleton physics tree
	TSharedPtr<FNewtonModelPhysicsTree> m_skeletonPhysicsTree;

	// Persona toolkit stuff 
	TSharedPtr<IPersonaViewport> m_viewport;
	TSharedPtr<IPersonaToolkit> m_personaToolkit;
	TSharedPtr<IPersonaPreviewScene> m_previewScene;

	// Binding to send/receive skeletal mesh modifications
	TSharedPtr<FNewtonModelEditorBinding> m_binding;

	// Pinned commands panel
	//TSharedPtr<IPinnedCommandList> m_pinnedCommands;

	// save model when closing editor
	FDelegateHandle m_onCloseHandle;

	static FName m_identifier;
	static FEditorModeID m_id;
	friend class FNewtonModelPhysicsTree;
};

