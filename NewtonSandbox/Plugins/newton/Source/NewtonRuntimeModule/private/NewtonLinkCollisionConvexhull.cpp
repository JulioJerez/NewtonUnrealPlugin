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
#include "Rendering/SkeletalMeshRenderData.h"

#include "NewtonCommons.h"
#include "NewtonLinkRigidBody.h"
#include "NewtonCollisionConvexHull.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"


UNewtonLinkCollisionConvexhull::UNewtonLinkCollisionConvexhull()
	:Super()
{
	SetName(TEXT("NewConvexhull"));
}

ndShapeInstance UNewtonLinkCollisionConvexhull::CreateInstance(TObjectPtr<USkeletalMesh> mesh, int boneIndex) const
{
	ndArray<ndVector> points;
	if (boneIndex > 0)
	{
		USkeletalMesh* const staticMesh = mesh.Get();
		const FSkeletalMeshRenderData* const resourceData = staticMesh->GetResourceForRendering();
		const FSkeletalMeshLODRenderData& renderResource = resourceData->LODRenderData[0];

		TArray<FSkinWeightInfo> weights;
		const FStaticMeshVertexBuffers& vertexBuffer = renderResource.StaticVertexBuffers;
		const FSkinWeightVertexBuffer& weightBuffer = renderResource.SkinWeightVertexBuffer;
		weightBuffer.GetSkinWeights(weights);

		const FMatrix refBoneMatrix(staticMesh->GetComposedRefPoseMatrix(boneIndex));
		ndMatrix scaleMatrix;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				scaleMatrix[i][j] = refBoneMatrix.M[i][j];
			}
		}

		ndVector scale;
		ndMatrix stretchAxis;
		ndMatrix transformMatrix;
		scaleMatrix.PolarDecomposition(transformMatrix, scale, stretchAxis);

		check(boneIndex > -1);
		// for some reason moronic unreal the bone exclude the root bone.
		boneIndex = boneIndex - 1;

		check(weights.Num() == vertexBuffer.PositionVertexBuffer.GetNumVertices());
		for (ndInt32 i = weights.Num() - 1; i >= 0; --i)
		{
			ndInt32 bone = -1;
			ndInt32 maxWieght = -1;
			const FSkinWeightInfo& info = weights[i];
			for (ndInt32 j = 0; j < MAX_TOTAL_INFLUENCES; ++j)
			{
				if (info.InfluenceWeights[j] > maxWieght)
				{
					bone = info.InfluenceBones[j];
					maxWieght = info.InfluenceWeights[j];
				}
			}
			if (bone == boneIndex)
			{
				const FVector3f skinPoint(vertexBuffer.PositionVertexBuffer.VertexPosition(i));
				const ndVector p(float(skinPoint.X), float(skinPoint.Y), float(skinPoint.Z), float(1.0f));
				points.PushBack(transformMatrix.UntransformVector(p).Scale(UNREAL_INV_UNIT_SYSTEM));
			}
		}
	}
	
	ndShapeInstance instance(new ndShapeConvexHull(ndInt32(points.GetCount()), sizeof(ndVector), 1.0e-3f, &points[0].m_x, 128));
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
	UNewtonCollisionConvexHull* const shape = Cast<UNewtonCollisionConvexHull>(component.Get());
	check(parentBody);
	if (parentBody && parentBody->BoneIndex > 0)
	{
		USkeletalMesh* const staticMesh = mesh.Get();
		const FSkeletalMeshRenderData* const resourceData = staticMesh->GetResourceForRendering();
		const FSkeletalMeshLODRenderData& renderResource = resourceData->LODRenderData[0];

		TArray<FSkinWeightInfo> weights;
		const FStaticMeshVertexBuffers& vertexBuffer = renderResource.StaticVertexBuffers;
		const FSkinWeightVertexBuffer& weightBuffer = renderResource.SkinWeightVertexBuffer;
		weightBuffer.GetSkinWeights(weights);

		const FMatrix refBoneMatrix(staticMesh->GetComposedRefPoseMatrix(parentBody->BoneIndex));
		ndMatrix scaleMatrix;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				scaleMatrix[i][j] = refBoneMatrix.M[i][j];
			}
		}

		ndVector scale;
		ndMatrix stretchAxis;
		ndMatrix transformMatrix;
		scaleMatrix.PolarDecomposition(transformMatrix, scale, stretchAxis);

		// for some reason moronic unreal the bone exclude the root bone.
		int boneIndex = parentBody->BoneIndex - 1;
		ndArray<ndBigVector> points;
		check(weights.Num() == vertexBuffer.PositionVertexBuffer.GetNumVertices());
		for (ndInt32 i = weights.Num() - 1; i >= 0; --i)
		{
			ndInt32 bone = -1;
			ndInt32 maxWieght = -1;
			const FSkinWeightInfo& info = weights[i];
			for (ndInt32 j = 0; j < MAX_TOTAL_INFLUENCES; ++j)
			{
				if (info.InfluenceWeights[j] > maxWieght)
				{
					bone = info.InfluenceBones[j];
					maxWieght = info.InfluenceWeights[j];
				}
			}
			if (bone == boneIndex)
			{
				const FVector3f skinPoint(vertexBuffer.PositionVertexBuffer.VertexPosition(i));
				const ndBigVector p(float(skinPoint.X), float(skinPoint.Y), float(skinPoint.Z), float(1.0f));
				points.PushBack(transformMatrix.UntransformVector(p).Scale(UNREAL_INV_UNIT_SYSTEM));
			}
		}

		ndConvexHull3d convexHull(&points[0].m_x, sizeof(ndBigVector), points.GetCount(), shape->Tolerance, shape->MaxVertexCount);
		const ndArray<ndBigVector>& convexVertex = convexHull.GetVertexPool();

		for (ndInt32 i = convexVertex.GetCount() - 1; i >= 0; --i)
		{
			//FVector3f p(float(convexVertex[i].m_x * UNREAL_UNIT_SYSTEM), float(convexVertex[i].m_y * UNREAL_UNIT_SYSTEM), float(convexVertex[i].m_z * UNREAL_UNIT_SYSTEM));
			FVector3f p(float(convexVertex[i].m_x), float(convexVertex[i].m_y), float(convexVertex[i].m_z));
			shape->m_convexHullPoints.Push(p);
		}
	}
}