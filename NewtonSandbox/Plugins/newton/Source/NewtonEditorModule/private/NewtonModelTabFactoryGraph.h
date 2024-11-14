// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class NewtonModelEditor;

//This is the primary tab

/**
 * 
 */
class NEWTONEDITORMODULE_API NewtonModelTabFactoryGraph: public FWorkflowTabFactory
{
	public:
	NewtonModelTabFactoryGraph(const TSharedPtr<NewtonModelEditor>& editor);
	~NewtonModelTabFactoryGraph();

	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const;
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

	TWeakPtr<NewtonModelEditor> m_editor;

	static FName m_tabName;
};
