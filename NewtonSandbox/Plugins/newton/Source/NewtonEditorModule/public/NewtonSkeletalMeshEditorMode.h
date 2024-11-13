// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"


class NewtonSkeletalMeshEditor;
/**
 * 
 */
class NEWTONEDITORMODULE_API NewtonSkeletalMeshEditorMode: public FApplicationMode
{
	public:
	NewtonSkeletalMeshEditorMode(TSharedPtr<NewtonSkeletalMeshEditor> editor);
	~NewtonSkeletalMeshEditorMode();

	virtual void PostActivateMode() override;
	virtual void PreDeactivateMode() override;
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> inTabManager) override;

	FWorkflowAllowedTabSet m_tabs;
	TWeakPtr<NewtonSkeletalMeshEditor> m_editor;

	static FName m_editorModeName;
	static FName m_editorVersionName;
};
