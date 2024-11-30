// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Widgets/SCompoundWidget.h"



#include "ndTree/NewtonModelPhysicsTreeBuilder.h"


class IPinnedCommandList;
class FUICommandList_Pinnable;
class FNewtonModelPhysicsTreeEditableSkeleton;


struct FNewtonModelPhysicsTreeArgs
{
	FNewtonModelPhysicsTreeArgs()
		//: Mode(ESkeletonTreeMode::Editor)
		:ContextName(TEXT("PhysicsTree"))
		//, bShowBlendProfiles(true)
		//, bShowFilterMenu(true)
		//, bShowDebugVisualizationOptions(false)
		//, bAllowMeshOperations(true)
		//, bAllowSkeletonOperations(true)
		//, bHideBonesByDefault(false)
	{
	}

	///** Delegate called by the tree when a socket is selected */
	//FOnSkeletonTreeSelectionChanged OnSelectionChanged;
	//
	///** Delegate that allows custom filtering text to be shown on the filter button */
	//FOnGetFilterText OnGetFilterText;
	//
	///** Optional preview scene that we can pair with */
	//TSharedPtr<IPersonaPreviewScene> PreviewScene;
	//
	///** Optional builder to allow for custom tree construction */
	//TSharedPtr<ISkeletonTreeBuilder> Builder;
	//
	///** The mode that this skeleton tree is in */
	//ESkeletonTreeMode Mode;
	
	///** Whether to show the blend profiles editor for the skeleton being displayed */
	//bool bShowBlendProfiles;
	//
	///** Whether to show the filter menu to allow filtering of active bones, sockets etc. */
	//bool bShowFilterMenu;
	//
	///** Whether to show the filter option to allow filtering of debug draw elements in the viewport. */
	//bool bShowDebugVisualizationOptions;
	//
	///** Whether to allow operations that modify the mesh */
	//bool bAllowMeshOperations;
	//
	///** Whether to allow operations that modify the skeleton */
	//bool bAllowSkeletonOperations;
	//
	///** Whether to hide bones by default */
	//bool bHideBonesByDefault;

	/** Context name used to persist settings */
	FName ContextName;

	/** Menu extenders applied to context and filter menus */
	TSharedPtr<FExtender> Extenders;
};


class FNewtonModelPhysicsTree : public SCompoundWidget
{
	public:

	SLATE_BEGIN_ARGS(FNewtonModelPhysicsTree)
		: _IsEditable(true)
		{
		}

		SLATE_ATTRIBUTE(bool, IsEditable)

		SLATE_ARGUMENT(TSharedPtr<class FNewtonModelPhysicsTreeBuilder>, Builder)

	SLATE_END_ARGS()

	FNewtonModelPhysicsTree();
	~FNewtonModelPhysicsTree();

	void Construct(const FArguments& InArgs, const TSharedRef<FNewtonModelPhysicsTreeEditableSkeleton>& InEditableSkeleton, const FNewtonModelPhysicsTreeArgs& InSkeletonTreeArgs);

	void BindCommands();
	void RegisterNewMenu();

	// Delegates 
	/** Called to display the add new menu */
	TSharedRef< SWidget > CreateNewMenuWidget();

	TWeakPtr<FNewtonModelPhysicsTreeEditableSkeleton> EditableSkeleton;

	/** Context name used to persist settings */
	FName ContextName;

	/** Pinned commands panel */
	TSharedPtr<IPinnedCommandList> PinnedCommands;

	/** Commands that are bound to delegates*/
	TSharedPtr<FUICommandList_Pinnable> UICommandList;

	/** Extenders for menus */
	TSharedPtr<FExtender> Extenders;



	static FName m_menuName;
};
