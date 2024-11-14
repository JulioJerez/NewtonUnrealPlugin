// Copyright Epic Games, Inc. All Rights Reserved.

#include "testButtonStyle.h"
#include "Slate/SlateGameResources.h"
#include "Styling/SlateStyleMacros.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"

TSharedPtr<FSlateStyleSet> FtestButtonStyle::StyleInstance = nullptr;

void FtestButtonStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FtestButtonStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FtestButtonStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("testButtonStyle"));
	return StyleSetName;
}

TSharedRef< FSlateStyleSet > FtestButtonStyle::Create()
{
	TSharedRef<FSlateStyleSet> styleSet(MakeShareable(new FSlateStyleSet(TEXT("testButtonStyle"))));
	TSharedPtr<IPlugin> plugin (IPluginManager::Get().FindPlugin(TEXT("newton")));
	const FString resourceDir(plugin->GetBaseDir() / TEXT("Resources"));
	styleSet->SetContentRoot(resourceDir);
	const FVector2D iconSize(40.0f, 40.0f);

	const FString iconPath(styleSet->RootToContentDir(TEXT("newtonIcon.png")));
	FSlateImageBrush* const newtonIcon = new FSlateImageBrush(iconPath, iconSize);
	styleSet->Set(TEXT("newton.PluginAction"), newtonIcon);
	return styleSet;
}

void FtestButtonStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FtestButtonStyle::Get()
{
	return *StyleInstance;
}
