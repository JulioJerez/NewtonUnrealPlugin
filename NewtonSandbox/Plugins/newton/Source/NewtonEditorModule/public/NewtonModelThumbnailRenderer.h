// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThumbnailHelpers.h"
#include "ThumbnailRendering/ThumbnailRenderer.h"
#include "NewtonModelThumbnailRenderer.generated.h"

UCLASS()
class UNewtonModelThumbnailRenderer : public UThumbnailRenderer
{
	GENERATED_UCLASS_BODY()
	public:

	virtual void BeginDestroy() override;
	virtual bool CanVisualizeAsset(UObject* Object) override;
	virtual EThumbnailRenderFrequency GetThumbnailRenderFrequency(UObject* Object) const override;
	virtual void Draw(UObject* object, int32 x, int32 y, uint32 width, uint32 height, FRenderTarget* renderTarget, FCanvas* canvas, bool additionalViewFamily) override;

	protected:
	TObjectInstanceThumbnailScene<FSkeletalMeshThumbnailScene, 128> thumbnailSceneCache;
};
