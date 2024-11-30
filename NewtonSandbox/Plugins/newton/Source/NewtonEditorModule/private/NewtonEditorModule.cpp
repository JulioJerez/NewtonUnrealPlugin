#include "NewtonEditorModule.h"
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
#include "factories/NewtonModelPinFactory.h"

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

void FNewtonEditorModule::CreateIcons()
{
	m_styleSet = MakeShareable(new FSlateStyleSet(ND_MESH_EDITOR_NAME));
	TSharedPtr<IPlugin> plugin(IPluginManager::Get().FindPlugin(TEXT("newton")));
	const FString resourceDir(plugin->GetBaseDir() / TEXT("Resources"));
	m_styleSet->SetContentRoot(resourceDir);

	const FVector2D iconSize(16.0f, 16.0f);
	const FString iconPath(m_styleSet->RootToContentDir(TEXT("ndModelIcon.png")));
	FSlateImageBrush* const newtonIcon = new FSlateImageBrush(iconPath, iconSize);
	FSlateImageBrush* const nodeAddPinIcon = new FSlateImageBrush(iconPath, iconSize);
	FSlateImageBrush* const nodeDeletePinIcon = new FSlateImageBrush(iconPath, iconSize);
	FSlateImageBrush* const nodeDeleteNodeIcon = new FSlateImageBrush(iconPath, iconSize);
	FSlateImageBrush* const newtonThumbnail = new FSlateImageBrush(iconPath, iconSize);

	m_styleSet->Set(TEXT("ClassIcon.NewtonModel"), newtonIcon);
	m_styleSet->Set(TEXT("ClassThumbnail.NewtonModel"), newtonThumbnail);
	m_styleSet->Set(TEXT("FNewtonModelEditor.NodeAddPinIcon"), nodeAddPinIcon);
	m_styleSet->Set(TEXT("FNewtonModelEditor.NodeDeletePinIcon"), nodeDeletePinIcon);
	m_styleSet->Set(TEXT("FNewtonModelEditor.NodeDeleteNodeIcon"), nodeDeleteNodeIcon);

	FSlateStyleRegistry::RegisterSlateStyle(*m_styleSet);
}

void FNewtonEditorModule::UnreagiterIcons()
{
	if (m_styleSet)
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*m_styleSet);
		ensure(m_styleSet.IsUnique());
		m_styleSet.Reset();
	}
}

void FNewtonEditorModule::RegisterNewtonModelEditor()
{
	// register the asset icon
	//CreateIcons();

	// register the asset menu item entry
	IAssetTools& assetTools = IAssetTools::Get();
	const FName name(TEXT("newtonPhysics"));
	const FText showName(FText::FromString(TEXT("Newton Physics")));
	EAssetTypeCategories::Type assetType = assetTools.RegisterAdvancedAssetCategory(name, showName);

	// register the menu action.
	m_newtonModelAction = MakeShareable(new NewtonModelAction(assetType));
	assetTools.RegisterAssetTypeActions(m_newtonModelAction.ToSharedRef());

	m_customPinFactory = MakeShareable(new FNewtonModelPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(m_customPinFactory);

	// load dependency modules
	FModuleManager::LoadModuleChecked<FPersonaModule>("Persona");
	FModuleManager::LoadModuleChecked<ISkeletonEditorModule>("SkeletonEditor");
	FModuleManager::LoadModuleChecked<IPinnedCommandListModule>(TEXT("PinnedCommandList"));
}

void FNewtonEditorModule::UnregisterNewtonModelEditor()
{
	UnreagiterIcons();

	FEdGraphUtilities::UnregisterVisualPinFactory(m_customPinFactory);
	ensure(m_customPinFactory.IsUnique());
	m_customPinFactory.Reset();

	//IAssetTools& assetTools = IAssetTools::Get();
	//assetTools.UnregisterAssetTypeActions(m_newtonModelAction.ToSharedRef());
}