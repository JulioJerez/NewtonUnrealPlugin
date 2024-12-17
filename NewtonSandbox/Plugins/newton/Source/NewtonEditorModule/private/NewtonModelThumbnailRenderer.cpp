// Fill out your copyright notice in the Description page of Project Settings.


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
	UNewtonModel* const newtonModel = Cast<UNewtonModel>(object);
	check(newtonModel);
	const USkeletalMesh* const skeletalMesh = newtonModel->SkeletalMeshAsset;
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
	
	UNewtonModel* const newtonModel = Cast<UNewtonModel>(object);
	check(newtonModel);

	USkeletalMesh* const skeletalMesh = newtonModel->SkeletalMeshAsset;
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
	//UPackage* const package = newtonModel->GetPackage();
	//const FString packageName(package->GetName());
	//xxxx.SaveExternalCache()

	thumbnailScene->SetSkeletalMesh(nullptr);
}