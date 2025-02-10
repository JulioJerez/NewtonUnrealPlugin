#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FSlateStyleSet;
class FNewtonModelAction;
class FNewtonModelPinFactory;
class FNewtonModelVehicleAction;

#define ND_NEWTON_MODEL_DISPLAY_NAME			TEXT("Newton Model")
#define ND_NEWTON_MODEL_VEHICLE_DISPLAY_NAME	TEXT("Newton Vehicle Model")
#define ND_NEWTON_MODEL_MODULE_NAME				TEXT("NewtonModelModule")

class FNewtonModelModule : public FDefaultModuleImpl
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
	TSharedPtr<FNewtonModelAction> m_newtonModelAction;
	TSharedPtr<FNewtonModelVehicleAction> m_newtonModelVehicleAction;
};
