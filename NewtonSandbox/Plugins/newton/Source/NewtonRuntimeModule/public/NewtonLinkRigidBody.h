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
#include "NewtonRigidBody.h"
#include "NewtonLinkRigidBody.generated.h"

class UNewtonLinkCollision;

UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonLinkRigidBody : public UNewtonLink
{
	GENERATED_BODY()
	public:
	UNewtonLinkRigidBody();

	virtual TObjectPtr<USceneComponent> CreateBlueprintProxy() const override;
	virtual void InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const override;
	FVector CalculateLocalCenterOfMass(TObjectPtr<USkeletalMesh> mesh, int boneIndex, const FTransform& globalTransform, const TArray<const UNewtonLinkCollision*>& childenShapes) const;

	UPROPERTY(VisibleAnywhere)
	FName BoneName;

	UPROPERTY(VisibleAnywhere)
	int BoneIndex;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.1f))
	float Mass;

	UPROPERTY(EditAnywhere)
	FVector CenterOfMass;

	UPROPERTY()
	FVector ShapeGeometricCenter;

	UPROPERTY(EditAnywhere)
	float DebugScale;
};
