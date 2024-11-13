// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class NewtonSkeletalMeshEditor;


/**
 * 
 */
class NEWTONEDITORMODULE_API NewtonSkeletalMeshTabFactory0: public FWorkflowTabFactory
{
	public:
	NewtonSkeletalMeshTabFactory0(const TSharedPtr<NewtonSkeletalMeshEditor>& editor);
	~NewtonSkeletalMeshTabFactory0();

	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const;
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

	TWeakPtr<NewtonSkeletalMeshEditor> m_editor;

	static FName m_primaryTabName;
};
