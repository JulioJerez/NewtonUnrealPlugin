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
#include "NewtonLink.h"
#include "NewtonLinkStaticMesh.generated.h"

class UNewtonJoint;

/**
 * 
 */
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonLinkStaticMesh : public UNewtonLink
{
	GENERATED_BODY()

	public:
	UNewtonLinkStaticMesh();

	void CreateWireFrameMesh(TArray<FVector>& wireFrameMeshOut) const;
	TObjectPtr<USceneComponent> CreateBlueprintProxy() const override;
	void InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const override;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMesh> StaticMesh;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (AllowPrivateAccess = "true"))
	bool ExcludeFromBlueprint;
};
