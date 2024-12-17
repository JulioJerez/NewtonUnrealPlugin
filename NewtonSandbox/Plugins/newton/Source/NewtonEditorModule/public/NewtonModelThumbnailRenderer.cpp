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



#include "NewtonModelThumbnailRenderer.h"

#include "NewtonModelEditorCommon.h"
#include "Rendering/SkeletalMeshRenderData.h"

UNewtonModelThumbnailRenderer::UNewtonModelThumbnailRenderer(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

EThumbnailRenderFrequency UNewtonModelThumbnailRenderer::GetThumbnailRenderFrequency(UObject* object) const
{
	return EThumbnailRenderFrequency::Realtime;
	//return EThumbnailRenderFrequency::OnPropertyChange;
}

bool UNewtonModelThumbnailRenderer::CanVisualizeAsset(UObject* object)
{
	Super::CanVisualizeAsset(object);

	bool ret = false;
	UNewtonAsset* const newtonAsset = Cast<UNewtonAsset>(object);
	check(newtonAsset);
	const USkeletalMesh* const skeletalMesh = newtonAsset->SkeletalMeshAsset;
	if (skeletalMesh)
	{
		FSkeletalMeshRenderData* const renderData = skeletalMesh->GetResourceForRendering();
		ret = renderData && renderData->LODRenderData.Num() && skeletalMesh->IsReadyToRenderInThumbnail();
	}
	return ret;
}

void UNewtonModelThumbnailRenderer::BeginDestroy()
{
	thumbnailSceneCache.Clear();
	Super::BeginDestroy();
}

void UNewtonModelThumbnailRenderer::Draw(UObject* object, int32 x, int32 y, uint32 width, uint32 height, FRenderTarget* renderTarget, FCanvas* canvas, bool additionalViewFamily)
{
	//UE_LOG(LogTemp, Warning, TEXT("TODO: remember complete function:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
	TSharedRef<FSkeletalMeshThumbnailScene> thumbnailScene = thumbnailSceneCache.EnsureThumbnailScene(object);
	
	UNewtonAsset* const newtonAsset = Cast<UNewtonAsset>(object);
	check(newtonAsset);

	USkeletalMesh* const skeletalMesh = newtonAsset->SkeletalMeshAsset;
	if (skeletalMesh)
	{
		thumbnailScene->SetSkeletalMesh(skeletalMesh);
	}

	FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(renderTarget, thumbnailScene->GetScene(), FEngineShowFlags(ESFIM_Game))
		.SetTime(UThumbnailRenderer::GetTime())
		.SetAdditionalViewFamily(additionalViewFamily));
	
	viewFamily.EngineShowFlags.LOD = 0;
	viewFamily.EngineShowFlags.MotionBlur = 0;
	viewFamily.EngineShowFlags.DisableAdvancedFeatures();
	RenderViewFamily(canvas, &viewFamily, thumbnailScene->CreateView(&viewFamily, x, y, width, height));

	//FThumbnailRenderingInfo* renderInfo = GUnrealEd->GetThumbnailManager()->GetRenderingInfo(object);
	//FThumbnailExternalCache& xxxx = FThumbnailExternalCache::Get();
	//UPackage* const package = newtonAsset->GetPackage();
	//const FString packageName(package->GetName());
	//xxxx.SaveExternalCache()

	thumbnailScene->SetSkeletalMesh(nullptr);
}