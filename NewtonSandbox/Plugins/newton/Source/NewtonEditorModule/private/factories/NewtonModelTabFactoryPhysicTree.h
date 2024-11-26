// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "Widgets/SWidget.h"
//#include "Toolkits/AssetEditorToolkit.h"
//#include "ISkeletonTree.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

//class SToolTip;
class FNewtonModelEditor;

class FNewtonModelTabFactoryPhysicTree : public FWorkflowTabFactory
{
	public:
	//FNewtonModelTabFactoryPhysicTree(TSharedPtr<class FAssetEditorToolkit> InHostingApp, TSharedRef<class ISkeletonTree> InSkeletonTree);
	FNewtonModelTabFactoryPhysicTree(const TSharedPtr<FNewtonModelEditor>& editor);

	/** FWorkflowTabFactory interface */
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const;
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	//virtual TSharedPtr<SToolTip> CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const override;

	/** Reference to our skeleton tree */
	//TWeakPtr<class ISkeletonTree> SkeletonTreePtr;
	TWeakPtr<FNewtonModelEditor> m_editor;
};
