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
	TObjectInstanceThumbnailScene<FSkeletalMeshThumbnailScene, 128> m_thumbnailSceneCache;
};
