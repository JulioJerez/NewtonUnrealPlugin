#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FSlateStyleSet;
class NewtonModelAction;
class FNewtonModelPinFactory;

#define ND_MESH_EDITOR_NAME TEXT("NewtonModel")

class FNewtonEditorModule : public FDefaultModuleImpl
{
	public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	const FSlateBrush* GetBrush(const FName propertyName) const;

	private:
	void CreateIcons();
	void RegisterMenus();
	void ToolbarUpdate();
	void DestroyToolbarButton();
	void CreateEditorToolbarButton();

	void UnreagiterIcons();

	void RegisterNewtonModelEditor();
	void UnregisterNewtonModelEditor();

	TSharedPtr<class FUICommandList> PluginCommands;
	int m_toobarCount;

	TSharedPtr<FSlateStyleSet> m_styleSet;
	TSharedPtr<NewtonModelAction> m_newtonModelAction;
};
