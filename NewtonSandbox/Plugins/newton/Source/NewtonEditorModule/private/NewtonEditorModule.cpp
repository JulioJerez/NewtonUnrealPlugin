#include "NewtonEditorModule.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Styling/SlateStyle.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#include "testButtonStyle.h"
#include "NewtonModelAction.h"
#include "testButtonCommands.h"

IMPLEMENT_MODULE(FNewtonEditorModule, NewtonEditorModule);

void FNewtonEditorModule::StartupModule()
{
	m_styleSet = nullptr;
	m_newtonModelAction = nullptr;

	CreateEditorToolbarButton();
	RegisterNewtonModelEditor();
}

void FNewtonEditorModule::ShutdownModule()
{
	DestroyToolbarButton();
	UnregisterNewtonModelEditor();
}

void FNewtonEditorModule::RegisterMenus()
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

void FNewtonEditorModule::CreateEditorToolbarButton()
{
	m_toobarCount = 0;
	FtestButtonStyle::Initialize();
	FtestButtonStyle::ReloadTextures();

	FtestButtonCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FtestButtonCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FNewtonEditorModule::ToolbarUpdate),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FNewtonEditorModule::RegisterMenus));
}

void FNewtonEditorModule::DestroyToolbarButton()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FtestButtonStyle::Shutdown();
	FtestButtonCommands::Unregister();
}

void FNewtonEditorModule::ToolbarUpdate()
{
	m_toobarCount++;
}


void FNewtonEditorModule::RegisterNewtonModelEditor()
{
	// register the asset icon
	m_styleSet = MakeShareable(new FSlateStyleSet(ND_MESH_EDITOR_NAME));
	TSharedPtr<IPlugin> plugin (IPluginManager::Get().FindPlugin(TEXT("newton")));
	const FString resourceDir(plugin->GetBaseDir() / TEXT("Resources"));
	m_styleSet->SetContentRoot(resourceDir);

	const FVector2D iconSize(16.0f, 16.0f);
	const FString iconPath(m_styleSet->RootToContentDir(TEXT("ndModelIcon.png")));
	FSlateImageBrush* const newtonIcon = new FSlateImageBrush(iconPath, iconSize);
	FSlateImageBrush* const newtonThumbnail = new FSlateImageBrush(iconPath, iconSize);
	m_styleSet->Set(TEXT("ClassIcon.NewtonModel"), newtonIcon);
	m_styleSet->Set(TEXT("ClassThumbnail.NewtonModel"), newtonThumbnail);
	FSlateStyleRegistry::RegisterSlateStyle(*m_styleSet);

	// register the asset menu item entry
	IAssetTools& assetTools = IAssetTools::Get();
	const FName name(ND_MESH_EDITOR_NAME);
	const FText showName(FText::FromString(TEXT("Newton Model")));
	EAssetTypeCategories::Type assetType = assetTools.RegisterAdvancedAssetCategory(name, showName);

	// register the action panes.
	m_newtonModelAction = MakeShareable(new NewtonModelAction(assetType));
	assetTools.RegisterAssetTypeActions(m_newtonModelAction.ToSharedRef());
}

void FNewtonEditorModule::UnregisterNewtonModelEditor()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*m_styleSet);
	ensure(m_styleSet.IsUnique());
	m_styleSet.Reset();

	//IAssetTools& assetTools = IAssetTools::Get();
	//assetTools.UnregisterAssetTypeActions(m_newtonModelAction.ToSharedRef());
}