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
#include "NewtonRigidBody.h"
#include "NewtonLinkJoint.h"
#include "NewtonLinkRigidBodyRoot.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

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
		//USCS_Node* const bluePrintRootNode = CreateRootBodyComponent(model);
	}

	FBlueprintEditorUtils::MarkBlueprintAsModified(blueprint);
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(blueprint);
}

//USCS_Node* UNewtonModelBlueprintBuilder::CreateRootBodyComponent(UNewtonModel* const model)
//{
//	AActor* const actor = model->GetOwner();
//	UBlueprint* const blueprint = Cast<UBlueprint>(actor->GetClass()->ClassGeneratedBy);
//
//	TObjectPtr<USimpleConstructionScript> constructScript(blueprint->SimpleConstructionScript);
//	const UNewtonAsset* const asset = model->NewtonAsset;
//	TObjectPtr<USceneComponent> rootComponentProxy(NewObject<UNewtonRigidBody>(GetTransientPackage(), asset->RootBody->Name, RF_Transient));
//	// set the defualt properties here
//
//	// for some reason this moronic function have a dual behavior. 
//	// it only add root nodes, but if the number of node is 1, it will replace the root node. thsi is just bizzar. 
//	USCS_Node* const rootBlueprintNode = constructScript->CreateNode(rootComponentProxy->GetClass(), asset->RootBody->Name);
//	constructScript->AddNode(rootBlueprintNode);
//	UEditorEngine::CopyPropertiesForUnrelatedObjects(rootComponentProxy, rootBlueprintNode->ComponentTemplate);
//
//	AddSkeletalMesh(model, rootBlueprintNode);
//	return rootBlueprintNode;
//}

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
	
	TObjectPtr<USkeletalMeshComponent> skeletalMeshComponentProxy(NewObject<USkeletalMeshComponent>(GetTransientPackage(), TEXT("tempMeshComponet"), RF_Transient));
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

	ndFixSizeArray<USCS_Node*, 256> parentPool;
	ndFixSizeArray<UNewtonLink*, 256> stackPool;

	parentPool.PushBack(nullptr);
	stackPool.PushBack(asset->RootBody);
	while (stackPool.GetCount())
	{
		USCS_Node* parent = parentPool.Pop();
		UNewtonLink* const link = stackPool.Pop();

		TObjectPtr<USceneComponent> componentProxy(link->CreateBlueprintProxy());
		USCS_Node* const blueprintNode = constructScript->CreateNode(componentProxy->GetClass(), link->Name);
		UEditorEngine::CopyPropertiesForUnrelatedObjects(componentProxy, blueprintNode->ComponentTemplate);

		if (!parent)
		{
			constructScript->AddNode(blueprintNode);
			AddSkeletalMesh(model, blueprintNode);
		}
		else
		{
			parent->AddChildNode(blueprintNode);
		}
	
		for (int i = 0; i < link->Children.Num(); ++i)
		{
			parentPool.PushBack(blueprintNode);
			stackPool.PushBack(link->Children[i]);
		}
	}
}

