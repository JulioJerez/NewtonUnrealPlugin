// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/ArrayView.h"
#include "Misc/TextFilterExpressionEvaluator.h"

class INewtonModelPhysicsTreeItem;
//enum class ENewtonModelPhysicsTreeFilterResult : int32;
enum ENewtonModelPhysicsTreeFilterResult
{
	I_dontKnow,
};
class FTextFilterExpressionEvaluator;

/** Output struct for builders to use */
struct SKELETONEDITOR_API FFNewtonModelPhysicsTreeBuilderOutput
{
	FFNewtonModelPhysicsTreeBuilderOutput(TArray<TSharedPtr<class INewtonModelPhysicsTreeItem>>& InItems, TArray<TSharedPtr<class INewtonModelPhysicsTreeItem>>& InLinearItems)
		: Items(InItems)
		, LinearItems(InLinearItems)
	{}

	/** 
	 * Add an item to the output
	 * @param	InItem			The item to add
	 * @param	InParentName	The name of the item's parent
	 * @param	InParentTypes	The types of items to search. If this is empty all items will be searched.
	 * @param	bAddToHead		Whether to add the item to the start or end of the parent's children array
	 */
	void Add(const TSharedPtr<class INewtonModelPhysicsTreeItem>& InItem, const FName& InParentName, TArrayView<const FName> InParentTypes, bool bAddToHead = false);

	/** 
	 * Add an item to the output
	 * @param	InItem			The item to add
	 * @param	InParentName	The name of the item's parent
	 * @param	InParentTypes	The types of items to search. If this is empty all items will be searched.
	 * @param	bAddToHead		Whether to add the item to the start or end of the parent's children array
	 */
	FORCEINLINE void Add(const TSharedPtr<class INewtonModelPhysicsTreeItem>& InItem, const FName& InParentName, std::initializer_list<FName> InParentTypes, bool bAddToHead = false)
	{
		Add(InItem, InParentName, MakeArrayView(InParentTypes), bAddToHead);
	}

	/** 
	 * Add an item to the output
	 * @param	InItem			The item to add
	 * @param	InParentName	The name of the item's parent
	 * @param	InParentType	The type of items to search. If this is NAME_None all items will be searched.
	 * @param	bAddToHead		Whether to add the item to the start or end of the parent's children array
	 */
	void Add(const TSharedPtr<class INewtonModelPhysicsTreeItem>& InItem, const FName& InParentName, const FName& InParentType, bool bAddToHead = false);

	/** 
	 * Find the item with the specified name
	 * @param	InName	The item's name
	 * @param	InTypes	The types of items to search. If this is empty all items will be searched.
	 * @return the item found, or an invalid ptr if it was not found.
	 */
	TSharedPtr<class INewtonModelPhysicsTreeItem> Find(const FName& InName, TArrayView<const FName> InTypes) const;

	/** 
	 * Find the item with the specified name
	 * @param	InName	The item's name
	 * @param	InTypes	The types of items to search. If this is empty all items will be searched.
	 * @return the item found, or an invalid ptr if it was not found.
	 */
	FORCEINLINE TSharedPtr<class INewtonModelPhysicsTreeItem> Find(const FName& InName, std::initializer_list<FName> InTypes) const
	{
		return Find(InName, MakeArrayView(InTypes));
	}

	/** 
	 * Find the item with the specified name
	 * @param	InName	The item's name
	 * @param	InType	The type of items to search. If this is NAME_None all items will be searched.
	 * @return the item found, or an invalid ptr if it was not found.
	 */
	TSharedPtr<class INewtonModelPhysicsTreeItem> Find(const FName& InName, const FName& InType) const;

private:
	/** The items that are built by this builder */
	TArray<TSharedPtr<class INewtonModelPhysicsTreeItem>>& Items;

	/** A linearized list of all items in OutItems (for easier searching) */
	TArray<TSharedPtr<class INewtonModelPhysicsTreeItem>>& LinearItems;
};

/** Basic filter used when re-filtering the tree */
struct FNewtonModelPhysicsTreeFilterArgs
{
	FNewtonModelPhysicsTreeFilterArgs(TSharedPtr<FTextFilterExpressionEvaluator> InTextFilter)
		: TextFilter(InTextFilter)
		, bFlattenHierarchyOnFilter(true)
	{}

	/** The text filter we are using, if any */
	TSharedPtr<FTextFilterExpressionEvaluator> TextFilter;

	/** Whether to flatten the hierarchy so filtered items appear in a linear list */
	bool bFlattenHierarchyOnFilter;
};

/** Delegate used to filter an item. */
DECLARE_DELEGATE_RetVal_TwoParams(ENewtonModelPhysicsTreeFilterResult, FOnFilterNewtonModelPhysicsTreeItem, const FNewtonModelPhysicsTreeFilterArgs& /*InArgs*/, const TSharedPtr<class INewtonModelPhysicsTreeItem>& /*InItem*/);

/** Interface to implement to provide custom build logic to newton model physics trees */
class FNewtonModelPhysicsTreeBuilder : public TSharedFromThis<FNewtonModelPhysicsTreeBuilder>
{
	public:
	FNewtonModelPhysicsTreeBuilder();
	virtual ~FNewtonModelPhysicsTreeBuilder();

	/** Setup this builder with links to the tree and preview scene */
	virtual void Initialize(const TSharedRef<class INewtonModelPhysicsTree>& InNewtonModelPhysicsTree, const TSharedPtr<class IPersonaPreviewScene>& InPreviewScene, FOnFilterNewtonModelPhysicsTreeItem InOnFilterNewtonModelPhysicsTreeItem);

	/**
	 * Build an array of newton model physics tree items to display in the tree.
	 * @param	Output			The items that are built by this builder
	 */
	virtual void Build(FFNewtonModelPhysicsTreeBuilderOutput& Output);

	/** Apply filtering to the tree items */
	virtual void Filter(const FNewtonModelPhysicsTreeFilterArgs& InArgs, const TArray<TSharedPtr<class INewtonModelPhysicsTreeItem>>& InItems, TArray<TSharedPtr<class INewtonModelPhysicsTreeItem>>& OutFilteredItems);

	/** Allows the builder to contribute to filtering an item */
	virtual ENewtonModelPhysicsTreeFilterResult FilterItem(const FNewtonModelPhysicsTreeFilterArgs& InArgs, const TSharedPtr<class INewtonModelPhysicsTreeItem>& InItem);

	/** Get whether this builder is showing bones */
	virtual bool IsShowingBones() const;

	/** Get whether this builder is showing bones */
	virtual bool IsShowingSockets() const;

	/** Get whether this builder is showing bones */
	virtual bool IsShowingAttachedAssets() const;
};
