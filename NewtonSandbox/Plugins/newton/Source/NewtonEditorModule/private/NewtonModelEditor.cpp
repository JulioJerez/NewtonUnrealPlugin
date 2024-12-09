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
#include "Engine/SkeletalMesh.h"
#include "IPersonaPreviewScene.h"
#include "ISkeletonEditorModule.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetRegistry/AssetData.h"
#include "UObject/ObjectSaveContext.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"


#include "NewtonModelGraphNode.h"
#include "NewtonModelEditorMode.h"
#include "NewtonModelGraphSchema.h"
#include "NewtonModelEditorCommon.h"
#include "NewtonModelGraphNodeRoot.h"
#include "NewtonModelEditorBinding.h"
#include "ndTree/NewtonModelPhysicsTree.h"

#define LOCTEXT_NAMESPACE "FNewtonModelEditor"

FName FNewtonModelEditor::m_identifier(FName(TEXT("FNewtonModelEditor")));

FNewtonModelEditor::FNewtonModelEditor()
	:INewtonModelEditor()
{
	SkeletonTree = nullptr;
	m_newtonModel = nullptr;
	PersonaToolkit = nullptr;
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
	return SkeletonTree.ToSharedRef(); 
}

TSharedRef<FNewtonModelPhysicsTree> FNewtonModelEditor::GetNewtonModelPhysicsTree() const
{
	return m_skeletonPhysicsTree.ToSharedRef();
}

void FNewtonModelEditor::HandleViewportCreated(const TSharedRef<IPersonaViewport>& viewport)
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

UNewtonModel* FNewtonModelEditor::GetNewtonModel() const
{
	return m_newtonModel;
}

void FNewtonModelEditor::SetSelectedNodeDetailView(TSharedPtr<IDetailsView> detailData)
{
	m_selectedNodeDetailView = detailData;
	m_selectedNodeDetailView->OnFinishedChangingProperties().AddRaw(this, &FNewtonModelEditor::OnNodeDetailViewPropertiesUpdated);
}

void FNewtonModelEditor::SetNewtonModel(TObjectPtr<UNewtonModel> model)
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

void FNewtonModelEditor::HandleOnPreviewSceneSettingsCustomized(IDetailLayoutBuilder& detailBuilder)
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

void FNewtonModelEditor::HandleSkeletalMeshSelectionChanged(const TArrayView<TSharedPtr<ISkeletonTreeItem>>& selectedItem, ESelectInfo::Type InSelectInfo)
{
	check(m_skeletonPhysicsTree.IsValid());
	if (selectedItem.Num() && selectedItem[0].IsValid())
	{
		m_skeletonPhysicsTree->DetailViewBoneSelectedUpdated(selectedItem[0]);
	}
}

void FNewtonModelEditor::OnFinishedChangingProperties(const FPropertyChangedEvent& propertyChangedEvent)
{
	FProperty* const property = propertyChangedEvent.Property;
	if (property->GetName() == TEXT("SkeletalMeshAsset"))
	{
		for (int i = 0; i < propertyChangedEvent.GetNumObjectsBeingEdited(); ++i)
		{
			const UNewtonModel* const newtonModel = Cast<UNewtonModel>(propertyChangedEvent.GetObjectBeingEdited(i));
			if (newtonModel)
			{
				if (m_skeletalMeshAssetCached != m_newtonModel->SkeletalMeshAsset)
				{
					m_skeletalMeshAssetCached = m_newtonModel->SkeletalMeshAsset;
					m_skeletonPhysicsTree->ResetSkeletalMesh();

					FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("CloseAndReopenEditor", "you need to close and reopen the physics editor for this change to take place. All the Physics Tree data will be loss"));

					//SkeletonTree->SetSkeletalMesh(m_skeletalMeshAssetCached);
					//PersonaToolkit->SetMesh(m_skeletalMeshAssetCached);
					//
					//FPersonaToolkitArgs personaToolkitArgs;
					//personaToolkitArgs.OnPreviewSceneSettingsCustomized = FOnPreviewSceneSettingsCustomized::FDelegate::CreateSP(this, &FNewtonModelEditor::HandleOnPreviewSceneSettingsCustomized);
					//PersonaToolkit->Initialize(m_skeletalMeshAssetCached, personaToolkitArgs);
					//PersonaToolkit->Initialize(USkeleton * InSkeleton, personaToolkitArgs)

					//FPersonaToolkitArgs personaToolkitArgs;
					//personaToolkitArgs.OnPreviewSceneSettingsCustomized = FOnPreviewSceneSettingsCustomized::FDelegate::CreateSP(this, &FNewtonModelEditor::HandleOnPreviewSceneSettingsCustomized);
					//FPersonaModule& personaModule = FModuleManager::LoadModuleChecked<FPersonaModule>("Persona");
					//PersonaToolkit = personaModule.CreatePersonaToolkit(m_skeletalMeshAssetCached, personaToolkitArgs);
					//PersonaToolkit->GetPreviewScene()->SetDefaultAnimationMode(EPreviewSceneDefaultAnimationMode::ReferencePose);
					//personaModule.RecordAssetOpened(FAssetData(m_newtonModel));
					//PreviewScene = PersonaToolkit->GetPreviewScene();
					//{
					//	// create a skeleton tree widgets for visualization.
					//	FSkeletonTreeArgs skeletonTreeArgs;
					//	skeletonTreeArgs.OnSelectionChanged = FOnSkeletonTreeSelectionChanged::CreateSP(this, &FNewtonModelEditor::HandleSkeletalMeshSelectionChanged);
					//	skeletonTreeArgs.PreviewScene = PreviewScene;
					//	skeletonTreeArgs.ContextName = GetToolkitFName();
					//	ISkeletonEditorModule& skeletonEditorModule = FModuleManager::GetModuleChecked<ISkeletonEditorModule>(TEXT("SkeletonEditor"));
					//	SkeletonTree = skeletonEditorModule.CreateSkeletonTree(PersonaToolkit->GetSkeleton(), skeletonTreeArgs);
					//}
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

UNewtonModelGraphNode* FNewtonModelEditor::GetSelectedNode(const FGraphPanelSelectionSet& selectionSet)
{
	for (UObject* object : selectionSet)
	{
		UNewtonModelGraphNode* const node = Cast<UNewtonModelGraphNode>(object);
		if (node)
		{
			return node;
		}
	}
	return nullptr;
}

void FNewtonModelEditor::OnGraphSelectionChanged(const FGraphPanelSelectionSet& selectionSet)
{
	UNewtonModelGraphNode* const selectedNode = GetSelectedNode(selectionSet);
	if (selectedNode)
	{
		m_selectedNodeDetailView->SetObject(selectedNode->GetNodeInfo());
	}
	else
	{
		m_selectedNodeDetailView->SetObject(nullptr);
	}
}

void FNewtonModelEditor::OnClose()
{
	m_skeletonPhysicsTree->SaveModel();

	FCoreUObjectDelegates::OnObjectPreSave.Remove(m_onCloseHandle);
	FPersonaAssetEditorToolkit::OnClose();
}
void FNewtonModelEditor::OnObjectSave(UObject* savedObject, FObjectPreSaveContext saveContext)
{
	m_skeletonPhysicsTree->SaveModel();
	//BuildGraphEditorAsset();
}

void FNewtonModelEditor::BindCommands()
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
}

void FNewtonModelEditor::InitEditor(const EToolkitMode::Type mode, const TSharedPtr< class IToolkitHost >& initToolkitHost, class UNewtonModel* const newtonModel)
{
	m_newtonModel = newtonModel;
	m_skeletalMeshAssetCached = m_newtonModel->SkeletalMeshAsset;

	FPersonaToolkitArgs personaToolkitArgs;
	personaToolkitArgs.OnPreviewSceneSettingsCustomized = FOnPreviewSceneSettingsCustomized::FDelegate::CreateSP(this, &FNewtonModelEditor::HandleOnPreviewSceneSettingsCustomized);

	FPersonaModule& personaModule = FModuleManager::LoadModuleChecked<FPersonaModule>("Persona");
	PersonaToolkit = personaModule.CreatePersonaToolkit(m_skeletalMeshAssetCached, personaToolkitArgs);

	PersonaToolkit->GetPreviewScene()->SetDefaultAnimationMode(EPreviewSceneDefaultAnimationMode::ReferencePose);

	personaModule.RecordAssetOpened(FAssetData(m_newtonModel));

	PreviewScene = PersonaToolkit->GetPreviewScene();
	{
		// create a skeleton tree widgets for visualization.
		FSkeletonTreeArgs skeletonTreeArgs;
		skeletonTreeArgs.OnSelectionChanged = FOnSkeletonTreeSelectionChanged::CreateSP(this, &FNewtonModelEditor::HandleSkeletalMeshSelectionChanged);
		skeletonTreeArgs.PreviewScene = PreviewScene;
		skeletonTreeArgs.ContextName = GetToolkitFName();
		ISkeletonEditorModule& skeletonEditorModule = FModuleManager::GetModuleChecked<ISkeletonEditorModule>(TEXT("SkeletonEditor"));
		SkeletonTree = skeletonEditorModule.CreateSkeletonTree(PersonaToolkit->GetSkeleton(), skeletonTreeArgs);
	}

	{
		// create a acyclic physics tree widgets for physic model.
		m_skeletonPhysicsTree = SNew(FNewtonModelPhysicsTree, this);
	}

	FAssetEditorToolkit::InitAssetEditor(mode, initToolkitHost, m_identifier, FTabManager::FLayout::NullLayout, true, true, m_newtonModel);

	BindCommands();

	AddApplicationMode(
		NewtonModelEditorMode::m_editorModelName,
		MakeShareable(new NewtonModelEditorMode(SharedThis(this), SkeletonTree.ToSharedRef())));

	SetCurrentMode(NewtonModelEditorMode::m_editorModelName);
	
	// register callback for rebuild model when click save button
	m_onCloseHandle = FCoreUObjectDelegates::OnObjectPreSave.AddRaw(this, &FNewtonModelEditor::OnObjectSave);
}

#undef LOCTEXT_NAMESPACE