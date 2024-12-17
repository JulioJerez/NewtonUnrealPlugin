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

#include "NewtonCommons.h"
#include "ndCollision/NewtonCollision.h"
#include "NewtonCollisionConvexApproximate.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(NewtonCollision), meta=(BlueprintSpawnableComponent))
class UNewtonCollisionConvexApproximate : public UNewtonCollision
{
	GENERATED_BODY()
	class ConvexVhacdGenerator;

	public:
	// Sets default values for this component's properties
	UNewtonCollisionConvexApproximate();
	virtual void InitStaticMeshCompoment(const USceneComponent* const meshComponent) override;
	
	protected:
	virtual void ApplyPropertyChanges() override;
	virtual ndShape* CreateShape() const override;
	virtual long long CalculateHash() const override;
	virtual ndShapeInstance* CreateInstanceShape() const override;
	virtual ndVector GetVolumePosition(const ndMatrix& bodyMatrix) const override;
	virtual ndShapeInstance* CreateBodyInstanceShape(const ndMatrix& bodyMatrix) const override;
	
	long long CalculateStaticMeshHash() const;
	const FStaticMeshLODResources* GetRenderLOD() const;
	ndConvexHullSet* CreateConvexApproximationShapes() const;

	virtual void Serialize(FArchive& ar) override;
	void SerializeLoadRevision_firstVersion(FArchive& ar);

	UPROPERTY(EditAnywhere, Category = Newton)
	bool Generate;

	UPROPERTY(EditAnywhere, Category = Newton)
	bool HighResolution;

	UPROPERTY(VisibleAnywhere, Category = Newton)
	int NumberOfConvex;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 1, ClampMax = 128))
	int MaxVertexPerConvex;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 1, ClampMax = 128))
	int MaxConvexes;

	UPROPERTY(EditAnywhere, Category = Newton, meta = (ClampMin = 0.0, ClampMax = 1.0))
	float Tolerance;
	
	long long m_meshHash;
	TSharedPtr<ndConvexHullSet> m_convexHullSet;
	bool m_generateFlipFlop;
};
