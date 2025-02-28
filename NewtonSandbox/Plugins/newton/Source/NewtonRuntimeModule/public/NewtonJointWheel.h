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
#include "NewtonJoint.h"
#include "NewtonJointWheel.generated.h"

class ndWorld;
class ANewtonWorldActor;
/**
 * 
 */
UCLASS(ClassGroup = NewtonJoints, meta = (BlueprintSpawnableComponent))
class UNewtonJointWheel : public UNewtonJoint
{
	GENERATED_BODY()
	
	public:
	// Sets default values for this component's properties
	UNewtonJointWheel();

	virtual void DrawGizmo(float timestep) const override;
	virtual ndJointBilateralConstraint* CreateJoint() override;

	UFUNCTION(BlueprintCallable, Category = Newton)
	float GetSteering() const;

	UFUNCTION(BlueprintCallable, Category = Newton)
	float GetBreak() const;

	UFUNCTION(BlueprintCallable, Category = Newton)
	float GetHandBreak() const;

	UFUNCTION(BlueprintCallable, Category = Newton)
	void SetSteering(float parametricAngle);

	UFUNCTION(BlueprintCallable, Category = Newton)
	void SetBreak(float parametricBreak);

	UFUNCTION(BlueprintCallable, Category = Newton)
	void SetHandBreak(float parametricBreak);

	UFUNCTION(BlueprintCallable, Category = Newton)
	float GetSuspensionPosit() const;

	UFUNCTION(BlueprintCallable, Category = Newton)
	float GetSuspensionSpeed() const;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 100.0f, ClampMax = 100000.0f))
	float SpringK;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
	float DamperC;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = -0.0f, ClampMax = 100.0f))
	float UpperStop;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = -100.0f, ClampMax = 0.0f))
	float LowerStop;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.00001f, ClampMax = 1.0f))
	float Regularizer;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 100000.0f))
	float BrakeTorque;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = -45.0f, ClampMax = -45.0f))
	float SteeringAngle;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 100000.0f))
	float HandBrakeTorque;
};
