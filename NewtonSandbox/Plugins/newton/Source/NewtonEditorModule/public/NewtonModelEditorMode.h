// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"


class NewtonModelEditor;
/**
 * 
 */
class NEWTONEDITORMODULE_API NewtonModelEditorMode: public FApplicationMode
{
	public:
	NewtonModelEditorMode(TSharedPtr<NewtonModelEditor> editor);
	~NewtonModelEditorMode();

	virtual void PostActivateMode() override;
	virtual void PreDeactivateMode() override;
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> inTabManager) override;

	FWorkflowAllowedTabSet m_tabs;
	TWeakPtr<NewtonModelEditor> m_editor;

	static FName m_editorModelName;
	static FName m_editorVersionName;
};
