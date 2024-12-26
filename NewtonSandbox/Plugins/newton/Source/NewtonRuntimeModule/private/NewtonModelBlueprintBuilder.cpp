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

#include "NewtonModelBlueprintBuilder.h"
#include "Engine/SCS_Node.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Components/SkeletalMeshComponent.h"

#include "NewtonLink.h"
#include "NewtonModel.h"
#include "NewtonAsset.h"
#include "NewtonJoint.h"
#include "NewtonCollision.h"
#include "NewtonRigidBody.h"
#include "NewtonLinkJoint.h"
#include "NewtonLinkCollision.h"
#include "NewtonLinkRigidBodyRoot.h"
#include "NewtonModelSkeletalMesh.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

#define MAX_MODEL_NODES			2048
#define MAX_MODEL_STACK_DEPTH	256

void UNewtonModelBlueprintBuilder::BuildModel(UNewtonModel* const model)
{
	AActor* const actor = model->GetOwner();
	UBlueprint* const blueprint = Cast<UBlueprint>(actor->GetClass()->ClassGeneratedBy);
	if (!blueprint)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("a NewtonModel must be assigned to a blueprint")));
		return;
	}

	bool haveAsset = UpdateModel(model);
	if (haveAsset)
	{
		// remove all nodes from the blueprint node array. 
		TObjectPtr<USimpleConstructionScript> constructScript(blueprint->SimpleConstructionScript);
		const TArray<USCS_Node*>& nodes = constructScript->GetAllNodes();
		for (int i = nodes.Num() - 1; i >= 0; --i)
		{
			const USCS_Node* const blueprintNode = nodes[i];
			if (blueprintNode->ComponentClass->IsChildOf(USceneComponent::StaticClass()))
			{
				check(blueprintNode->ComponentClass);
				constructScript->RemoveNode(nodes[i]);
			}
		}
		// it seems unreal does not allow for the RootNode of a blueprint to be null, 
		// it can only be replaced, therefore, after the the array is emptied, thare souel be only one node. 
		// them we just replace it with out new scene body node root node.
		check((nodes[0] == constructScript->GetDefaultSceneRootNode()) || (nodes[1] == constructScript->GetDefaultSceneRootNode()));

		BuildHierarchy(model);
	}

	//FBlueprintEditorUtils::MarkBlueprintAsModified(blueprint);
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(blueprint);
}

bool UNewtonModelBlueprintBuilder::UpdateModel(UNewtonModel* const model)
{
	AActor* const actor = model->GetOwner();
	UBlueprint* const blueprint = Cast<UBlueprint>(actor->GetClass()->ClassGeneratedBy);

	model->RegenerateBluePrint = false;
	TObjectPtr<USimpleConstructionScript> constructScript(blueprint->SimpleConstructionScript);
	const TArray<USCS_Node*>& nodes = constructScript->GetAllNodes();
	for (int i = nodes.Num() - 1; i >= 0; --i)
	{
		USCS_Node* const blueprintNode = nodes[i];
		if (blueprintNode->ComponentClass->IsChildOf(UNewtonModel::StaticClass()))
		{
			USCS_Node* const assetNode = blueprintNode;
			UEditorEngine::CopyPropertiesForUnrelatedObjects(model, assetNode->ComponentTemplate);
			break;
		}
	}

	if (!model->NewtonAsset || !model->NewtonAsset->SkeletalMeshAsset.Get())
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("a NewtonModel must contain a valid NewtonAsset")));
		return false;
	}

	return true;
}

void UNewtonModelBlueprintBuilder::AddSkeletalMesh(UNewtonModel* const model, USCS_Node* const parentNode)
{
	AActor* const actor = model->GetOwner();
	UBlueprint* const blueprint = Cast<UBlueprint>(actor->GetClass()->ClassGeneratedBy);

	TObjectPtr<USimpleConstructionScript> constructScript(blueprint->SimpleConstructionScript);
	const TArray<USCS_Node*>& nodes = constructScript->GetAllNodes();

	UNewtonAsset* const asset = model->NewtonAsset;
	
	TObjectPtr<UNewtonModelSkeletalMesh> skeletalMeshComponentProxy(NewObject<UNewtonModelSkeletalMesh>(GetTransientPackage(), TEXT("tempMeshComponet"), RF_Transient));
	skeletalMeshComponentProxy->SetSkeletalMeshAsset(asset->SkeletalMeshAsset);

	USCS_Node* const childBlueprintNode = constructScript->CreateNode(skeletalMeshComponentProxy->GetClass(), TEXT("MeshComponet"));
	parentNode->AddChildNode(childBlueprintNode);
	UEditorEngine::CopyPropertiesForUnrelatedObjects(skeletalMeshComponentProxy, childBlueprintNode->ComponentTemplate);
}

void UNewtonModelBlueprintBuilder::BuildHierarchy(UNewtonModel* const model)
{
	AActor* const actor = model->GetOwner();
	const UNewtonAsset* const asset = model->NewtonAsset;
	UBlueprint* const blueprint = Cast<UBlueprint>(actor->GetClass()->ClassGeneratedBy);

	TObjectPtr<USimpleConstructionScript> constructScript(blueprint->SimpleConstructionScript);

	ndFixSizeArray<USCS_Node*, MAX_MODEL_STACK_DEPTH> parentPool;
	ndFixSizeArray<UNewtonLink*, MAX_MODEL_STACK_DEPTH> stackPool;
	ndFixSizeArray<TObjectPtr<USceneComponent>, MAX_MODEL_NODES> parentProxy;

	parentPool.PushBack(nullptr);
	parentProxy.PushBack(nullptr);
	stackPool.PushBack(asset->RootBody);

	ndFixSizeArray<UNewtonLink*, MAX_MODEL_NODES> links;
	ndFixSizeArray<USCS_Node*, MAX_MODEL_NODES> bluePrintNodes;
	ndFixSizeArray<TObjectPtr<USceneComponent>, MAX_MODEL_NODES> proxies;
	while (stackPool.GetCount())
	{
		USCS_Node* parent = parentPool.Pop();
		UNewtonLink* const link = stackPool.Pop();
		TObjectPtr<USceneComponent> parentComponentProxy = parentProxy.Pop();

		TObjectPtr<USceneComponent> componentProxy(link->CreateBlueprintProxy());
		USCS_Node* const blueprintNode = constructScript->CreateNode(componentProxy->GetClass(), link->Name);

		links.PushBack(link);
		proxies.PushBack(componentProxy);
		bluePrintNodes.PushBack(blueprintNode);

		if (!parent)
		{
			constructScript->AddNode(blueprintNode);
			AddSkeletalMesh(model, blueprintNode);
		}
		else
		{
			check(parentComponentProxy != nullptr);
			parent->AddChildNode(blueprintNode);
			componentProxy->AttachToComponent(parentComponentProxy, FAttachmentTransformRules::KeepRelativeTransform);
		}

		const FTransform globalTransform(link->CalculateGlobalTransform());
		componentProxy->SetRelativeRotation_Direct(link->Transform.Rotator());
		componentProxy->SetRelativeScale3D_Direct(link->Transform.GetScale3D());
		componentProxy->SetRelativeLocation_Direct(link->Transform.GetTranslation());
		componentProxy->SetComponentToWorld(globalTransform);

		USceneComponent* const nodeComponent = Cast<USceneComponent>(blueprintNode->ComponentTemplate);
		if (nodeComponent)
		{
			nodeComponent->SetRelativeRotation_Direct(link->Transform.Rotator());
			nodeComponent->SetRelativeScale3D_Direct(link->Transform.GetScale3D());
			nodeComponent->SetRelativeLocation_Direct(link->Transform.GetTranslation());
			nodeComponent->SetComponentToWorld(globalTransform);
		}
	
		for (int i = 0; i < link->Children.Num(); ++i)
		{
			parentPool.PushBack(blueprintNode);
			parentProxy.PushBack(componentProxy);
			stackPool.PushBack(link->Children[i]);
		}
	}

	// initalize shape
	for (int i = 0; i < links.GetCount(); ++i)
	{
		UNewtonLink* const link = links[i];
		USCS_Node* const blueprintNode = bluePrintNodes[i];
		TObjectPtr<USceneComponent> componentProxy(proxies[i]);

		if (Cast<UNewtonCollision>(componentProxy))
		{
			link->InitBlueprintProxy(componentProxy);
			UEditorEngine::CopyPropertiesForUnrelatedObjects(componentProxy, blueprintNode->ComponentTemplate);
		}
	}

	// initalize rigid bodies
	for (int i = 0; i < links.GetCount(); ++i)
	{
		UNewtonLink* const link = links[i];
		USCS_Node* const blueprintNode = bluePrintNodes[i];
		TObjectPtr<USceneComponent> componentProxy(proxies[i]);
		TObjectPtr<UNewtonRigidBody> bodyProxy(Cast<UNewtonRigidBody>(componentProxy));
		if (bodyProxy)
		{
			link->InitBlueprintProxy(componentProxy);
			bodyProxy->ShowDebug = model->ShowDebug;
			bodyProxy->ShowCenterOfMass = model->ShowDebug;
			bodyProxy->Inertia.ShowPrincipalAxis = model->ShowDebug;
			UEditorEngine::CopyPropertiesForUnrelatedObjects(componentProxy, blueprintNode->ComponentTemplate);
		}
	}

	// initalize joints
	for (int i = 0; i < links.GetCount(); ++i)
	{
		UNewtonLink* const link = links[i];
		USCS_Node* const blueprintNode = bluePrintNodes[i];
		TObjectPtr<USceneComponent> componentProxy(proxies[i]);
		TObjectPtr<UNewtonJoint> jointProxy(Cast<UNewtonJoint>(componentProxy));

		if (jointProxy)
		{
			link->InitBlueprintProxy(componentProxy);
			jointProxy->ShowDebug = model->ShowDebug;
			UEditorEngine::CopyPropertiesForUnrelatedObjects(componentProxy, blueprintNode->ComponentTemplate);
		}
	}
}

void UNewtonModelBlueprintBuilder::HideDebug(UNewtonModel* const model)
{
	//AActor* const actor = model->GetOwner();
	//UBlueprint* const blueprint = Cast<UBlueprint>(actor->GetClass()->ClassGeneratedBy);
	//if (!blueprint)
	//{
	//	return;
	//}
	//
	//TObjectPtr<USimpleConstructionScript> constructScript(blueprint->SimpleConstructionScript);
	//const TArray<USCS_Node*>& nodes = constructScript->GetAllNodes();
	//
	//model->HideDebug = false;
	//for (int i = nodes.Num() - 1; i >= 0; --i)
	//{
	//	USCS_Node* const blueprintNode = nodes[i];
	//	UNewtonRigidBody* const bodyNode = Cast<UNewtonRigidBody>(blueprintNode->ComponentTemplate);
	//	if (bodyNode)
	//	{
	//		bodyNode->ShowDebug = false;
	//		bodyNode->ShowCenterOfMass = false;
	//		bodyNode->Inertia.ShowPrincipalAxis = false;
	//	}
	//	UNewtonJoint* const jointNode = Cast<UNewtonJoint>(blueprintNode->ComponentTemplate);
	//	if (jointNode)
	//	{
	//		jointNode->ShowDebug = false;
	//	}
	//	UNewtonModel* const modelNode = Cast<UNewtonModel>(blueprintNode->ComponentTemplate);
	//	if (modelNode)
	//	{
	//		modelNode->HideDebug = false;
	//	}
	//}
	//FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(blueprint);
}

void UNewtonModelBlueprintBuilder::ApplyBlueprintProperties(UNewtonModel* const model)
{
	if (model->RegenerateBluePrint)
	{
		BuildModel(model);
	}
	//if (model->HideDebug)
	//{
	//	HideDebug(model);
	//}
}