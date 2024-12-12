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
#include "NewtonModelNode.h"
#include "NewtonRigidBody.h"
#include "NewtonModelNodeRigidBody.generated.h"


/**
 * 
 */
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonModelNodeRigidBody : public UNewtonModelNode
{
	GENERATED_BODY()
	public:
	UNewtonModelNodeRigidBody();

	virtual void DebugCenterOfMass(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const override;

	UPROPERTY(EditAnywhere)
	FTransform Transform;

	UPROPERTY(EditAnywhere)
	bool ShowDebug;

	UPROPERTY(EditAnywhere)
	bool ShowCenterOfMass;

	UPROPERTY(EditAnywhere)
	bool AutoSleepMode;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.1f, ClampMax = 4.0f))
	float DebugScale;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0f))
	float Mass;

	UPROPERTY(EditAnywhere)
	FNewtonRigidBodyInertia Inertia;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float LinearDamp;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float AngularDamp;

	UPROPERTY(EditAnywhere)
	FVector InitialVeloc;

	UPROPERTY(EditAnywhere)
	FVector InitialOmega;

	UPROPERTY(EditAnywhere)
	FVector CenterOfMass;

	UPROPERTY(EditAnywhere)
	FVector Gravity;


	UPROPERTY(VisibleAnywhere)
	FName BoneName;

	UPROPERTY(VisibleAnywhere)
	int BoneIndex;
};
