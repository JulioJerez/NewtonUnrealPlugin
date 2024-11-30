// Copyright Epic Games, Inc. All Rights Reserved.

#include "ndTree/NewtonModelPhysicsTreeBuilder.h"



FNewtonModelPhysicsTreeBuilder::FNewtonModelPhysicsTreeBuilder() 
	:TSharedFromThis<FNewtonModelPhysicsTreeBuilder>()
{
}
	
FNewtonModelPhysicsTreeBuilder::~FNewtonModelPhysicsTreeBuilder() 
{
}

	/** Setup this builder with links to the tree and preview scene */
void FNewtonModelPhysicsTreeBuilder::Initialize(const TSharedRef<class INewtonModelPhysicsTree>& InNewtonModelPhysicsTree, const TSharedPtr<class IPersonaPreviewScene>& InPreviewScene, FOnFilterNewtonModelPhysicsTreeItem InOnFilterNewtonModelPhysicsTreeItem)
{
	check(0);
}

void FNewtonModelPhysicsTreeBuilder::Build(FFNewtonModelPhysicsTreeBuilderOutput& Output)
{
	check(0);
}

/** Apply filtering to the tree items */
void FNewtonModelPhysicsTreeBuilder::Filter(const FNewtonModelPhysicsTreeFilterArgs& InArgs, const TArray<TSharedPtr<class INewtonModelPhysicsTreeItem>>& InItems, TArray<TSharedPtr<class INewtonModelPhysicsTreeItem>>& OutFilteredItems)
{
	check(0);
}

ENewtonModelPhysicsTreeFilterResult FNewtonModelPhysicsTreeBuilder::FilterItem(const FNewtonModelPhysicsTreeFilterArgs& InArgs, const TSharedPtr<class INewtonModelPhysicsTreeItem>& InItem)
{
	check(0);
	return I_dontKnow;
}

bool FNewtonModelPhysicsTreeBuilder::IsShowingBones() const
{
	check(0);
	return true;
}

bool FNewtonModelPhysicsTreeBuilder::IsShowingSockets() const
{
	check(0);
	return true;
}

bool FNewtonModelPhysicsTreeBuilder::IsShowingAttachedAssets() const
{
	check(0);
	return true;
}
