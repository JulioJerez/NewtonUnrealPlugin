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
#include "Engine/SkeletalMesh.h"
#include "IPersonaPreviewScene.h"
#include "ISkeletonEditorModule.h"
#include "UObject/ObjectSaveContext.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

#include "NewtonModel.h"
#include "NewtonModelGraphNode.h"
#include "NewtonModelEditorMode.h"
#include "NewtonModelGraphSchema.h"
#include "NewtonModelGraphNodeRoot.h"

FName NewtonModelEditor::m_identifier(FName(TEXT("NewtonModelEditor")));

NewtonModelEditor::NewtonModelEditor()
	:FWorkflowCentricApplication()
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

NewtonModelEditor::~NewtonModelEditor()
{
}

void NewtonModelEditor::OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit)
{
}

void NewtonModelEditor::OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit)
{
}

UEdGraph* NewtonModelEditor::GetGraphEditor() const
{
	return m_graphEditor;
}

FName NewtonModelEditor::GetToolkitFName() const
{
	return FName (TEXT("NewtonModelEditor"));
}

FText NewtonModelEditor::GetBaseToolkitName() const
{
	return FText::FromString("NewtonModelEditor");
}

FString NewtonModelEditor::GetWorldCentricTabPrefix() const
{
	return FString("NewtonModelEditor");
}

FLinearColor NewtonModelEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

UNewtonModel* NewtonModelEditor::GetNewtonModel() const
{
	return m_newtonModel;
}

void NewtonModelEditor::SetWorkingGraphUi(TSharedPtr<SGraphEditor> workingGraph)
{
	m_slateGraphUi = workingGraph;
}

void NewtonModelEditor::SetSelectedNodeDetailView(TSharedPtr<IDetailsView> detailData)
{
	m_selectedNodeDetailView = detailData;
	m_selectedNodeDetailView->OnFinishedChangingProperties().AddRaw(this, &NewtonModelEditor::OnNodeDetailViewPropertiesUpdated);
}

void NewtonModelEditor::OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& event)
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

void NewtonModelEditor::SetNewtonModel(TObjectPtr<UNewtonModel> model)
{
	m_newtonModel = model;
}

void NewtonModelEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& manager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(manager);
}

void NewtonModelEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& manager)
{
}

void NewtonModelEditor::HandleOnPreviewSceneSettingsCustomized(IDetailLayoutBuilder& DetailBuilder)
{
	check(0);
	//DetailBuilder.HideCategory("Mesh");
	//DetailBuilder.HideCategory("Physics");
	//// in mesh editor, we hide preview mesh section and additional mesh section
	//// sometimes additional meshes are interfering with preview mesh, it is not a great experience
	//DetailBuilder.HideCategory("Additional Meshes");
}

void NewtonModelEditor::HandleSelectionChanged(const TArrayView<TSharedPtr<ISkeletonTreeItem>>& InSelectedItems, ESelectInfo::Type InSelectInfo)
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

void NewtonModelEditor::CreateSkeletalMeshEditor()
{
	m_skeletalMeshAsset = m_newtonModel->SkeletalMeshAsset;

	FPersonaToolkitArgs personaToolkitArgs;
	personaToolkitArgs.OnPreviewSceneSettingsCustomized = FOnPreviewSceneSettingsCustomized::FDelegate::CreateSP(this, &NewtonModelEditor::HandleOnPreviewSceneSettingsCustomized);

	FPersonaModule& personaModule = FModuleManager::LoadModuleChecked<FPersonaModule>("Persona");
	PersonaToolkit = personaModule.CreatePersonaToolkit(m_newtonModel->SkeletalMeshAsset, personaToolkitArgs);
	PersonaToolkit->GetPreviewScene()->SetDefaultAnimationMode(EPreviewSceneDefaultAnimationMode::ReferencePose);

	TSharedPtr<IPersonaPreviewScene> previewScene(PersonaToolkit->GetPreviewScene());

	FSkeletonTreeArgs skeletonTreeArgs;
	skeletonTreeArgs.OnSelectionChanged = FOnSkeletonTreeSelectionChanged::CreateSP(this, &NewtonModelEditor::HandleSelectionChanged);
	skeletonTreeArgs.PreviewScene = previewScene;
	skeletonTreeArgs.ContextName = GetToolkitFName();

	ISkeletonEditorModule& skeletonEditorModule = FModuleManager::GetModuleChecked<ISkeletonEditorModule>(TEXT("SkeletonEditor"));
	SkeletonTree = skeletonEditorModule.CreateSkeletonTree(PersonaToolkit->GetSkeleton(), skeletonTreeArgs);
	AddApplicationMode(
		NewtonModelEditorMode::m_editorModelName,
		MakeShareable(new NewtonModelEditorMode(SharedThis(this), SkeletonTree.ToSharedRef())));
}

void NewtonModelEditor::OnFinishedChangingProperties(const FPropertyChangedEvent& propertyChangedEvent)
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

UNewtonModelGraphNode* NewtonModelEditor::GetSelectedNode(const FGraphPanelSelectionSet& selectionSet)
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

void NewtonModelEditor::OnGraphSelectionChanged(const FGraphPanelSelectionSet& selectionSet)
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

void NewtonModelEditor::BuildAsset()
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

bool NewtonModelEditor::OnRequestClose(EAssetEditorCloseReason closeReason)
{
	BuildAsset();
	return FWorkflowCentricApplication::OnRequestClose(closeReason);
}

void NewtonModelEditor::OnObjectSaved(UObject* savedObject, FObjectPreSaveContext saveContext)
{
	BuildAsset();
}

void NewtonModelEditor::OnGraphChanged(const FEdGraphEditAction& action)
{
	m_modelChange = true;
}

void NewtonModelEditor::InitEditor(const EToolkitMode::Type mode, const TSharedPtr< class IToolkitHost >& initToolkitHost, class UNewtonModel* const newtonModel)
{
	m_modelChange = false;
	m_newtonModel = newtonModel;
	InitAssetEditor(mode, initToolkitHost, m_identifier, FTabManager::FLayout::NullLayout, true, true, m_newtonModel);

	m_graphEditor = FBlueprintEditorUtils::CreateNewGraph(m_newtonModel, TEXT("NewtonModelGraph"), UEdGraph::StaticClass(), UNewtonModelGraphSchema::StaticClass());
	m_graphEditor->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateSP(this, &NewtonModelEditor::OnGraphChanged));

	CreateSkeletalMeshEditor();
	SetCurrentMode(NewtonModelEditorMode::m_editorModelName);
	
	// register callback for rebuild model when 
	FCoreUObjectDelegates::OnObjectPreSave.AddRaw(this, &NewtonModelEditor::OnObjectSaved);

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
