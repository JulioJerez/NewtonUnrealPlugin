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

#include "NewtonJointLoop.h"
#include "NewtonJointLoop6dofEffector.generated.h"

USTRUCT(BlueprintType)
struct FRobotTargetPosit
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Z;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Azimuth;
};


UCLASS(ClassGroup = NewtonJoints, meta = (BlueprintSpawnableComponent))
class UNewtonJointLoop6dofEffector : public UNewtonJointLoop
{
	GENERATED_BODY()
	
	public:
	// Sets default values for this component's properties
	UNewtonJointLoop6dofEffector();

	virtual void DrawGizmo(float timestep) const override;
	virtual ndJointBilateralConstraint* CreateJoint() override;

	UFUNCTION(BlueprintCallable, Category = Newton)
	FRobotTargetPosit WorldLocationToEffectorSpace(const FVector& worldPosition);

	UFUNCTION(BlueprintCallable, Category = Newton)
	FTransform GetTargetTransform() const;

	UFUNCTION(BlueprintCallable, Category = Newton)
	void SetTargetTransform(const FTransform& transform);

	UFUNCTION(BlueprintCallable, Category = Newton)
	FVector ClipRobotTarget();

	UFUNCTION(BlueprintCallable, Category = Newton)
	void SetRobotTarget(float x, float z, float azimuth, float pitch, float yaw, float roll);

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 10000.0f))
	float AngularSpring;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 500.0f))
	float AngularDamper;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = -10.0f))
	float AngularMaxTorque;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.00001f, ClampMax = 0.99f))
	float AngularRegularizer;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 20000.0f))
	float LinearSpring;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0f, ClampMax = 20000.0f))
	float LinearDamper;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 10.0f))
	float LinearMaxForce;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.00001f, ClampMax = 0.99f))
	float LinearRegularizer;

	FMatrix m_referenceFrame;
	FRobotTargetPosit m_target;
	float m_targetYaw;
	float m_targetRoll;
	float m_targetPitch;
};
