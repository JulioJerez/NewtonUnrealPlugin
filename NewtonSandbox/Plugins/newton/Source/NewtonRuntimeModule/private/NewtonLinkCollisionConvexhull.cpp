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
	 
	#if 1
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
	#else
		const FSkeletalMeshRenderData* const resourceData = skinMesh->GetResourceForRendering();
		const FSkeletalMeshLODRenderData* const renderResource = &resourceData->LODRenderData[0];
		const FStaticMeshVertexBuffers* const vertexBuffer = &renderResource->StaticVertexBuffers;
		const FSkinWeightVertexBuffer* const weightBuffer = renderResource->GetSkinWeightVertexBuffer();
		for (ndInt32 vertexIndex = weightBuffer->GetNumVertices() - 1; vertexIndex >= 0 ; --vertexIndex)
		{
			uint32 vertexWeightOffset = 0;
			uint32 vertexInfluenceCount = 0;
			weightBuffer->GetVertexInfluenceOffsetCount(vertexIndex, vertexWeightOffset, vertexInfluenceCount);

			ndInt32 bone = -2;
			float maxWeight = -1;

			for (ndInt32 influenceIndex = vertexInfluenceCount - 1; influenceIndex >= 0; --influenceIndex)
			{
				float weight = weightBuffer->GetBoneWeight(vertexIndex, influenceIndex);
				if (weight > maxWeight)
				{
					maxWeight = weight;
					bone = weightBuffer->GetBoneIndex(vertexIndex, influenceIndex);
				}
			}
			if (boneIndex == bone)
			{
				const FVector skinVertex (vertexBuffer->PositionVertexBuffer.VertexPosition(vertexIndex));
				points.Push(skinVertex);
			}
		}
	#endif
}

ndShapeInstance UNewtonLinkCollisionConvexhull::CreateInstance(TObjectPtr<USkeletalMesh> mesh, int boneIndex) const
{
	ndArray<ndVector> points;
	if (boneIndex > 0)
	{
		TArray<FVector> bonePoints;
		GetBoneVertices(bonePoints, mesh, boneIndex);

		const FMatrix refBoneMatrix(mesh->GetComposedRefPoseMatrix(boneIndex));
		//ndMatrix scaleMatrix;
		//for (ndInt32 i = 0; i < 4; ++i)
		//{
		//	for (ndInt32 j = 0; j < 4; ++j)
		//	{
		//		scaleMatrix[i][j] = refBoneMatrix.M[i][j];
		//	}
		//}
		ndMatrix scaleMatrix(ToNewtonMatrix(refBoneMatrix));

		ndVector scale;
		ndMatrix stretchAxis;
		ndMatrix transformMatrix;
		scaleMatrix.PolarDecomposition(transformMatrix, scale, stretchAxis);

		for (ndInt32 i = bonePoints.Num() - 1; i >= 0; --i)
		{
			const ndVector p(float(bonePoints[i].X), float(bonePoints[i].Y), float(bonePoints[i].Z), float(1.0f));
			points.PushBack(transformMatrix.UntransformVector(p).Scale(UNREAL_INV_UNIT_SYSTEM));
		}
		ndShapeInstance instance(new ndShapeConvexHull(ndInt32(points.GetCount()), sizeof(ndVector), 1.0e-3f, &points[0].m_x, 128));
		return instance;
	}
	check(0);
	ndShapeInstance instance(new ndShapeNull());
	return instance;
}

TObjectPtr<USceneComponent> UNewtonLinkCollisionConvexhull::CreateBlueprintProxy() const
{
	TObjectPtr<UNewtonCollisionConvexHull> component(NewObject<UNewtonCollisionConvexHull>(UNewtonCollisionConvexHull::StaticClass(), Name, RF_Transient));

	return component;
}

void UNewtonLinkCollisionConvexhull::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonLinkRigidBody* const parentBody = Cast<UNewtonLinkRigidBody>(Parent);
	check(parentBody);
	if (parentBody && parentBody->BoneIndex > 0)
	{
		TArray<FVector> bonePoints;
		GetBoneVertices(bonePoints, mesh, parentBody->BoneIndex);
	
		const FMatrix refBoneMatrix(mesh->GetComposedRefPoseMatrix(parentBody->BoneIndex));
		//ndMatrix scaleMatrix;
		//for (ndInt32 i = 0; i < 4; ++i)
		//{
		//	for (ndInt32 j = 0; j < 4; ++j)
		//	{
		//		scaleMatrix[i][j] = refBoneMatrix.M[i][j];
		//	}
		//}
		ndMatrix scaleMatrix (ToNewtonMatrix(refBoneMatrix));
	
		ndVector scale;
		ndMatrix stretchAxis;
		ndMatrix transformMatrix;
		scaleMatrix.PolarDecomposition(transformMatrix, scale, stretchAxis);
	
		ndArray<ndBigVector> points;
		for (ndInt32 i = bonePoints.Num() - 1; i >= 0; --i)
		{
			const ndVector p(float(bonePoints[i].X), float(bonePoints[i].Y), float(bonePoints[i].Z), float(1.0f));
			points.PushBack(transformMatrix.UntransformVector(p).Scale(UNREAL_INV_UNIT_SYSTEM));
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