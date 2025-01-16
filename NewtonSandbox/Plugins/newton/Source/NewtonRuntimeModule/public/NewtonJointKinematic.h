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
#include "NewtonJointKinematic.generated.h"

class ndWorld;
class UNewtonRigidBody;
class ANewtonWorldActor;
/**
 * 
 */
UCLASS(ClassGroup=(NewtonJoints), meta=(BlueprintSpawnableComponent))
class UNewtonJointKinematic : public UNewtonJoint
{
	GENERATED_BODY()
	
	public:
	// Sets default values for this component's properties
	UNewtonJointKinematic();

	UFUNCTION(BlueprintCallable, Category = Newton)
	void CreateAttachament(UNewtonRigidBody* const childBody, const FVector& location, float angularFriction, float linearFriction);

	UFUNCTION(BlueprintCallable, Category = Newton)
	void DestroyAttachament();

	UFUNCTION(BlueprintCallable, Category = Newton)
	void SetTargetPosit(const FVector& glocalSpacePosit);

	UFUNCTION(BlueprintCallable, Category = Newton)
	void SetTargetRotation(const FRotator& glocalSpaceRotation);

	UFUNCTION(BlueprintCallable, Category = Newton)
	void SetTargetMatrix(const FTransform& glocalSpaceTransform);

	UFUNCTION(BlueprintCallable, Category = Newton)
	FTransform GetTargetMatrix() const;

	protected:
	virtual void DrawGizmo(float timestep) const override;
	virtual ndJointBilateralConstraint* CreateJoint() override;
};
