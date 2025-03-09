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
#include "NewtonLinkCollision.h"
#include "NewtonLinkCollisionCapsule.generated.h"

/**
 *
 */
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonLinkCollisionCapsule : public UNewtonLinkCollision
{
	GENERATED_BODY()
	UNewtonLinkCollisionCapsule();

	virtual TObjectPtr<USceneComponent> CreateBlueprintProxy() const override;

	virtual ndShapeInstance CreateInstance() const override;
	virtual ndShapeInstance CreateInstance(TObjectPtr<UStaticMesh> mesh) const override;
	virtual ndShapeInstance CreateInstance(TObjectPtr<USkeletalMesh> mesh, int boneIndex) const override;
	virtual void InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const override;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 2.0f))
	float Radio0;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 2.0f))
	float Radio1;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 2.0f))
	float Length;
};
