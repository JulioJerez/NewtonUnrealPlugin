// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "Widgets/SWidget.h"
//#include "Toolkits/AssetEditorToolkit.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

//class SToolTip;
class FNewtonModelEditor;

class FNewtonModelTabFactoryPhysicsTree : public FWorkflowTabFactory
{
	public:
	FNewtonModelTabFactoryPhysicsTree(const TSharedPtr<FNewtonModelEditor>& editor);

	/** FWorkflowTabFactory interface */
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const;
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	//virtual TSharedPtr<SToolTip> CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const override;

	/** Reference to our skeleton tree */
	TWeakPtr<FNewtonModelEditor> m_editor;
};
