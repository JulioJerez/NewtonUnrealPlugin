// Copyright Epic Games, Inc. All Rights Reserved.

#include "ndTree/NewtonModelPhysicsTreeEditableSkeleton.h"

#include "ndTree/NewtonModelPhysicsTree.h"

FNewtonModelPhysicsTreeEditableSkeleton::FNewtonModelPhysicsTreeEditableSkeleton(UNewtonModelAcyclicGraph* const skeleton)
//	:FGCObject()
	:TSharedFromThis<FNewtonModelPhysicsTreeEditableSkeleton>()
	,Skeleton(skeleton)
{
}
	
FNewtonModelPhysicsTreeEditableSkeleton::~FNewtonModelPhysicsTreeEditableSkeleton()
{
}

TSharedRef<FNewtonModelPhysicsTree> FNewtonModelPhysicsTreeEditableSkeleton::CreateSkeletonTree(const FNewtonModelPhysicsTreeArgs& args)
{
	// first compact widget tracking array
	//SkeletonTrees.RemoveAll([](TWeakPtr<SSkeletonTree> InSkeletonTree) { return !InSkeletonTree.IsValid(); });

	// build new tree
	TSharedRef<FNewtonModelPhysicsTree> tree (SNew(FNewtonModelPhysicsTree, AsShared(), args));

	UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
	//OnTreeRefresh.AddSP(&SkeletonTree.Get(), &SSkeletonTree::HandleTreeRefresh);
	//SkeletonTrees.Add(tree);
	return tree;
}

