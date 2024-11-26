// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/ArrayView.h"
#include "Misc/TextFilterExpressionEvaluator.h"

class ISkeletonTreeItem;
enum class ESkeletonTreeFilterResult : int32;
class FTextFilterExpressionEvaluator;

/** Output struct for builders to use */
struct FNewtonModelSkeletonTreeBuilderOutput
{
	FNewtonModelSkeletonTreeBuilderOutput(TArray<TSharedPtr<class ISkeletonTreeItem>>& InItems, TArray<TSharedPtr<class ISkeletonTreeItem>>& InLinearItems);

	/** 
	 * Add an item to the output
	 * @param	InItem			The item to add
	 * @param	InParentName	The name of the item's parent
	 * @param	InParentTypes	The types of items to search. If this is empty all items will be searched.
	 * @param	bAddToHead		Whether to add the item to the start or end of the parent's children array
	 */
	void Add(const TSharedPtr<class ISkeletonTreeItem>& InItem, const FName& InParentName, TArrayView<const FName> InParentTypes, bool bAddToHead = false);

	/** 
	 * Add an item to the output
	 * @param	InItem			The item to add
	 * @param	InParentName	The name of the item's parent
	 * @param	InParentTypes	The types of items to search. If this is empty all items will be searched.
	 * @param	bAddToHead		Whether to add the item to the start or end of the parent's children array
	 */
	FORCEINLINE void Add(const TSharedPtr<class ISkeletonTreeItem>& InItem, const FName& InParentName, std::initializer_list<FName> InParentTypes, bool bAddToHead = false)
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
	void Add(const TSharedPtr<class ISkeletonTreeItem>& InItem, const FName& InParentName, const FName& InParentType, bool bAddToHead = false);

	/** 
	 * Find the item with the specified name
	 * @param	InName	The item's name
	 * @param	InTypes	The types of items to search. If this is empty all items will be searched.
	 * @return the item found, or an invalid ptr if it was not found.
	 */
	TSharedPtr<class ISkeletonTreeItem> Find(const FName& InName, TArrayView<const FName> InTypes) const;

	/** 
	 * Find the item with the specified name
	 * @param	InName	The item's name
	 * @param	InTypes	The types of items to search. If this is empty all items will be searched.
	 * @return the item found, or an invalid ptr if it was not found.
	 */
	FORCEINLINE TSharedPtr<class ISkeletonTreeItem> Find(const FName& InName, std::initializer_list<FName> InTypes) const
	{
		return Find(InName, MakeArrayView(InTypes));
	}

	/** 
	 * Find the item with the specified name
	 * @param	InName	The item's name
	 * @param	InType	The type of items to search. If this is NAME_None all items will be searched.
	 * @return the item found, or an invalid ptr if it was not found.
	 */
	TSharedPtr<class ISkeletonTreeItem> Find(const FName& InName, const FName& InType) const;

private:
	/** The items that are built by this builder */
	TArray<TSharedPtr<class ISkeletonTreeItem>>& Items;

	/** A linearized list of all items in OutItems (for easier searching) */
	TArray<TSharedPtr<class ISkeletonTreeItem>>& LinearItems;
};


/** Basic filter used when re-filtering the tree */
struct FSkeletonTreeFilterArgs
{
	FSkeletonTreeFilterArgs(TSharedPtr<FTextFilterExpressionEvaluator> InTextFilter)
		: TextFilter(InTextFilter)
		, bFlattenHierarchyOnFilter(true)
	{}

	/** The text filter we are using, if any */
	TSharedPtr<FTextFilterExpressionEvaluator> TextFilter;

	/** Whether to flatten the hierarchy so filtered items appear in a linear list */
	bool bFlattenHierarchyOnFilter;
};

/** Delegate used to filter an item. */
DECLARE_DELEGATE_RetVal_TwoParams(ESkeletonTreeFilterResult, FOnFilterSkeletonTreeItem, const FSkeletonTreeFilterArgs& /*InArgs*/, const TSharedPtr<class ISkeletonTreeItem>& /*InItem*/);

/** Interface to implement to provide custom build logic to skeleton trees */
class ISkeletonTreeBuilder : public TSharedFromThis<ISkeletonTreeBuilder>
{
	public:
	virtual ~ISkeletonTreeBuilder() {};

	/** Setup this builder with links to the tree and preview scene */
	virtual void Initialize(const TSharedRef<class ISkeletonTree>& InSkeletonTree, const TSharedPtr<class IPersonaPreviewScene>& InPreviewScene, FOnFilterSkeletonTreeItem InOnFilterSkeletonTreeItem) = 0;

	/**
	 * Build an array of skeleton tree items to display in the tree.
	 * @param	Output			The items that are built by this builder
	 */
	virtual void Build(FNewtonModelSkeletonTreeBuilderOutput& Output) = 0;

	/** Apply filtering to the tree items */
	virtual void Filter(const FSkeletonTreeFilterArgs& InArgs, const TArray<TSharedPtr<class ISkeletonTreeItem>>& InItems, TArray<TSharedPtr<class ISkeletonTreeItem>>& OutFilteredItems) = 0;

	/** Allows the builder to contribute to filtering an item */
	virtual ESkeletonTreeFilterResult FilterItem(const FSkeletonTreeFilterArgs& InArgs, const TSharedPtr<class ISkeletonTreeItem>& InItem) = 0;

	/** Get whether this builder is showing bones */
	virtual bool IsShowingBones() const = 0;

	/** Get whether this builder is showing bones */
	virtual bool IsShowingSockets() const = 0;

	/** Get whether this builder is showing bones */
	virtual bool IsShowingAttachedAssets() const = 0;
};



/** Options for skeleton building */
struct FSkeletonTreeBuilderArgs
{
	FSkeletonTreeBuilderArgs(bool bInShowBones = true, bool bInShowSockets = true, bool bInShowAttachedAssets = true, bool bInShowVirtualBones = true)
		: bShowBones(bInShowBones)
		, bShowSockets(bInShowSockets)
		, bShowAttachedAssets(bInShowAttachedAssets)
		, bShowVirtualBones(bInShowVirtualBones)
	{}

	/** Whether we should show bones */
	bool bShowBones;

	/** Whether we should show sockets */
	bool bShowSockets;

	/** Whether we should show attached assets */
	bool bShowAttachedAssets;

	/** Whether we should show virtual bones */
	bool bShowVirtualBones;
};


class FSkeletonTreeBuilder : public ISkeletonTreeBuilder
{
	public:
	enum ESocketParentType
	{
		Skeleton,
		Mesh
	};

	FSkeletonTreeBuilder(const FSkeletonTreeBuilderArgs& InBuilderArgs);

	/** ISkeletonTreeBuilder interface */
	virtual void Initialize(const TSharedRef<class ISkeletonTree>& InSkeletonTree, const TSharedPtr<class IPersonaPreviewScene>& InPreviewScene, FOnFilterSkeletonTreeItem InOnFilterSkeletonTreeItem) override;
	virtual void Build(FNewtonModelSkeletonTreeBuilderOutput& Output) override;
	virtual void Filter(const FSkeletonTreeFilterArgs& InArgs, const TArray<TSharedPtr<class ISkeletonTreeItem>>& InItems, TArray<TSharedPtr<class ISkeletonTreeItem>>& OutFilteredItems) override;
	virtual ESkeletonTreeFilterResult FilterItem(const FSkeletonTreeFilterArgs& InArgs, const TSharedPtr<class ISkeletonTreeItem>& InItem) override;
	virtual bool IsShowingBones() const override;
	virtual bool IsShowingSockets() const override;
	virtual bool IsShowingAttachedAssets() const override;

protected:
	/** Add Bones */
	void AddBones(FNewtonModelSkeletonTreeBuilderOutput& Output);

	/** Add Sockets */
	void AddSockets(FNewtonModelSkeletonTreeBuilderOutput& Output);

	void AddAttachedAssets(FNewtonModelSkeletonTreeBuilderOutput& Output);

	void AddSocketsFromData(const TArray< USkeletalMeshSocket* >& SocketArray, ESocketParentType ParentType, FNewtonModelSkeletonTreeBuilderOutput& Output);

	void AddAttachedAssetContainer(const FPreviewAssetAttachContainer& AttachedObjects, FNewtonModelSkeletonTreeBuilderOutput& Output);

	void AddVirtualBones(FNewtonModelSkeletonTreeBuilderOutput& Output);

	/** Create an item for a bone */
	TSharedRef<class ISkeletonTreeItem> CreateBoneTreeItem(const FName& InBoneName);

	/** Create an item for a socket */
	TSharedRef<class ISkeletonTreeItem> CreateSocketTreeItem(USkeletalMeshSocket* InSocket, ESocketParentType ParentType, bool bIsCustomized);

	/** Create an item for an attached asset */
	TSharedRef<class ISkeletonTreeItem> CreateAttachedAssetTreeItem(UObject* InAsset, const FName& InAttachedTo);

	/** Create an item for a virtual bone */
	TSharedRef<class ISkeletonTreeItem> CreateVirtualBoneTreeItem(const FName& InBoneName);

	/** Helper function for filtering */
	ESkeletonTreeFilterResult FilterRecursive(const FSkeletonTreeFilterArgs& InArgs, const TSharedPtr<ISkeletonTreeItem>& InItem, TArray<TSharedPtr<ISkeletonTreeItem>>& OutFilteredItems);

protected:
	/** Options for building */
	FSkeletonTreeBuilderArgs BuilderArgs;

	/** Delegate used for filtering */
	FOnFilterSkeletonTreeItem OnFilterSkeletonTreeItem;

	/** The skeleton tree we will build against */
	TWeakPtr<class ISkeletonTree> SkeletonTreePtr;

	/** The editable skeleton that the skeleton tree represents */
	TWeakPtr<class IEditableSkeleton> EditableSkeletonPtr;

	/** The (optional) preview scene we will build against */
	TWeakPtr<class IPersonaPreviewScene> PreviewScenePtr;
};