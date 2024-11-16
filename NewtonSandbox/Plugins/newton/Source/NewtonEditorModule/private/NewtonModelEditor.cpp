// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelEditor.h"
#include "GraphEditAction.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

#include "NewtonModel.h"
#include "NewtonModelGraphNode.h"
#include "NewtonModelEditorMode.h"
#include "NewtonModelGraphSchema.h"

NewtonModelEditor::NewtonModelEditor()
	:FWorkflowCentricApplication()
{
	m_graphEditor = nullptr;
	m_newtonModel = nullptr;
}

NewtonModelEditor::~NewtonModelEditor()
{
}

void NewtonModelEditor::InitEditor(const EToolkitMode::Type mode, const TSharedPtr< class IToolkitHost >& initToolkitHost, class UNewtonModel* const newtonModel)
{
	TArray<UObject*> objectsToEdit;

	m_newtonModel = newtonModel;
	objectsToEdit.Push(m_newtonModel);

	const FName graphName(TEXT("NewtonModelGraph"));
	//m_graphEditor = FBlueprintEditorUtils::CreateNewGraph(m_newtonModel, graphName, UEdGraph::StaticClass(), UEdGraphSchema::StaticClass());
	m_graphEditor = FBlueprintEditorUtils::CreateNewGraph(m_newtonModel, graphName, UEdGraph::StaticClass(), UNewtonModelGraphSchema::StaticClass());
	
	InitAssetEditor(mode, initToolkitHost, TEXT("NewtonModelEditor"), FTabManager::FLayout::NullLayout, true, true, objectsToEdit);

	AddApplicationMode(NewtonModelEditorMode::m_editorModelName, MakeShareable(new NewtonModelEditorMode(SharedThis(this))));
	SetCurrentMode(NewtonModelEditorMode::m_editorModelName);

	//UpdateGraphFromNewtonModel();
	NewtonModelGraphToEditorGraph();

	m_graphListener = m_graphEditor->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateSP(this, &NewtonModelEditor::OnGraphChanged));
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
	
	m_graphEditor->RemoveOnGraphChangedHandler(m_graphListener);
	FWorkflowCentricApplication::OnClose();
}

void NewtonModelEditor::OnGraphChanged(const struct FEdGraphEditAction& inAction)
{
	if (inAction.Action & (GRAPHACTION_AddNode | GRAPHACTION_RemoveNode | GRAPHACTION_EditNode))
	{
		EditorGraphToNewtonModelGraph();
	}
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
		UNewtonModelNode* const newtonModelNode = NewObject<UNewtonModelNode>(newtonModelGraph);

		newtonModelNode->CreatePinNodes();
		newtonModelNode->Posit = FVector2D(graphNode->NodePosX, graphNode->NodePosY);
		
		const TArray<UEdGraphPin*>& graphPins = graphNode->GetAllPins();
		check(graphPins.Num() <= 2);
		check(graphPins.Num() >= 1);
		for (int j = 0; j < graphPins.Num(); ++j)
		{
			UEdGraphPin* const pin = graphPins[j];
			if (pin->Direction == EEdGraphPinDirection::EGPD_Input)
			{
				UNewtonModelPin* const modelPin = newtonModelNode->GetInputPin();
				modelPin->Id = pin->PinId;
				//modelPin->Name = pin->GetFName();
				pinMap.Add(pin, modelPin);
			}
			else
			{
				check(pin->Direction == EEdGraphPinDirection::EGPD_Output);
				UNewtonModelPin* const modelPin = newtonModelNode->GetOuputPin();
				modelPin->Id = pin->PinId;
				//modelPin->Name = pin->GetFName();
				pinMap.Add(pin, modelPin);
			}
		}
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
		UNewtonModelGraphNode* const graphNode = NewObject<UNewtonModelGraphNode>(m_graphEditor);
		UEdGraphPin* const input = graphNode->CreateNodePin(EEdGraphPinDirection::EGPD_Input);
		UEdGraphPin* const output = graphNode->CreateNodePin(EEdGraphPinDirection::EGPD_Output);

		graphNode->CreateNewGuid();
		graphNode->NodePosX = newtonModelNode->Posit.X;
		graphNode->NodePosY = newtonModelNode->Posit.Y;
		input->PinId = newtonModelNode->GetInputPin()->Id;
		output->PinId = newtonModelNode->GetOuputPin()->Id;
		pinMap.Add(newtonModelNode->GetInputPin(), input);
		pinMap.Add(newtonModelNode->GetOuputPin(), output);
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
