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
#include "NewtonLinkLoop.h"
#include "NewtonLinkLoopEffector6dof.generated.h"

/**
 * 
 */
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonLinkLoopEffector6dof : public UNewtonLinkLoop
{
	GENERATED_BODY()

	public:
	UNewtonLinkLoopEffector6dof();

	virtual TObjectPtr<USceneComponent> CreateBlueprintProxy() const override;
	void InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const override;

	UPROPERTY(EditAnywhere, Category = Newton)
	FTransform TargetFrame;

	//UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 20000.0f))
	//float AngularSpring;
	//
	//UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 500.0f))
	//float AngularDamper;
	//
	//UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = -10.0f))
	//float AngularMaxTorque;
	//
	//UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.00001f, ClampMax = 0.99f))
	//float AngularRegularizer;
	//
	//UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 20000.0f))
	//float LinearSpring;
	//
	//UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 20000.0f))
	//float LinearDamper;
	//
	//UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 10.0f))
	//float LinearMaxForce;
	//
	//UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.00001f, ClampMax = 0.99f))
	//float LinearRegularizer;
};