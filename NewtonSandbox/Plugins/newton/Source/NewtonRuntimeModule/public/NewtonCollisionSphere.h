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
#include "NewtonCollision.h"
#include "NewtonCollisionSphere.generated.h"

class ndShapeInstance;
class UNewtonRigidBody;

/**
 * 
 */
UCLASS(ClassGroup = NewtonCollision, meta = (BlueprintSpawnableComponent))
class UNewtonCollisionSphere : public UNewtonCollision
{
	GENERATED_BODY()
	
	public:
	// Sets default values for this component's properties
	UNewtonCollisionSphere();
	virtual void InitStaticMeshCompoment(const USceneComponent* const meshComponent) override;

	virtual void ApplyPropertyChanges();
	virtual ndShape* CreateShape() const;
	virtual long long CalculateHash() const;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 1.0f))
	float Radio;
};
