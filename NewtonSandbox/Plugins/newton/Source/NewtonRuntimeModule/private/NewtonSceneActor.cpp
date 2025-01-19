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

#include "NewtonSceneActor.h"
#include "EngineUtils.h"
#include "LandscapeStreamingProxy.h"
#include "LandscapeHeightfieldCollisionComponent.h"

#include "NewtonWorldActor.h"
#include "NewtonRuntimeModule.h"
#include "NewtonCollision.h"
#include "NewtonCollisionBox.h"
#include "NewtonSceneRigidBody.h"
#include "NewtonCollisionSphere.h"
#include "NewtonCollisionCapsule.h"
#include "NewtonCollisionLandscape.h"
#include "NewtonCollisionConvexHull.h"
#include "NewtonCollisionCollection.h"
#include "NewtonCollisionPolygonalMesh.h"
#include "ThirdParty/newtonLibrary/Public/thirdParty/ndConvexApproximation.h"

// Sets default values
ANewtonSceneActor::ANewtonSceneActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GenerateShapes = false;
	m_propertyChanged = false;
	
	RootBody = CreateDefaultSubobject<UNewtonSceneRigidBody>(TEXT("NewtonStaticBody"));
	RootComponent = RootBody;
	//RootBody->Mobility = EComponentMobility::Static;
}

void ANewtonSceneActor::PostLoad()
{
	Super::PostLoad();
	m_propertyChanged = true;
}

void ANewtonSceneActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	m_propertyChanged = true;
}

void ANewtonSceneActor::CreateCollisionFromUnrealPrimitive(TObjectPtr<UStaticMeshComponent> staticComponent)
{
	const UStaticMesh* const staticMesh = staticComponent->GetStaticMesh().Get();
	if (staticMesh)
	{
		UNewtonCollisionCollection* const collection = FindComponentByClass<UNewtonCollisionCollection>();
		check(collection);

		auto GenerateSimpleCollision = [this, staticComponent, collection]()
		{
			bool hasSimple = false;
			const UStaticMesh* const staticMesh = staticComponent->GetStaticMesh().Get();
			check(staticMesh);
			const UBodySetup* const bodySetup = staticMesh->GetBodySetup();
			check(bodySetup);
			
			//if (staticMesh->GetFName() == "Door_01_L")
			//{
			//	staticMesh->GetFName();
			//}

			const FKAggregateGeom& aggGeom = bodySetup->AggGeom;
			auto AddComponent = [this, staticComponent, collection](UNewtonCollision* const childComp)
			{
				//UE_LOG(LogTemp, Warning, TEXT("From NewtonSceneRigidBody, component render but does not shows in blueprint browser"));
				check(IsValid(childComp));
				FinishAddComponent(childComp, false, FTransform());
				AddInstanceComponent(childComp);
				//childComp->AttachToComponent(RootBody, FAttachmentTransformRules::KeepRelativeTransform);
				childComp->AttachToComponent(collection, FAttachmentTransformRules::KeepRelativeTransform);
				childComp->InitStaticMeshCompoment(staticComponent);
				childComp->MarkRenderDynamicDataDirty();
				childComp->NotifyMeshUpdated();
			};
	
			for (ndInt32 i = aggGeom.SphereElems.Num() - 1; i >= 0; --i)
			{
				UNewtonCollisionSphere* const child = Cast<UNewtonCollisionSphere>(AddComponentByClass(UNewtonCollisionSphere::StaticClass(), false, FTransform(), true));
				AddComponent(child);
				hasSimple = true;
			}
	
			for (ndInt32 i = aggGeom.SphylElems.Num() - 1; i >= 0; --i)
			{
				UNewtonCollisionCapsule* const child = Cast<UNewtonCollisionCapsule>(AddComponentByClass(UNewtonCollisionCapsule::StaticClass(), false, FTransform(), true));
				AddComponent(child);
				hasSimple = true;
			}
	
			for (ndInt32 i = aggGeom.BoxElems.Num() - 1; i >= 0; --i)
			{
				UNewtonCollisionBox* const child = Cast<UNewtonCollisionBox>(AddComponentByClass(UNewtonCollisionBox::StaticClass(), false, FTransform(), true));
				AddComponent(child);
				hasSimple = true;
			}
	
			for (ndInt32 i = aggGeom.ConvexElems.Num() - 1; i >= 0; --i)
			{
				UNewtonCollisionConvexHull* const child = Cast<UNewtonCollisionConvexHull>(AddComponentByClass(UNewtonCollisionConvexHull::StaticClass(), false, FTransform(), true));
				AddComponent(child);
				hasSimple = true;
			}
	
			for (ndInt32 i = aggGeom.TaperedCapsuleElems.Num() - 1; i >= 0; --i)
			{
				check(0);
			}
	
			for (ndInt32 i = aggGeom.LevelSetElems.Num() - 1; i >= 0; --i)
			{
				check(0);
			}
	
			for (ndInt32 i = aggGeom.SkinnedLevelSetElems.Num() - 1; i >= 0; --i)
			{
				check(0);
			}
	
			return hasSimple;
		};
	
		auto GenerateComplexCollision = [this, staticComponent, collection]()
		{
			UNewtonCollisionPolygonalMesh* const childComp = Cast<UNewtonCollisionPolygonalMesh>(AddComponentByClass(UNewtonCollisionPolygonalMesh::StaticClass(), false, FTransform(), true));
			FinishAddComponent(childComp, false, FTransform());
			AddInstanceComponent(childComp);
			//childComp->AttachToComponent(RootBody, FAttachmentTransformRules::KeepRelativeTransform);
			childComp->AttachToComponent(collection, FAttachmentTransformRules::KeepRelativeTransform);
			childComp->InitStaticMeshCompoment(staticComponent);
			childComp->MarkRenderDynamicDataDirty();
			childComp->NotifyMeshUpdated();
		};
	
		const UBodySetup* const bodySetup = staticMesh->GetBodySetup();
		ECollisionTraceFlag collisionFlag = bodySetup->GetCollisionTraceFlag();
		switch (collisionFlag)
		{
			case CTF_UseDefault:
			{
				if (!GenerateSimpleCollision())
				{
					GenerateComplexCollision();
				}
				break;
			}
			case CTF_UseSimpleAndComplex:
			{
				if (!GenerateSimpleCollision())
				{
					GenerateComplexCollision();
				}
				break;
			}
			case CTF_UseSimpleAsComplex:
			{
				//GenerateSimpleCollision();
				break;
			}
			case CTF_UseComplexAsSimple:
			{
				GenerateComplexCollision();
				break;
			}
		}
	}
}

void ANewtonSceneActor::GenerateLandScapeCollision(const ALandscapeProxy* const landscapeProxy)
{
	const TArray<TObjectPtr<ULandscapeHeightfieldCollisionComponent>>& landScapeTiles = landscapeProxy->CollisionComponents;
	for (ndInt32 i = 0; i < landScapeTiles.Num(); ++i)
	{
		const TObjectPtr<ULandscapeHeightfieldCollisionComponent>& tile = landScapeTiles[i];
		UNewtonCollisionLandscape* const collisionTile = Cast<UNewtonCollisionLandscape>(AddComponentByClass(UNewtonCollisionLandscape::StaticClass(), false, FTransform(), true));
		FinishAddComponent(collisionTile, false, FTransform());
		AddInstanceComponent(collisionTile);
		collisionTile->AttachToComponent(RootBody, FAttachmentTransformRules::KeepRelativeTransform);
	
		collisionTile->InitStaticMeshCompoment(tile);
		collisionTile->MarkRenderDynamicDataDirty();
		collisionTile->NotifyMeshUpdated();
	}
}

void ANewtonSceneActor::ApplyPropertyChanges()
{
	if (!m_propertyChanged)
	{
		return;
	}
	m_propertyChanged = false;
	
	if (!GenerateShapes)
	{
		return;
	}
	GenerateShapes = false;
	
	UNewtonSceneRigidBody* const staticSceneBody = FindComponentByClass<UNewtonSceneRigidBody>();
	if (!staticSceneBody)
	{
		return;
	}
	
	FFolder folder(GetFolder());
	if (folder.IsNone())
	{
		//const FName msg(TEXT("NewtonSceneActor must be child of a filter folder in the level editor outliner"));
		//const FString msg(TEXT("NewtonSceneActor must be child of a filter folder in the level editor outliner"));
		//const TCHAR* const msg = TEXT("NewtonSceneActor must be child of a filter folder in the level editor outliner");
		#define msg TEXT("NewtonSceneActor must be child of a filter folder in the level editor outliner")

		UE_LOG(LogTemp, Warning, msg);
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(msg));
		return;
	}

	//staticSceneBody->RemoveAllCollisions();
	if (GetOwner()->FindComponentByClass<UNewtonCollisionCollection>())
	{
		UNewtonCollisionCollection* const collection = GetOwner()->FindComponentByClass<UNewtonCollisionCollection>();
		const TArray<TObjectPtr<USceneComponent>>& children = collection->GetAttachChildren();
		for (ndInt32 j = children.Num() - 1; j >= 0; --j)
		{
			children[j]->DestroyComponent();
		}
		collection->DestroyComponent();
	}

	ndTree<ndInt32, const USceneComponent*> filter;
	ndFixSizeArray<TObjectPtr<USceneComponent>, ND_STACK_DEPTH> stack;
	stack.PushBack(GetRootComponent());
	while (stack.GetCount())
	{
		USceneComponent* const component = stack.Pop();
		filter.Insert(0, component);
		const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
		for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
		{
			stack.PushBack(childrenComp[i].Get());
		}
	}

	const UWorld* const world = GetWorld();
	const FString key(folder.GetPath().ToString());

	ndArray<AActor*> landScapesList;
	ndTree<TObjectPtr<USceneComponent>, const UStaticMeshComponent*> staticList;
	for (TActorIterator<AActor> actorItr(world); actorItr; ++actorItr)
	{
		AActor* const actor = *actorItr;

		if (actor != this)
		{
			const FString key1(actor->GetFolder().ToString());
			int index = key1.Find(key);
			if (index == 0)
			{
				if (Cast<ALandscapeStreamingProxy>(actor))
				{
					const ALandscapeStreamingProxy* const streamingProxy = Cast<ALandscapeStreamingProxy>(actor);
					const AActor* const parent = streamingProxy->GetSceneOutlinerParent();
					const FString streamingKey(parent->GetFolder().ToString());
					index = streamingKey.Find(key);
					if (index == 0)
					{
						//actorList.PushBack(actor);
						landScapesList.PushBack(actor);
					}
				}
				else
				{
					check(!stack.GetCount());
					stack.PushBack(actor->GetRootComponent());
					while (stack.GetCount())
					{
						TObjectPtr<USceneComponent> component(stack.Pop());
						if (!filter.Find(component.Get()))
						{
							const UStaticMeshComponent* const staticComponent = Cast<UStaticMeshComponent>(component.Get());
							const UStaticMesh* const staticMesh = staticComponent->GetStaticMesh().Get();
							if (staticMesh)
							{
								//if (staticMesh->GetFName() == TEXT("Door_01_L"))
								//{
								//	index = 1;
								//}
								staticList.Insert(component, staticComponent);
							}
						}

						const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
						for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
						{
							stack.PushBack(childrenComp[i].Get());
						}
					}
				}
			}
		}
	}
	
	UNewtonCollisionCollection* const collection = Cast<UNewtonCollisionCollection>(AddComponentByClass(UNewtonCollisionCollection::StaticClass(), false, FTransform(), true));
	check(IsValid(collection));
	FinishAddComponent(collection, false, FTransform());
	AddInstanceComponent(collection);
	collection->AttachToComponent(RootBody, FAttachmentTransformRules::KeepRelativeTransform);
	collection->MarkRenderDynamicDataDirty();

	for (ndInt32 i = ndInt32(landScapesList.GetCount()) - 1; i >= 0; --i)
	{
		AActor* const sceneActor = landScapesList[i];
		const ALandscapeProxy* const landscapeProxy = Cast<ALandscapeProxy>(sceneActor);
		GenerateLandScapeCollision(landscapeProxy);
	}

	ndTree<TObjectPtr<USceneComponent>, const UStaticMeshComponent*>::Iterator it(staticList);
	for (it.Begin(); it; it++)
	{
		TObjectPtr<USceneComponent> component(it.GetNode()->GetInfo());
		check(Cast<UStaticMeshComponent>(component));
		CreateCollisionFromUnrealPrimitive(Cast<UStaticMeshComponent>(component));
	}
}
