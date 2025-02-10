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


#include "NewtonAsset.h"
#include "UObject/ObjectSaveContext.h"

#if WITH_EDITOR
#include "Editor.h"
#include "ObjectTools.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "Rendering/SkeletalMeshRenderData.h"
#endif

#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonAsset::UNewtonAsset()
	:Super()
{
	RootBody = nullptr;
	SkeletalMeshAsset = nullptr;

	m_hideShapes = false;
	m_hideJoints = false;
	m_modelType = ModelsType::m_baseModel;
}

void UNewtonAsset::Serialize(FArchive& ar)
{
	Super::Serialize(ar);

#if WITH_EDITOR
	// this is another moronic unreal quirk.
	// force the Thumbnail module to render this thumbnail, so that I can be display on the asset browser.
	if (SkeletalMeshAsset)
	{
		FSkeletalMeshRenderData* const renderData = SkeletalMeshAsset->GetResourceForRendering();
		if (renderData && renderData->LODRenderData.Num())
		{
			ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(this);
		}
	}
#endif
}
