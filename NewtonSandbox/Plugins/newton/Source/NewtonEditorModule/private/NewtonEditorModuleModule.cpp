#include "NewtonEditorModuleModule.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Modules/ModuleManager.h"

#include "testButtonStyle.h"
#include "testButtonCommands.h"
#include "NewtonSkeletalMeshAction.h"


IMPLEMENT_MODULE(FNewtonEditorModule, NewtonEditorModule);

void FNewtonEditorModule::StartupModule()
{
	CreateEditorToolbarButton();
	RegisterNewtonSkeletalMeshAsset();
}

void FNewtonEditorModule::ShutdownModule()
{
	DestroyToolbarButton();
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


void FNewtonEditorModule::RegisterNewtonSkeletalMeshAsset()
{
	IAssetTools& assetTools = IAssetTools::Get();
	const FName name(TEXT("NewtonSkeletalMesh"));
	const FText showName(FText::FromString("Newton Skeletal Mesh"));
	EAssetTypeCategories::Type assetType = assetTools.RegisterAdvancedAssetCategory(name, showName);

	TSharedPtr<NewtonSkeletalMeshAction> skeletalMesh (MakeShareable(new NewtonSkeletalMeshAction(assetType)));
	assetTools.RegisterAssetTypeActions(skeletalMesh.ToSharedRef());
}