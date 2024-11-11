#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FNewtonEditorModule : public FDefaultModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


	private:
	void RegisterMenus();
	void ToolbarUpdate();
	void DestroyToolbarButton();
	void CreateEditorToolbarButton();

	TSharedPtr<class FUICommandList> PluginCommands;
	int m_toobarCount;
};
