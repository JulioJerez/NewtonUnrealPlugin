/* Copyright (c) <2024-2024> <Julio Jerez, Newton Game Dynamics>
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
*
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any source distribution.
*/


#include "testButtonStyle.h"
#include "Slate/SlateGameResources.h"
#include "Styling/SlateStyleMacros.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"

TSharedPtr<FSlateStyleSet> FtestButtonStyle::styleInstance = nullptr;

void FtestButtonStyle::Initialize()
{
	if (!styleInstance.IsValid())
	{
		styleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*styleInstance);
	}
}

void FtestButtonStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*styleInstance);
	ensure(styleInstance.IsUnique());
	styleInstance.Reset();
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
	return *styleInstance;
}
