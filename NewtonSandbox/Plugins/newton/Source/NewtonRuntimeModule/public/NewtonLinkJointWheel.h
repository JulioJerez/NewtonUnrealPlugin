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
#include "NewtonLinkJointWheel.generated.h"

/**
 * 
 */
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonLinkJointWheel : public UNewtonLinkJoint
{
	GENERATED_BODY()

	public:
	UNewtonLinkJointWheel();

	virtual TObjectPtr<USceneComponent> CreateBlueprintProxy() const override;
	void InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const override;

	UPROPERTY(EditAnywhere, meta = (ClampMin = -1.0f, ClampMax = 1000.0f))
	float Radio;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 100.0f, ClampMax = 100000.0f))
	float SpringK;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 10.0f, ClampMax = 1000.0f))
	float DamperC;

	UPROPERTY(EditAnywhere, meta = (ClampMin = -0.0f, ClampMax = 50.0f))
	float UpperStop;

	UPROPERTY(EditAnywhere, meta = (ClampMin = -50.0f, ClampMax = 0.0f))
	float LowerStop;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.00001f, ClampMax = 1.0f))
	float Regularizer;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0f, ClampMax = 100000.0f))
	float BrakeTorque;

	UPROPERTY(EditAnywhere, meta = (ClampMin = -45.0f, ClampMax = -45.0f))
	float SteeringAngle;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0f, ClampMax = 100000.0f))
	float HandBrakeTorque;

};
