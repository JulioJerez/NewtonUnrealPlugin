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
#include "ndJoints/NewtonJoint.h"
#include "NewtonJointCylinder.generated.h"

class ANewtonWorldActor;

/**
 * 
 */
UCLASS(ClassGroup=(NewtonJoints), meta=(BlueprintSpawnableComponent))
class UNewtonJointCylinder : public UNewtonJoint
{
	GENERATED_BODY()

	public:
	// Sets default values for this component's properties
	UNewtonJointCylinder();

	protected:
	void DrawLinearGizmo(float timestep) const;
	void DrawAngularGizmo(float timestep) const;
	virtual void DrawGizmo(float timestep) const override;
	virtual void CreateJoint(ANewtonWorldActor* const worldActor) override;

	UPROPERTY(EditAnywhere, Category = Newton)
	bool EnableAngularLimits;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = -10000.0f, ClampMax = -1.0f))
	float MinAngle;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 1.0f, ClampMax = 10000.0f))
	float MaxAngle;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 10000.0f))
	float SpringAngularConst;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
	float DampingAngularConst;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0001f, ClampMax = 1.0f))
	float SpringDamperAngularRegularizer;

	UPROPERTY(EditAnywhere, Category = Newton)
	bool EnableLinearLimits;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = -10000.0f, ClampMax = -0.01f))
	float MinDistance;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.01f, ClampMax = 10000.0f))
	float MaxDistance;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 10000.0f))
	float SpringLinearConst;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
	float DampingLinearConst;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0001f, ClampMax = 1.0f))
	float SpringDamperLinearRegularizer;
};
