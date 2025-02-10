#include "NewtonModelModule.h"
#include "IAssetTools.h"
#include "PersonaModule.h"
#include "AssetToolsModule.h"
#include "Styling/SlateStyle.h"
#include "Modules/ModuleManager.h"
#include "ISkeletonEditorModule.h"
#include "IPinnedCommandListModule.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#include "testButtonStyle.h"
#include "testButtonCommands.h"

#include "NewtonModelAction.h"
#include "NewtonModelEditorCommon.h"
#include "NewtonModelVehicleAction.h"
#include "NewtonModelThumbnailRenderer.h"


IMPLEMENT_MODULE(FNewtonModelModule, NewtonModelModule);


void FNewtonModelModule::StartupModule()
{
	m_styleSet = nullptr;
	m_newtonModelAction = nullptr;

	CreateEditorToolbarButton();
	RegisterNewtonModelEditor();
}

void FNewtonModelModule::ShutdownModule()
{
	DestroyToolbarButton();
	UnregisterNewtonModelEditor();
}

void FNewtonModelModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FtestButtonCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FtestButtonCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

void FNewtonModelModule::CreateEditorToolbarButton()
{
	m_toobarCount = 0;
	FtestButtonStyle::Initialize();
	FtestButtonStyle::ReloadTextures();

	FtestButtonCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FtestButtonCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FNewtonModelModule::ToolbarUpdate),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FNewtonModelModule::RegisterMenus));
}

void FNewtonModelModule::DestroyToolbarButton()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FtestButtonStyle::Shutdown();
	FtestButtonCommands::Unregister();
}

void FNewtonModelModule::ToolbarUpdate()
{
	m_toobarCount++;
}

void FNewtonModelModule::CreateIcons()
{
	m_styleSet = MakeShareable(new FSlateStyleSet(ND_NEWTON_MODEL_DISPLAY_NAME));
	TSharedPtr<IPlugin> plugin(IPluginManager::Get().FindPlugin(TEXT("newton")));
	const FString resourceDir(plugin->GetBaseDir() / TEXT("Resources"));
	m_styleSet->SetContentRoot(resourceDir);

	auto LoadIcon = [this](const FName iconName)
	{
		const FVector2D iconSize(16.0f, 16.0f);
		const FString iconPath(m_styleSet->RootToContentDir(iconName.ToString()));
		FSlateImageBrush* const icon = new FSlateImageBrush(iconPath, iconSize);
		m_styleSet->Set(iconName, icon);
	};

	LoadIcon(TEXT("bodyIcon.png"));
	LoadIcon(TEXT("loopIcon.png"));
	LoadIcon(TEXT("jointIcon.png"));
	LoadIcon(TEXT("shapeIcon.png"));
	LoadIcon(TEXT("boneMapping.png"));

	FSlateStyleRegistry::RegisterSlateStyle(*m_styleSet);
	UThumbnailManager::Get().RegisterCustomRenderer(UNewtonAsset::StaticClass(), UNewtonModelThumbnailRenderer::StaticClass());
}

void FNewtonModelModule::UnreagiterIcons()
{
	if (m_styleSet)
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*m_styleSet);
		ensure(m_styleSet.IsUnique());
		m_styleSet.Reset();
	}

	if (UObjectInitialized())
	{
		UThumbnailManager::Get().UnregisterCustomRenderer(UNewtonModelThumbnailRenderer::StaticClass());
	}
}

void FNewtonModelModule::RegisterNewtonModelEditor()
{
	// load dependency modules
	FModuleManager::LoadModuleChecked<FPersonaModule>("Persona");
	FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	FModuleManager::LoadModuleChecked<ISkeletonEditorModule>("SkeletonEditor");
	FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	FModuleManager::LoadModuleChecked<IPinnedCommandListModule>(TEXT("PinnedCommandList"));

	// register the asset icon
	CreateIcons();

	// register the asset menu item entry
	IAssetTools& assetTools = IAssetTools::Get();

	// register the menu action.
	const FName name(TEXT("newtonPhysics"));
	const FText showName(FText::FromString(TEXT("Newton Physics")));
	EAssetTypeCategories::Type assetType = assetTools.RegisterAdvancedAssetCategory(name, showName);

	m_newtonModelAction = MakeShareable(new FNewtonModelAction(assetType));
	assetTools.RegisterAssetTypeActions(m_newtonModelAction.ToSharedRef());

	//const FName name1(TEXT("newtonPhyyyysics"));
	//EAssetTypeCategories::Type assetType1 = assetTools.RegisterAdvancedAssetCategory(name1, showName);
	m_newtonModelVehicleAction = MakeShareable(new FNewtonModelVehicleAction(assetType));
	assetTools.RegisterAssetTypeActions(m_newtonModelVehicleAction.ToSharedRef());
}

void FNewtonModelModule::UnregisterNewtonModelEditor()
{
	UnreagiterIcons();

	//IAssetTools& assetTools = IAssetTools::Get();
	//assetTools.UnregisterAssetTypeActions(m_newtonModelAction.ToSharedRef());
}

const FSlateBrush* FNewtonModelModule::GetBrush(const FName propertyName) const
{
	const FSlateBrush* const brush = m_styleSet->GetBrush(propertyName);
	return brush;
}
