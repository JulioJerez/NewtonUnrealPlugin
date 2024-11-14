// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class NewtonModelEditor;


/**
 * 
 */
class NEWTONEDITORMODULE_API NewtonModelTabFactoryDetail: public FWorkflowTabFactory
{
	public:
	NewtonModelTabFactoryDetail(const TSharedPtr<NewtonModelEditor>& editor);
	~NewtonModelTabFactoryDetail();

	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const;
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

	TWeakPtr<NewtonModelEditor> m_editor;

	static FName m_primaryTabName;
};
