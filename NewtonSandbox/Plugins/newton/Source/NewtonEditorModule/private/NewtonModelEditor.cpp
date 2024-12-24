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

#include "NewtonModelEditor.h"
#include "PersonaModule.h"
#include "ISkeletonTree.h"
#include "GraphEditAction.h"
#include "IPersonaToolkit.h"
#include "EdGraphUtilities.h"
#include "IPersonaViewport.h"
#include "EditorModeManager.h"
#include "Engine/SkeletalMesh.h"
#include "IPersonaPreviewScene.h"
#include "ISkeletonEditorModule.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetRegistry/AssetData.h"
#include "UObject/ObjectSaveContext.h"
#include "AnimationEditorPreviewActor.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Animation/DebugSkelMeshComponent.h"

#include "NewtonEditorModule.h"
#include "NewtonModelEditorMode.h"
#include "NewtonModelPhysicsTree.h"
#include "NewtonModelEditorCommon.h"
#include "NewtonModelEditorBinding.h"
#include "NewtonModelSkeletalMeshEditorMode.h"


#define LOCTEXT_NAMESPACE "FNewtonModelEditor"

FEditorModeID FNewtonModelEditor::m_id(FName(TEXT("FNewtonModelEditor")));
FName FNewtonModelEditor::m_identifier(FName(TEXT("FNewtonModelEditor")));

FNewtonModelEditor::FNewtonModelEditor()
	:FPersonaAssetEditorToolkit()
	,IHasPersonaToolkit()
{
	m_modelSaved = false;
	m_newtonModel = nullptr;
	PersonaToolkit = nullptr;
	m_skeletonTree = nullptr;
	m_selectedBone = nullptr;
	m_selectedNodeDetailView = nullptr;
	m_skeletalMeshAssetCached = nullptr;
}

FNewtonModelEditor::~FNewtonModelEditor()
{
}

TSharedPtr<FNewtonModelEditorBinding> FNewtonModelEditor::GetBinding()
{
	if (!Binding)
	{
		Binding = MakeShared<FNewtonModelEditorBinding>(SharedThis(this));
	}

	return Binding;
}

TSharedRef<ISkeletonTree> FNewtonModelEditor::GetSkeletonTree() const
{ 
	return m_skeletonTree.ToSharedRef(); 
}

UDebugSkelMeshComponent* FNewtonModelEditor::GetSkelMeshComponent() const
{
	check(PreviewScene.IsValid());
	return PreviewScene->GetPreviewMeshComponent();
}

TSharedRef<FNewtonModelPhysicsTree> FNewtonModelEditor::GetNewtonModelPhysicsTree() const
{
	return m_skeletonPhysicsTree.ToSharedRef();
}

void FNewtonModelEditor::OnViewportCreated(const TSharedRef<IPersonaViewport>& viewport)
{
	Viewport = viewport;

	// we need the viewport client to start out focused, or else it won't get ticked until we click inside it.
	FEditorViewportClient& ViewportClient = viewport->GetViewportClient();
	ViewportClient.ReceivedFocus(ViewportClient.Viewport);
}

void FNewtonModelEditor::OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit)
{
}

void FNewtonModelEditor::OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit)
{
}

TSharedRef<IPersonaToolkit> FNewtonModelEditor::GetPersonaToolkit() const
{ 
	return PersonaToolkit.ToSharedRef(); 
}

TSharedRef<IPersonaPreviewScene> FNewtonModelEditor::GetPreviewScene() const
{
	return PreviewScene.ToSharedRef();
}

FName FNewtonModelEditor::GetToolkitFName() const
{
	return FName (TEXT("FNewtonModelEditor"));
}

FText FNewtonModelEditor::GetBaseToolkitName() const
{
	return FText::FromString("FNewtonModelEditor");
}

FString FNewtonModelEditor::GetWorldCentricTabPrefix() const
{
	return FString("FNewtonModelEditor");
}

FLinearColor FNewtonModelEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

UNewtonAsset* FNewtonModelEditor::GetNewtonModel() const
{
	return m_newtonModel;
}

void FNewtonModelEditor::SetSelectedNodeDetailView(TSharedPtr<IDetailsView> detailData)
{
	m_selectedNodeDetailView = detailData;
	m_selectedNodeDetailView->OnFinishedChangingProperties().AddRaw(this, &FNewtonModelEditor::OnNodeDetailViewPropertiesUpdated);
}

void FNewtonModelEditor::SetNewtonModel(TObjectPtr<UNewtonAsset> model)
{
	m_newtonModel = model;
}

void FNewtonModelEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& manager)
{
	FPersonaAssetEditorToolkit::RegisterTabSpawners(manager);
}

void FNewtonModelEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& manager)
{
}

void FNewtonModelEditor::OnPreviewSceneSettingsCustomized(IDetailLayoutBuilder& detailBuilder)
{
	check(0);
	//DetailBuilder.HideCategory("Mesh");
	//DetailBuilder.HideCategory("Physics");
	//// in mesh editor, we hide preview mesh section and additional mesh section
	//// sometimes additional meshes are interfering with preview mesh, it is not a great experience
	//DetailBuilder.HideCategory("Additional Meshes");
}

void FNewtonModelEditor::OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& event)
{
	check(m_skeletonPhysicsTree.IsValid());
	m_skeletonPhysicsTree->DetailViewPropertiesUpdated(event);
}

void FNewtonModelEditor::OnSkeletalMeshSelectionChanged(const TArrayView<TSharedPtr<ISkeletonTreeItem>>& selectedItem, ESelectInfo::Type InSelectInfo)
{
	check(m_skeletonPhysicsTree.IsValid());
	//if (selectedItem.Num() && selectedItem[0].IsValid())
	//{
	//	m_skeletonPhysicsTree->DetailViewBoneSelectedUpdated(selectedItem[0]);
	//}
	m_selectedBone = (selectedItem.Num() && selectedItem[0].IsValid()) ? selectedItem[0] : nullptr;
	m_skeletonPhysicsTree->DetailViewBoneSelectedUpdated(m_selectedBone);
}

// for some reason this doesn't fucking works. 
// there are so many delegates and flags in the persona system that changing anything, 
// the chances to get it work correctly, is near zero. (tried for weeks an only got bad outcomes)
// My solution is to just keep a separate data structure bind to the skeleton bone by bone.
void FNewtonModelEditor::OnPreviewSceneCreated(const TSharedRef<IPersonaPreviewScene>& personaPreviewScene)
{
	personaPreviewScene->SetDefaultAnimationMode(EPreviewSceneDefaultAnimationMode::ReferencePose);

	//AAnimationEditorPreviewActor* const actor = personaPreviewScene->GetWorld()->SpawnActor<AAnimationEditorPreviewActor>(AAnimationEditorPreviewActor::StaticClass(), FTransform::Identity);
	//personaPreviewScene->SetActor(actor);
	AActor* const actor = personaPreviewScene->GetActor();

	// Create the preview component
	//UDebugSkelMeshComponent* const skeletalMeshComponent = NewObject<UDebugSkelMeshComponent>(actor);
	UDebugSkelMeshComponent* const skeletalMeshComponent = personaPreviewScene->GetPreviewMeshComponent();
	USkeletalMesh* XXX0 = m_skeletalMeshAssetCached;
	USkeletalMesh* XXX1 = skeletalMeshComponent->GetSkeletalMeshAsset();
	check(XXX0 == XXX1);

	//skeletalMeshComponent->SetSkeletalMesh(m_skeletalMeshAssetCached);
	skeletalMeshComponent->SetDisablePostProcessBlueprint(true);

	//personaPreviewScene->AddComponent(skeletalMeshComponent, FTransform::Identity);
	//personaPreviewScene->SetPreviewMeshComponent(skeletalMeshComponent);
	//actor->SetRootComponent(skeletalMeshComponent);

	//// add a cube for testing.
	//FTransform transform(FVector(100.0, 0.0, 100.0f));
	//UBoxComponent* const child = NewObject<UBoxComponent>(actor);
	//child->SetBoxExtent(FVector(40.0, 60.0, 50.0f));
	////child->AttachToComponent(skeletalMeshComponent, FAttachmentTransformRules::KeepRelativeTransform);
	//personaPreviewScene->AddComponent(child, transform);
	//child->SetBoxExtent(FVector(40.0, 60.0, 50.0f));
}

void FNewtonModelEditor::OnFinishedChangingProperties(const FPropertyChangedEvent& propertyChangedEvent)
{
	FProperty* const property = propertyChangedEvent.Property;
	if (property->GetName() == TEXT("SkeletalMeshAsset"))
	{
		for (int i = 0; i < propertyChangedEvent.GetNumObjectsBeingEdited(); ++i)
		{
			const UNewtonAsset* const newtonAsset = Cast<UNewtonAsset>(propertyChangedEvent.GetObjectBeingEdited(i));
			if (newtonAsset)
			{
				if (m_skeletalMeshAssetCached != m_newtonModel->SkeletalMeshAsset)
				{
					m_skeletalMeshAssetCached = m_newtonModel->SkeletalMeshAsset;
					m_skeletonPhysicsTree->ResetSkeletalMesh();

					// persona plugin does not support recreation the viewport, nor it supports changing the skeletal mesh,
					// or skeleton, In fact almost nothing can be modified after Parsona tollkid is initialized. 
					// The only solution is to change the skeletal mesh, is to just close and re-open the editor.
					FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("CloseAndReopenEditor", "you need to close and reopen the physics editor for this change to take place. All the Physics Tree data will be loss"));
				}
				break;
			}
		}
	}
	else
	{
		check(0);
	}
}

void FNewtonModelEditor::OnMeshClick(HActor* hitProxy, const FViewportClick& click)
{
	check(0);
}

void FNewtonModelEditor::OnClose()
{
	FCoreUObjectDelegates::OnObjectPreSave.Remove(m_onCloseHandle);
	m_skeletonPhysicsTree->SaveModel();
	
	FPersonaAssetEditorToolkit::OnClose();
}
void FNewtonModelEditor::OnObjectSave(UObject* savedObject, FObjectPreSaveContext saveContext)
{
	if (!m_modelSaved)
	{
		// avoid recursion when saving model
		m_modelSaved = true;
		m_skeletonPhysicsTree->SaveModel();
		m_modelSaved = false;
	}
}

void FNewtonModelEditor::BindCommands()
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
}

void FNewtonModelEditor::InitEditor(const EToolkitMode::Type mode, const TSharedPtr< class IToolkitHost >& initToolkitHost, class UNewtonAsset* const newtonAsset)
{
	m_newtonModel = newtonAsset;
	m_skeletalMeshAssetCached = m_newtonModel->SkeletalMeshAsset;

	FPersonaModule& personaModule = FModuleManager::LoadModuleChecked<FPersonaModule>("Persona");
	ISkeletonEditorModule& skeletonEditorModule = FModuleManager::GetModuleChecked<ISkeletonEditorModule>(TEXT("SkeletonEditor"));

	personaModule.RecordAssetOpened(FAssetData(m_newtonModel));

	FPersonaToolkitArgs personaToolkitArgs;
	personaToolkitArgs.bPreviewMeshCanUseDifferentSkeleton = false;
	//personaToolkitArgs.OnPreviewSceneCreated = FOnPreviewSceneCreated::FDelegate::CreateSP(this, &FNewtonModelEditor::OnPreviewSceneCreated);
	personaToolkitArgs.OnPreviewSceneSettingsCustomized = FOnPreviewSceneSettingsCustomized::FDelegate::CreateSP(this, &FNewtonModelEditor::OnPreviewSceneSettingsCustomized);
	PersonaToolkit = personaModule.CreatePersonaToolkit(m_skeletalMeshAssetCached, personaToolkitArgs);
	PreviewScene = PersonaToolkit->GetPreviewScene();
	PreviewScene->SetDefaultAnimationMode(EPreviewSceneDefaultAnimationMode::ReferencePose);

	UDebugSkelMeshComponent* const skeletalMeshComponent = PreviewScene->GetPreviewMeshComponent();
	skeletalMeshComponent->SetDisablePostProcessBlueprint(true);
	check(m_skeletalMeshAssetCached == skeletalMeshComponent->GetSkeletalMeshAsset());


	// create a skeleton tree widgets for visualization.
	FSkeletonTreeArgs skeletonTreeArgs;
	skeletonTreeArgs.PreviewScene = PreviewScene;
	skeletonTreeArgs.ContextName = GetToolkitFName();
	skeletonTreeArgs.bAllowMeshOperations = false;
	skeletonTreeArgs.bAllowMeshOperations = false;
	skeletonTreeArgs.bAllowSkeletonOperations = false;
	skeletonTreeArgs.bShowDebugVisualizationOptions = true;
	skeletonTreeArgs.OnSelectionChanged = FOnSkeletonTreeSelectionChanged::CreateSP(this, &FNewtonModelEditor::OnSkeletalMeshSelectionChanged);
	m_skeletonTree = skeletonEditorModule.CreateSkeletonTree(PersonaToolkit->GetSkeleton(), skeletonTreeArgs);
	
	// create a acyclic physics tree widgets for physic model.
	m_skeletonPhysicsTree = SNew(FNewtonModelPhysicsTree, this);
	
	FAssetEditorToolkit::InitAssetEditor(mode, initToolkitHost, m_identifier, FTabManager::FLayout::NullLayout, true, true, m_newtonModel);

	//FPhysicsAssetEditorCommands::Register();
	BindCommands();

	AddApplicationMode(
		NewtonModelEditorMode::m_editorModelName,
		MakeShareable(new NewtonModelEditorMode(SharedThis(this), m_skeletonTree.ToSharedRef())));

	SetCurrentMode(NewtonModelEditorMode::m_editorModelName);
	
	// register callback for rebuild model when click save button
	m_onCloseHandle = FCoreUObjectDelegates::OnObjectPreSave.AddRaw(this, &FNewtonModelEditor::OnObjectSave);

	PreviewScene->RegisterOnMeshClick(FOnMeshClick::CreateSP(this, &FNewtonModelEditor::OnMeshClick));
	//PreviewScene->RegisterOnDeselectAll(FOnMeshClick::CreateSP(this, &FNewtonModelEditor::OnMeshClick));
	PreviewScene->SetAllowMeshHitProxies(true);

	// this shit enable the transform tool bar. 
	// why unreal does this crap is beyond reprehensible. 
	FEditorModeTools& editorModeManager = GetEditorModeManager();
	// make sure SkeletonSelection is active when toggling the mode, as they are compatible.
	if (!editorModeManager.IsModeActive(FPersonaEditModes::SkeletonSelection))
	{
		editorModeManager.ActivateMode(FPersonaEditModes::SkeletonSelection);
	}
	editorModeManager.ActivateMode(UNewtonModelSkeletalMeshEditorMode::m_id, true);
	UNewtonModelSkeletalMeshEditorMode* const editorMode = Cast<UNewtonModelSkeletalMeshEditorMode>(editorModeManager.GetActiveScriptableMode(UNewtonModelSkeletalMeshEditorMode::m_id));
	editorMode->SetEditor(this);


	//FNewtonEditorModule& module = FModuleManager::GetModuleChecked<FNewtonEditorModule>("NewtonEditorModule");
	//const TArray<FNewtonEditorModule::FOnSkeletalMeshEditorInitialized>& PostInitDelegates = SkeletalMeshEditorModule.GetPostEditorInitDelegates();
	//for (const auto& PostInitDelegate : PostInitDelegates)
	//{
	//	PostInitDelegate.ExecuteIfBound(SharedThis<ISkeletalMeshEditor>(this));
	//}

}

void FNewtonModelEditor::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	m_skeletonPhysicsTree->DebugDraw(view, viewport, pdi);
}

FMatrix FNewtonModelEditor::GetWidgetMatrix() const
{
	return m_skeletonPhysicsTree->GetWidgetMatrix();
}

void FNewtonModelEditor::ApplyDeltaTransform(const FVector& inDrag, const FRotator& inRot, const FVector& inScale) const
{
	m_skeletonPhysicsTree->ApplyDeltaTransform(inDrag, inRot, inScale);
}

bool FNewtonModelEditor::ShouldDrawWidget() const
{
	return m_skeletonPhysicsTree->ShouldDrawWidget();
}

bool FNewtonModelEditor::HaveSelection() const
{
	return m_skeletonPhysicsTree->HaveSelection();
}

#undef LOCTEXT_NAMESPACE