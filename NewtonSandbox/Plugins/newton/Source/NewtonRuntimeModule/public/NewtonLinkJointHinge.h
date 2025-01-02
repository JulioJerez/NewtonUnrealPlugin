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
#include "NewtonLinkJoint.h"
#include "NewtonLinkJointHinge.generated.h"

/**
 * 
 */
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonLinkJointHinge : public UNewtonLinkJoint
{
	GENERATED_BODY()

	public:
	UNewtonLinkJointHinge();

	virtual TObjectPtr<USceneComponent> CreateBlueprintProxy() const override;
	void InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const override;

	UPROPERTY(EditAnywhere)
	bool EnableLimits;

	UPROPERTY(EditAnywhere, meta = (ClampMin = -10000.0f, ClampMax = -1.0f))
	float MinAngle;
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.0f, ClampMax = 10000.0f))
	float MaxAngle;

	UPROPERTY(EditAnywhere, meta = (ClampMin = -10000.0f, ClampMax = 10000.0f))
	float ProptionalDerivativeAngle;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0f, ClampMax = 10000.0f))
	float SpringConst;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
	float DampingConst;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0001f, ClampMax = 1.0f))
	float SpringDamperRegularizer;
};
