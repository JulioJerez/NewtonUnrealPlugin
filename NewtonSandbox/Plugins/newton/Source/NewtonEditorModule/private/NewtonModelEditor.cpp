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
#include "IPersonaViewport.h"
#include "Engine/SkeletalMesh.h"
#include "IPersonaPreviewScene.h"
#include "ISkeletonEditorModule.h"
#include "AssetRegistry/AssetData.h"
#include "UObject/ObjectSaveContext.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

#include "NewtonModel.h"
#include "NewtonModelGraphNode.h"
#include "NewtonModelEditorMode.h"
#include "NewtonModelGraphSchema.h"
#include "NewtonModelGraphNodeRoot.h"
#include "NewtonModelEditorBinding.h"

FName FNewtonModelEditor::m_identifier(FName(TEXT("FNewtonModelEditor")));

FNewtonModelEditor::FNewtonModelEditor()
	:INewtonModelEditor()
{
	m_modelChange = false;
	SkeletonTree = nullptr;
	m_graphEditor = nullptr;
	m_newtonModel = nullptr;
	PersonaToolkit = nullptr;
	m_slateGraphUi = nullptr;
	m_skeletalMeshAsset = nullptr;
	m_selectedNodeDetailView = nullptr;
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

TSharedRef<class ISkeletonTree> FNewtonModelEditor::GetSkeletonTree() const
{ 
	return SkeletonTree.ToSharedRef(); 
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

TSharedRef<class IPersonaToolkit> FNewtonModelEditor::GetPersonaToolkit() const
{ 
	return PersonaToolkit.ToSharedRef(); 
}

UEdGraph* FNewtonModelEditor::GetGraphEditor() const
{
	return m_graphEditor;
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

void FNewtonModelEditor::SetWorkingGraphUi(TSharedPtr<SGraphEditor> workingGraph)
{
	m_slateGraphUi = workingGraph;
}

void FNewtonModelEditor::SetSelectedNodeDetailView(TSharedPtr<IDetailsView> detailData)
{
	m_selectedNodeDetailView = detailData;
	m_selectedNodeDetailView->OnFinishedChangingProperties().AddRaw(this, &FNewtonModelEditor::OnNodeDetailViewPropertiesUpdated);
}

void FNewtonModelEditor::OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& event)
{
	if (m_slateGraphUi)
	{
		m_modelChange = true;
		UNewtonModelGraphNode* const selectedNode = GetSelectedNode(m_slateGraphUi->GetSelectedNodes());
		if (selectedNode)
		{
			selectedNode->SyncPinsWithResponses();
		}
		m_slateGraphUi->NotifyGraphChanged();
	}
}

void FNewtonModelEditor::SetNewtonModel(TObjectPtr<UNewtonModel> model)
{
	m_newtonModel = model;
}

void FNewtonModelEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& manager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(manager);
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

void FNewtonModelEditor::HandleSelectionChanged(const TArrayView<TSharedPtr<ISkeletonTreeItem>>& InSelectedItems, ESelectInfo::Type InSelectInfo)
{
	check(0);
	//if (DetailsView.IsValid())
	//{
	//	TArray<UObject*> Objects;
	//	Algo::TransformIf(InSelectedItems, Objects, [](const TSharedPtr<ISkeletonTreeItem>& InItem) { return InItem->GetObject() != nullptr; }, [](const TSharedPtr<ISkeletonTreeItem>& InItem) { return InItem->GetObject(); });
	//	DetailsView->SetObjects(Objects);
	//
	//	if (Binding.IsValid())
	//	{
	//		TArray<FName> BoneSelection;
	//		Algo::TransformIf(InSelectedItems, BoneSelection,
	//			[](const TSharedPtr<ISkeletonTreeItem>& InItem)
	//			{
	//				const bool bIsBoneType = InItem->IsOfTypeByName("FSkeletonTreeBoneItem");
	//				const bool bIsNotNone = InItem->GetRowItemName() != NAME_None;
	//				return bIsBoneType && bIsNotNone;
	//			},
	//			[](const TSharedPtr<ISkeletonTreeItem>& InItem) { return InItem->GetRowItemName(); });
	//		Binding->GetNotifier().Notify(BoneSelection, ESkeletalMeshNotifyType::BonesSelected);
	//	}
	//}
}

void FNewtonModelEditor::OnFinishedChangingProperties(const FPropertyChangedEvent& propertyChangedEvent)
{
	for (int i = 0; i < propertyChangedEvent.GetNumObjectsBeingEdited(); ++i)
	{
		const UNewtonModel* const newtonModel = Cast<UNewtonModel>(propertyChangedEvent.GetObjectBeingEdited(i));
		if (newtonModel && (newtonModel->SkeletalMeshAsset != m_skeletalMeshAsset.Get()))
		{
			check(m_newtonModel == newtonModel);
			CreateSkeletalMeshEditor();
			break;
		}
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

void FNewtonModelEditor::BuildAsset()
{
	if (m_modelChange && m_graphEditor && m_graphEditor->Nodes.Num() && m_newtonModel)
	{
		m_newtonModel->Graph = NewObject<UNewtonModelGraph>(m_newtonModel);

		TMap<const UEdGraphPin*, UNewtonModelPin*> pinMap;
		UNewtonModelGraph* const newtonModelGraph = m_newtonModel->Graph;
		for (int i = 0; i < m_graphEditor->Nodes.Num(); ++i)
		{
			const UNewtonModelGraphNode* const graphNode = Cast<UNewtonModelGraphNode>(m_graphEditor->Nodes[i]);
			auto MakeNewtomModelNode = [this, &pinMap, graphNode, newtonModelGraph]()
			{
				const TArray<UEdGraphPin*>& graphPins = graphNode->GetAllPins();
				if (Cast<UNewtonModelGraphNodeRoot>(graphNode))
				{
					UNewtonModelNode* const newtonModelNode = NewObject<UNewtonModelNodeRoot>(newtonModelGraph);
					newtonModelNode->Initialize(graphNode->GetNodeInfo());
					pinMap.Add(graphPins[0], newtonModelNode->GetOuputPin());
					return newtonModelNode;
				}

				UNewtonModelNode* const newtonModelNode = NewObject<UNewtonModelNode>(newtonModelGraph);
				newtonModelNode->Initialize(graphNode->GetNodeInfo());
				pinMap.Add(graphPins[0], newtonModelNode->GetInputPin());
				pinMap.Add(graphPins[1], newtonModelNode->GetOuputPin());
				return newtonModelNode;
			};
			UNewtonModelNode* const node = MakeNewtomModelNode();
			node->Posit = FVector2D(graphNode->NodePosX, graphNode->NodePosY);
			newtonModelGraph->NodesArray.Push(node);
		}

		for (int i = 0; i < m_graphEditor->Nodes.Num(); ++i)
		{
			const UNewtonModelGraphNode* const graphNode = Cast<UNewtonModelGraphNode>(m_graphEditor->Nodes[i]);

			const TArray<UEdGraphPin*>& graphPins = graphNode->GetAllPins();
			for (int j = 0; j < graphPins.Num(); ++j)
			{
				UEdGraphPin* const sourcePin = graphPins[j];
				UNewtonModelPin* const modelSourcePin = *pinMap.Find(sourcePin);
				for (int k = 0; k < sourcePin->LinkedTo.Num(); ++k)
				{
					UEdGraphPin* const targetPin = sourcePin->LinkedTo[k];
					UNewtonModelPin* const modelTargetPin = *pinMap.Find(targetPin);
					modelSourcePin->Connections.Push(modelTargetPin);
				}
			}
		}
	}
	m_modelChange = false;
}

bool FNewtonModelEditor::OnRequestClose(EAssetEditorCloseReason closeReason)
{
	BuildAsset();
	return FWorkflowCentricApplication::OnRequestClose(closeReason);
}

void FNewtonModelEditor::OnObjectSaved(UObject* savedObject, FObjectPreSaveContext saveContext)
{
	BuildAsset();
}

void FNewtonModelEditor::OnGraphChanged(const FEdGraphEditAction& action)
{
	m_modelChange = true;
}


void FNewtonModelEditor::CreateSkeletalMeshEditor()
{
	check(0);
	//m_skeletalMeshAsset = m_newtonModel->SkeletalMeshAsset;
	//
	//FPersonaToolkitArgs personaToolkitArgs;
	//personaToolkitArgs.OnPreviewSceneSettingsCustomized = FOnPreviewSceneSettingsCustomized::FDelegate::CreateSP(this, &FNewtonModelEditor::HandleOnPreviewSceneSettingsCustomized);
	//
	//FPersonaModule& personaModule = FModuleManager::LoadModuleChecked<FPersonaModule>("Persona");
	//PersonaToolkit = personaModule.CreatePersonaToolkit(m_newtonModel->SkeletalMeshAsset, personaToolkitArgs);
	//
	//PersonaToolkit->GetPreviewScene()->SetDefaultAnimationMode(EPreviewSceneDefaultAnimationMode::ReferencePose);
	//
	//personaModule.RecordAssetOpened(FAssetData(m_newtonModel));
	//
	//TSharedPtr<IPersonaPreviewScene> previewScene(PersonaToolkit->GetPreviewScene());
	//
	//FSkeletonTreeArgs skeletonTreeArgs;
	//skeletonTreeArgs.OnSelectionChanged = FOnSkeletonTreeSelectionChanged::CreateSP(this, &FNewtonModelEditor::HandleSelectionChanged);
	//skeletonTreeArgs.PreviewScene = previewScene;
	//skeletonTreeArgs.ContextName = GetToolkitFName();
	//
	//ISkeletonEditorModule& skeletonEditorModule = FModuleManager::GetModuleChecked<ISkeletonEditorModule>(TEXT("SkeletonEditor"));
	//SkeletonTree = skeletonEditorModule.CreateSkeletonTree(PersonaToolkit->GetSkeleton(), skeletonTreeArgs);
	//
	//AddApplicationMode(
	//	NewtonModelEditorMode::m_editorModelName,
	//	MakeShareable(new NewtonModelEditorMode(SharedThis(this), SkeletonTree.ToSharedRef())));
}

void FNewtonModelEditor::BindCommands()
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
}

void FNewtonModelEditor::InitEditor(const EToolkitMode::Type mode, const TSharedPtr< class IToolkitHost >& initToolkitHost, class UNewtonModel* const newtonModel)
{
	m_modelChange = false;
	m_newtonModel = newtonModel;
	m_skeletalMeshAsset = m_newtonModel->SkeletalMeshAsset;

	FPersonaToolkitArgs personaToolkitArgs;
	personaToolkitArgs.OnPreviewSceneSettingsCustomized = FOnPreviewSceneSettingsCustomized::FDelegate::CreateSP(this, &FNewtonModelEditor::HandleOnPreviewSceneSettingsCustomized);

	FPersonaModule& personaModule = FModuleManager::LoadModuleChecked<FPersonaModule>("Persona");
	PersonaToolkit = personaModule.CreatePersonaToolkit(m_newtonModel->SkeletalMeshAsset, personaToolkitArgs);

	PersonaToolkit->GetPreviewScene()->SetDefaultAnimationMode(EPreviewSceneDefaultAnimationMode::ReferencePose);

	personaModule.RecordAssetOpened(FAssetData(m_newtonModel));

	TSharedPtr<IPersonaPreviewScene> previewScene(PersonaToolkit->GetPreviewScene());

	FSkeletonTreeArgs skeletonTreeArgs;
	skeletonTreeArgs.OnSelectionChanged = FOnSkeletonTreeSelectionChanged::CreateSP(this, &FNewtonModelEditor::HandleSelectionChanged);
	skeletonTreeArgs.PreviewScene = previewScene;
	skeletonTreeArgs.ContextName = GetToolkitFName();

	ISkeletonEditorModule& skeletonEditorModule = FModuleManager::GetModuleChecked<ISkeletonEditorModule>(TEXT("SkeletonEditor"));
	SkeletonTree = skeletonEditorModule.CreateSkeletonTree(PersonaToolkit->GetSkeleton(), skeletonTreeArgs);

	FAssetEditorToolkit::InitAssetEditor(mode, initToolkitHost, m_identifier, FTabManager::FLayout::NullLayout, true, true, m_newtonModel);
	//FAssetEditorToolkit::InitAssetEditor(mode, initToolkitHost, m_identifier, FTabManager::FLayout::NullLayout, true, true, m_skeletalMeshAsset);

	m_graphEditor = FBlueprintEditorUtils::CreateNewGraph(m_newtonModel, TEXT("NewtonModelGraph"), UEdGraph::StaticClass(), UNewtonModelGraphSchema::StaticClass());
	m_graphEditor->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateSP(this, &FNewtonModelEditor::OnGraphChanged));

	BindCommands();

	AddApplicationMode(
		NewtonModelEditorMode::m_editorModelName,
		MakeShareable(new NewtonModelEditorMode(SharedThis(this), SkeletonTree.ToSharedRef())));

	SetCurrentMode(NewtonModelEditorMode::m_editorModelName);
	
	// register callback for rebuild model when 
	FCoreUObjectDelegates::OnObjectPreSave.AddRaw(this, &FNewtonModelEditor::OnObjectSaved);


	//FName ParentName;
	//const FName MenuName = GetToolMenuToolbarName(ParentName);
	//UToolMenu* ToolBar = UToolMenus::Get()->RegisterMenu(MenuName, ParentName, EMultiBoxType::ToolBar);
	
	//check(0);
	//ExtendMenu();
	//ExtendToolbar();
	//RegenerateMenusAndToolbars();
	//
	//// Set up mesh click selection
	//PreviewScene->RegisterOnMeshClick(FOnMeshClick::CreateSP(this, &FSkeletalMeshEditor::HandleMeshClick));
	//PreviewScene->SetAllowMeshHitProxies(true);
	//
	//// run attached post-init delegates
	//ISkeletalMeshEditorModule& SkeletalMeshEditorModule = FModuleManager::GetModuleChecked<ISkeletalMeshEditorModule>("SkeletalMeshEditor");
	//const TArray<ISkeletalMeshEditorModule::FOnSkeletalMeshEditorInitialized>& PostInitDelegates = SkeletalMeshEditorModule.GetPostEditorInitDelegates();
	//for (const auto& PostInitDelegate : PostInitDelegates)
	//{
	//	PostInitDelegate.ExecuteIfBound(SharedThis<ISkeletalMeshEditor>(this));
	//}
	
	if (m_newtonModel->Graph)
	{
		//create the editor graph from the the NewtonModel
		TMap<const UNewtonModelPin*, UEdGraphPin*> pinMap;
		UNewtonModelGraph* const graph = m_newtonModel->Graph;
		for (int i = 0; i < graph->NodesArray.Num(); ++i)
		{
			const UNewtonModelNode* const newtonModelNode = graph->NodesArray[i];
			auto MakeGraphNode = [this, &pinMap, newtonModelNode]()
			{
				if (Cast<UNewtonModelNodeRoot>(newtonModelNode))
				{
					UNewtonModelGraphNode* const graphNode = NewObject<UNewtonModelGraphNodeRoot>(m_graphEditor);
					graphNode->Initialize(newtonModelNode->Info);
					pinMap.Add(newtonModelNode->GetOuputPin(), graphNode->GetOutputPin());
					return graphNode;
				}

				UNewtonModelGraphNode* const graphNode = NewObject<UNewtonModelGraphNode>(m_graphEditor);
				graphNode->Initialize(newtonModelNode->Info);
				pinMap.Add(newtonModelNode->GetInputPin(), graphNode->GetInputPin());
				pinMap.Add(newtonModelNode->GetOuputPin(), graphNode->GetOutputPin());
				return graphNode;
			};
			UNewtonModelGraphNode* const graphNode = MakeGraphNode();

			graphNode->CreateNewGuid();
			graphNode->NodePosX = newtonModelNode->Posit.X;
			graphNode->NodePosY = newtonModelNode->Posit.Y;
			check(newtonModelNode->Info);
			m_graphEditor->AddNode(graphNode, true, true);
		}

		for (int i = 0; i < graph->NodesArray.Num(); ++i)
		{
			UNewtonModelNode* const newtonModelNode = graph->NodesArray[i];

			if (newtonModelNode->GetInputPin())
			{
				UEdGraphPin* const input = *pinMap.Find(newtonModelNode->GetInputPin());
				for (int j = 0; j < newtonModelNode->GetInputPin()->Connections.Num(); ++j)
				{
					UEdGraphPin* const target = *pinMap.Find(newtonModelNode->GetInputPin()->Connections[j]);
					input->LinkedTo.Add(target);
				}
			}

			if (newtonModelNode->GetOuputPin())
			{
				UEdGraphPin* const output = *pinMap.Find(newtonModelNode->GetOuputPin());
				for (int j = 0; j < newtonModelNode->GetOuputPin()->Connections.Num(); ++j)
				{
					UEdGraphPin* const target = *pinMap.Find(newtonModelNode->GetOuputPin()->Connections[j]);
					output->LinkedTo.Add(target);
				}
			}
		}
	}
	m_modelChange = false;
}
