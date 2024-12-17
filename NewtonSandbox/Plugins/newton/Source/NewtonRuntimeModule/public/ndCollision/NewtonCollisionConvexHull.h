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
#include "ndCollision/NewtonCollision.h"
#include "NewtonCollisionConvexHull.generated.h"

class ndHullOutput;
class ndShapeInstance;
class UNewtonRigidBody;

/**
 * 
 */
UCLASS(ClassGroup=(NewtonCollision), meta=(BlueprintSpawnableComponent))
class UNewtonCollisionConvexHull : public UNewtonCollision
{
	GENERATED_BODY()
	public:
	// Sets default values for this component's properties
	UNewtonCollisionConvexHull();

	virtual void InitStaticMeshCompoment(const USceneComponent* const meshComponent) override;
	
	protected:
	virtual void Serialize(FArchive& ar) override;

	virtual void ApplyPropertyChanges() override;
	virtual ndShape* CreateShape() const override;
	virtual long long CalculateHash() const override;
	virtual void SetTransform(const USceneComponent* const meshComponent) override;

	void GenerateMesh(const USceneComponent* const meshComponent);
	virtual ndShapeInstance* CreateInstanceShape() const override;
	virtual ndShapeInstance* CreateBodyInstanceShape(const ndMatrix& bodyMatrix) const override;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0001f))
	float Tolerance;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 16))
	int MaxVertexCount;

	TArray<FVector3f> m_convexHullPoints;
};
	

