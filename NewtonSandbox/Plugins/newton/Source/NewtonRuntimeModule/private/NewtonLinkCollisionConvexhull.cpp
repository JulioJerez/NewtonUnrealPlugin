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

#include "NewtonLinkCollisionConvexhull.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "Rendering/SkeletalMeshRenderData.h"

#include "NewtonCommons.h"
#include "NewtonLinkRigidBody.h"
#include "NewtonCollisionConvexHull.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkCollisionConvexhull::UNewtonLinkCollisionConvexhull()
	:Super()
{
	Name = TEXT("convexhull");
}

void UNewtonLinkCollisionConvexhull::GetBoneVertices(TArray<FVector>& points, TObjectPtr<USkeletalMesh> mesh, int boneIndex) const
{
	USkeletalMesh* const skinMesh = mesh.Get();

	check(boneIndex > -1);

	// this is ridiculous, moronic unreal does not make a simple lookup table for bone indices,  
	// instead, it makes a stupid section and a double indirection. 
	// unreal graphics are awesome, but the software is quite inefficient and mediocre, I am not impressed. 	
	const FSkeletalMeshModel* const importedResource = mesh->GetImportedModel();
	const FSkeletalMeshLODModel* const model = &importedResource->LODModels[0];
	for (int32 index = model->Sections.Num() - 1; index >= 0; --index)
	{
		const FSkelMeshSection& section = model->Sections[index];
		for (int32 i = section.SoftVertices.Num() - 1; i >= 0 ; --i)
		{
			const FSoftSkinVertex* const softVert = &section.SoftVertices[i];

			ndInt32 bone = -1;
			ndInt32 maxWeight = 0;
			for (int32 j = 0; j < MAX_TOTAL_INFLUENCES; ++j)
			{
				if (softVert->InfluenceWeights[j] > maxWeight)
				{
					bone = softVert->InfluenceBones[j];
					maxWeight = softVert->InfluenceWeights[j];
				}
			}
			check(bone >= 0);
			if (section.BoneMap[bone] == boneIndex)
			{
				const FVector skinVertex(softVert->Position);
				points.Push(skinVertex);
			}
		}
	}
}

ndShapeInstance UNewtonLinkCollisionConvexhull::CreateInstance(TObjectPtr<USkeletalMesh> mesh, int boneIndex) const
{
	if (boneIndex > 0)
	{
		if (!m_hull.Num())
		{
			TArray<FVector> bonePoints;
			GetBoneVertices(bonePoints, mesh, boneIndex);

			const FMatrix refBoneMatrix(mesh->GetComposedRefPoseMatrix(boneIndex));
			ndMatrix scaleMatrix(ToNewtonMatrix(refBoneMatrix));

			ndVector scale;
			ndMatrix stretchAxis;
			ndMatrix transformMatrix;
			scaleMatrix.PolarDecomposition(transformMatrix, scale, stretchAxis);

			for (ndInt32 i = bonePoints.Num() - 1; i >= 0; --i)
			{
				const FVector meshPosint(bonePoints[i]);
				const ndVector p(float(meshPosint.X * UNREAL_INV_UNIT_SYSTEM), float(meshPosint.Y * UNREAL_INV_UNIT_SYSTEM), float(meshPosint.Z * UNREAL_INV_UNIT_SYSTEM), float(1.0f));
				const ndVector p1(transformMatrix.UntransformVector(p));
				const FVector cachePoint(float(p1.m_x), float(p1.m_y), float(p1.m_z));
				m_hull.Push(cachePoint);
			}
		}

		ndArray<ndVector> points;
		for (ndInt32 i = m_hull.Num() - 1; i >= 0; --i)
		{
			const FVector p(m_hull[i]);
			points.PushBack (ndVector (float(p.X), float(p.Y), float(p.Z), float(1.0f)));
		}
		ndShapeInstance instance(new ndShapeConvexHull(ndInt32(points.GetCount()), sizeof(ndVector), 1.0e-3f, &points[0].m_x, 128));
		return instance;
	}
	check(0);
	ndShapeInstance instance(new ndShapeNull());
	return instance;
}

ndShapeInstance UNewtonLinkCollisionConvexhull::CreateInstance(TObjectPtr<UStaticMesh> mesh) const
{
	ndArray<ndVector> points;
	FStaticMeshRenderData* const renderData = mesh->GetRenderData();
	check(renderData);
	FStaticMeshLODResources& modelLod = renderData->LODResources[0];
	FRawStaticIndexBuffer* const indexBuffer = &modelLod.IndexBuffer;

	const FStaticMeshVertexBuffers& staticMeshVertexBuffer = modelLod.VertexBuffers;
	const FPositionVertexBuffer& positBuffer = staticMeshVertexBuffer.PositionVertexBuffer;

	m_hull.Empty();
	const FStaticMeshSectionArray& sections = modelLod.Sections;
	for (int32 index = sections.Num() - 1; index >= 0; --index)
	{
		const FStaticMeshSection& section = modelLod.Sections[index];
		for (ndInt32 i = 0; i < ndInt32(section.NumTriangles); ++i)
		{
			for (ndInt32 j = 0; j < 3; ++j)
			{
				ndInt32 k = indexBuffer->GetIndex(section.FirstIndex + i * 3 + j);
				const FVector p(positBuffer.VertexPosition(k) * UNREAL_INV_UNIT_SYSTEM);
				const ndVector q(float(p.X), float(p.Y), float(p.Z), float(1.0f));
				m_hull.Push(p);
				points.PushBack(q);
			}
		}
	}
	ndShapeInstance instance(new ndShapeConvexHull(ndInt32(points.GetCount()), sizeof(ndVector), 1.0e-3f, &points[0].m_x, 128));
	return instance;
}

ndShapeInstance UNewtonLinkCollisionConvexhull::CreateInstance(const TArray<FVector>& hull) const
{
	ndArray<ndVector> points;
	for (int32 i = hull.Num() - 1; i >= 0; --i)
	{
		ndVector p(ndFloat32 (hull[i].X), ndFloat32(hull[i].Y), ndFloat32(hull[i].Z), ndFloat32(1.0f));
		points.PushBack(p);
	}

	ndShapeInstance instance(new ndShapeConvexHull(ndInt32(points.GetCount()), sizeof(ndVector), 1.0e-3f, &points[0].m_x, 128));
	return instance;
}

TObjectPtr<USceneComponent> UNewtonLinkCollisionConvexhull::CreateBlueprintProxy() const
{
	TObjectPtr<USceneComponent> component(NewObject<UNewtonCollisionConvexHull>(UNewtonCollisionConvexHull::StaticClass(), Name, RF_Transient));

	return component;
}

void UNewtonLinkCollisionConvexhull::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonLinkRigidBody* const parentBody = Cast<UNewtonLinkRigidBody>(Parent);
	check(parentBody);
	if (parentBody)
	{
		ndArray<ndBigVector> points;
		for (ndInt32 i = m_hull.Num() - 1; i >= 0; --i)
		{
			const ndVector p(float(m_hull[i].X), float(m_hull[i].Y), float(m_hull[i].Z), float(1.0f));
			points.PushBack(p);
		}

		UNewtonCollisionConvexHull* const shape = Cast<UNewtonCollisionConvexHull>(component.Get());
		ndConvexHull3d convexHull(&points[0].m_x, sizeof(ndBigVector), points.GetCount(), shape->Tolerance, shape->MaxVertexCount);
		const ndArray<ndBigVector>& convexVertex = convexHull.GetVertexPool();

		for (ndInt32 i = convexVertex.GetCount() - 1; i >= 0; --i)
		{
			FVector p(float(convexVertex[i].m_x), float(convexVertex[i].m_y), float(convexVertex[i].m_z));
			shape->ShapeHull.Points.Push(p);
		}
	}
}