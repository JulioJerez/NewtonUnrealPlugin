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
#include "GraphEditAction.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

#include "NewtonModel.h"
#include "NewtonModelGraphNode.h"
#include "NewtonModelEditorMode.h"
#include "NewtonModelGraphSchema.h"
#include "NewtonModelGraphNodeRoot.h"

NewtonModelEditor::NewtonModelEditor()
	:FWorkflowCentricApplication()
{
	m_graphEditor = nullptr;
	m_newtonModel = nullptr;
	m_slateGraphUi = nullptr;
	m_selectedNodeDetailView = nullptr;
}

NewtonModelEditor::~NewtonModelEditor()
{
}

UEdGraph* NewtonModelEditor::GetGraphEditor() const
{
	return m_graphEditor;
}

UNewtonModel* NewtonModelEditor::GetNewtonMode() const
{
	return m_newtonModel;
}

void NewtonModelEditor::SetNewtonMode(UNewtonModel* const model)
{
	m_newtonModel = model;
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

void NewtonModelEditor::InitEditor(const EToolkitMode::Type mode, const TSharedPtr< class IToolkitHost >& initToolkitHost, class UNewtonModel* const newtonModel)
{
	TArray<UObject*> objectsToEdit;

	m_newtonModel = newtonModel;
	objectsToEdit.Push(m_newtonModel);

	m_newtonModel->SetPreSaveListeners([this]() {OnWorkingAssetPreSave(); });

	const FName graphName(TEXT("NewtonModelGraph"));
	m_graphEditor = FBlueprintEditorUtils::CreateNewGraph(m_newtonModel, graphName, UEdGraph::StaticClass(), UNewtonModelGraphSchema::StaticClass());
	
	InitAssetEditor(mode, initToolkitHost, TEXT("NewtonModelEditor"), FTabManager::FLayout::NullLayout, true, true, objectsToEdit);

	AddApplicationMode(NewtonModelEditorMode::m_editorModelName, MakeShareable(new NewtonModelEditorMode(SharedThis(this))));
	SetCurrentMode(NewtonModelEditorMode::m_editorModelName);

	NewtonModelGraphToEditorGraph();
}

FName NewtonModelEditor::GetToolkitFName() const
{
	const FName name(TEXT("NewtonModelEditor"));
	return name;
}

FText NewtonModelEditor::GetBaseToolkitName() const
{
	const FText name(FText::FromString("NewtonModelEditor"));
	return name;
}

FString NewtonModelEditor::GetWorldCentricTabPrefix() const
{
	return FString("nd");
}

FLinearColor NewtonModelEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(1.0f, 1.0f, 1.0f);
}

void NewtonModelEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& manager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(manager);
}

void NewtonModelEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& manager)
{
}

void NewtonModelEditor::OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit)
{
}

void NewtonModelEditor::OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit)
{
}

void NewtonModelEditor::OnClose()
{
	EditorGraphToNewtonModelGraph();
	
	m_newtonModel->SetPreSaveListeners(nullptr);
	FWorkflowCentricApplication::OnClose();
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
	//m_selectedNodeDetailView->SetObject(nullptr);
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

void NewtonModelEditor::OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& event)
{
	if (m_slateGraphUi)
	{
		UNewtonModelGraphNode* const selectedNode = GetSelectedNode(m_slateGraphUi->GetSelectedNodes());
		if (selectedNode)
		{
			selectedNode->SyncPinsWithResponses();
		}
		m_slateGraphUi->NotifyGraphChanged();
	}
}

//void NewtonModelEditor::OnGraphChanged(const struct FEdGraphEditAction& inAction)
void NewtonModelEditor::OnWorkingAssetPreSave()
{
	//if (inAction.Action & (GRAPHACTION_AddNode | GRAPHACTION_RemoveNode | GRAPHACTION_EditNode))
	//{
	EditorGraphToNewtonModelGraph();
	//}
}

void NewtonModelEditor::EditorGraphToNewtonModelGraph()
{
	if (!(m_graphEditor && m_newtonModel))
	{
		return;
	}

	m_newtonModel->Graph = NewObject<UNewtonModelGraph>(m_newtonModel);

	TMap<const UEdGraphPin*, UNewtonModelPin*> pinMap;
	UNewtonModelGraph* const newtonModelGraph = m_newtonModel->Graph;
	for (int i = 0; i < m_graphEditor->Nodes.Num(); ++i)
	{
		const UNewtonModelGraphNode* const graphNode = Cast<UNewtonModelGraphNode>(m_graphEditor->Nodes[i]);

		auto MakeNewtomModelNode = [this, &pinMap, graphNode, newtonModelGraph]()
		{
			if (Cast<UNewtonModelGraphNodeRoot>(graphNode))
			{
				//UEdGraphPin* const output = graphNode->CreateNodePin(EEdGraphPinDirection::EGPD_Output);
				//pinMap.Add(newtonModelNode->GetOuputPin(), output);
				//return graphNode;
			}

			//UNewtonModelGraphNode* const graphNode = NewObject<UNewtonModelGraphNode>(m_graphEditor);
			//UEdGraphPin* const input = graphNode->CreateNodePin(EEdGraphPinDirection::EGPD_Input);
			//UEdGraphPin* const output = graphNode->CreateNodePin(EEdGraphPinDirection::EGPD_Output);
			//pinMap.Add(newtonModelNode->GetInputPin(), input);
			//pinMap.Add(newtonModelNode->GetOuputPin(), output);
			UNewtonModelNode* const newtonModelNode = NewObject<UNewtonModelNode>(newtonModelGraph);
			return newtonModelNode;
		};

		//UNewtonModelNode* const newtonModelNode = NewObject<UNewtonModelNode>(newtonModelGraph);
		UNewtonModelNode* const newtonModelNode = MakeNewtomModelNode();

		check(0);
		//newtonModelNode->CreatePinNodes();
		//newtonModelNode->Posit = FVector2D(graphNode->NodePosX, graphNode->NodePosY);
		//newtonModelNode->Info = graphNode->GetNodeInfo();
		//const TArray<UEdGraphPin*>& graphPins = graphNode->GetAllPins();
		//check(graphPins.Num() <= 2);
		//check(graphPins.Num() >= 1);
		//for (int j = 0; j < graphPins.Num(); ++j)
		//{
		//	UEdGraphPin* const pin = graphPins[j];
		//	if (pin->Direction == EEdGraphPinDirection::EGPD_Input)
		//	{
		//		UNewtonModelPin* const modelPin = newtonModelNode->GetInputPin();
		//		//modelPin->Id = pin->PinId;
		//		//modelPin->Name = pin->GetFName();
		//		pinMap.Add(pin, modelPin);
		//	}
		//	else
		//	{
		//		check(pin->Direction == EEdGraphPinDirection::EGPD_Output);
		//		UNewtonModelPin* const modelPin = newtonModelNode->GetOuputPin();
		//		//modelPin->Id = pin->PinId;
		//		//modelPin->Name = pin->GetFName();
		//		pinMap.Add(pin, modelPin);
		//	}
		//}

		//const UNewtonModelGraphNodeRoot* const rootNode = Cast<UNewtonModelGraphNodeRoot>(graphNode);
		//if (rootNode)
		//{
		//	check(Cast<UNewtonModelNodeRoot>(newtonModelNode));
		//}
		newtonModelGraph->NodesArray.Push(newtonModelNode);
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

void NewtonModelEditor::NewtonModelGraphToEditorGraph()
{
	if (!(m_newtonModel && m_newtonModel->Graph))
	{
		return;
	}

	TMap<const UNewtonModelPin*, UEdGraphPin*> pinMap;
	UNewtonModelGraph* const newtonModelGraph = m_newtonModel->Graph;
	for (int i = 0; i < newtonModelGraph->NodesArray.Num(); ++i)
	{
		const UNewtonModelNode* const newtonModelNode = newtonModelGraph->NodesArray[i];
		auto MakeGraphNode = [this, &pinMap, newtonModelNode]()
		{
			if (Cast<UNewtonModelNodeRoot>(newtonModelNode))
			{
				UNewtonModelGraphNode* const graphNode = NewObject<UNewtonModelGraphNode>(m_graphEditor);
				UEdGraphPin* const output = graphNode->CreateNodePin(EEdGraphPinDirection::EGPD_Output);
				pinMap.Add(newtonModelNode->GetOuputPin(), output);
				return graphNode;
			}

			UNewtonModelGraphNode* const graphNode = NewObject<UNewtonModelGraphNode>(m_graphEditor);
			UEdGraphPin* const input = graphNode->CreateNodePin(EEdGraphPinDirection::EGPD_Input);
			UEdGraphPin* const output = graphNode->CreateNodePin(EEdGraphPinDirection::EGPD_Output);
			pinMap.Add(newtonModelNode->GetInputPin(), input);
			pinMap.Add(newtonModelNode->GetOuputPin(), output);
			return graphNode;
		};

		//UNewtonModelGraphNode* const graphNode = NewObject<UNewtonModelGraphNode>(m_graphEditor);
		//UEdGraphPin* const output = graphNode->CreateNodePin(EEdGraphPinDirection::EGPD_Output);
		//if (!Cast<UNewtonModelGraphNodeRoot>(graphNode))
		//{
		//	check(Cast<UNewtonModelNodeRoot>(newtonModelNode));
		//	UEdGraphPin* const input = graphNode->CreateNodePin(EEdGraphPinDirection::EGPD_Input);
		//}
		UNewtonModelGraphNode* const graphNode = MakeGraphNode();

		graphNode->CreateNewGuid();
		graphNode->NodePosX = newtonModelNode->Posit.X;
		graphNode->NodePosY = newtonModelNode->Posit.Y;

		if (newtonModelNode->Info)
		{
			graphNode->SetNodeInfo(DuplicateObject(newtonModelNode->Info, graphNode));
		}
		else
		{
			graphNode->SetNodeInfo(NewObject<UNewtonModelInfo>(graphNode));
		}

		//input->PinId = newtonModelNode->GetInputPin()->Id;
		//output->PinId = newtonModelNode->GetOuputPin()->Id;
		//pinMap.Add(newtonModelNode->GetInputPin(), input);
		//pinMap.Add(newtonModelNode->GetOuputPin(), output);
		m_graphEditor->AddNode(graphNode, true, true);
	}

	for (int i = 0; i < newtonModelGraph->NodesArray.Num(); ++i)
	{
		UNewtonModelNode* const newtonModelNode = newtonModelGraph->NodesArray[i];
		UEdGraphPin* const input = *pinMap.Find(newtonModelNode->GetInputPin());
		for (int j = 0; j < newtonModelNode->GetInputPin()->Connections.Num(); ++j)
		{
			UEdGraphPin* const target = *pinMap.Find(newtonModelNode->GetInputPin()->Connections[j]);
			input->LinkedTo.Add(target);
		}
	
		UEdGraphPin* const output = *pinMap.Find(newtonModelNode->GetOuputPin());
		for (int j = 0; j < newtonModelNode->GetOuputPin()->Connections.Num(); ++j)
		{
			UEdGraphPin* const target = *pinMap.Find(newtonModelNode->GetOuputPin()->Connections[j]);
			output->LinkedTo.Add(target);
		}
	}
}
