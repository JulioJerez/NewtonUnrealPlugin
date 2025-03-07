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

#include "NewtonLinkCollisionConvexApproximate.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "Rendering/SkeletalMeshRenderData.h"

#include "NewtonCommons.h"
#include "NewtonLinkRigidBody.h"
#include "NewtonCollisionConvexHull.h"
#include "NewtonCollisionConvexApproximate.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"
#include "ThirdParty/newtonLibrary/Public/thirdParty/ndConvexApproximation.h"

UNewtonLinkCollisionConvexApproximate::UNewtonLinkCollisionConvexApproximate()
	:Super()
{
	//ShapeHullsHash = 0;
	Name = TEXT("ConvexApproximate");
}

void UNewtonLinkCollisionConvexApproximate::GetBoneMesh(ndHullInputMesh& boneMesh, TObjectPtr<USkeletalMesh> mesh, int boneIndex) const
{
	USkeletalMesh* const skinMesh = mesh.Get();

	check(boneIndex > -1);

	const FSkeletalMeshModel* const importedResource = mesh->GetImportedModel();
	const FSkeletalMeshLODModel* const model = &importedResource->LODModels[0];
	const TArray<uint32>& indexBuffer = model->IndexBuffer;

	ndTree<ndInt32, ndInt32> vertexFilter;
	for (int32 index = model->Sections.Num() - 1; index >= 0; --index)
	{
		ndArray<ndInt32> vertexIndex;
		const FSkelMeshSection& section = model->Sections[index];
		for (int32 i = section.SoftVertices.Num() - 1; i >= 0; --i)
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
				ndHullPoint skinVertex;
				skinVertex.m_x = softVert->Position.X;
				skinVertex.m_y = softVert->Position.Y;
				skinVertex.m_z = softVert->Position.Z;
				vertexFilter.Insert(boneMesh.m_points.GetCount(), i + section.BaseVertexIndex);
				boneMesh.m_points.PushBack(skinVertex);
			}
		}

		for (int32 i = section.NumTriangles-1; i>= 0; --i)
		{
			int32 j0 = indexBuffer[section.BaseIndex + i * 3 + 0];
			int32 j1 = indexBuffer[section.BaseIndex + i * 3 + 1];
			int32 j2 = indexBuffer[section.BaseIndex + i * 3 + 2];
			const ndTree<ndInt32, ndInt32>::ndNode* const node0 = vertexFilter.Find(j0);
			const ndTree<ndInt32, ndInt32>::ndNode* const node1 = vertexFilter.Find(j1);
			const ndTree<ndInt32, ndInt32>::ndNode* const node2 = vertexFilter.Find(j2);
			if (node0 && node1 && node2)
			{
				ndHullInputMesh::ndFace face;
				face.m_i0 = node0->GetInfo();
				face.m_i1 = node1->GetInfo();
				face.m_i2 = node2->GetInfo();
				boneMesh.m_faces.PushBack(face);
			}
		}
	}
}

ndShapeInstance UNewtonLinkCollisionConvexApproximate::CreateInstance(TObjectPtr<USkeletalMesh> mesh, int boneIndex) const
{
	check(0);
	if (boneIndex > 0)
	{
		TSharedPtr<ndConvexApproximation> boneMeshHullApproximate(new ndConvexApproximation(64, true));
		boneMeshHullApproximate->m_maxPointPerHull = 64;
		
		GetBoneMesh(boneMeshHullApproximate->m_inputMesh, mesh, boneIndex);
		if (boneMeshHullApproximate->m_inputMesh.m_faces.GetCount() > 0)
		{
			//long long newHash = ndCRC64(ndShapeCompound::StaticClassName(), strlen(ndShapeCompound::StaticClassName()), 0);
			//newHash = ndCRC64(&boneMeshHullApproximate->m_inputMesh.m_points[0].m_x, boneMeshHullApproximate->m_inputMesh.m_points.GetCount() * sizeof(ndHullPoint), newHash);
			//newHash = ndCRC64(&boneMeshHullApproximate->m_inputMesh.m_faces[0].m_i0, boneMeshHullApproximate->m_inputMesh.m_faces.GetCount() * sizeof(ndHullInputMesh::ndFace), newHash);
			//if (newHash != ShapeHullsHash)
			{
				//ShapeHullsHash = uint64(newHash);
				boneMeshHullApproximate->Execute();
		
				//ShapeHulls.SetNum(0);
				//ndArray<ndHullOutput*>& hullArray = boneMeshHullApproximate->m_ouputHulls;
				//for (ndInt32 i = ndInt32(hullArray.GetCount() - 1); i >= 0; --i)
				//{
				//	FndCachedHullPoints hullPoints;
				//	const ndHullOutput& hull = *hullArray[i];
				//	for (ndInt32 j = ndInt32(hull.GetCount() - 1); j >= 0; --j)
				//	{
				//		const FVector p(float(hull[j].m_x), float(hull[j].m_y), float(hull[j].m_z));
				//		hullPoints.Points.Push(p);
				//	}
				//	ShapeHulls.Push(hullPoints);
				//}
			}
		
			const FMatrix refBoneMatrix(mesh->GetComposedRefPoseMatrix(boneIndex));
			ndMatrix scaleMatrix(ToNewtonMatrix(refBoneMatrix));
		
			ndVector scale;
			ndMatrix stretchAxis;
			ndMatrix transformMatrix;
			scaleMatrix.PolarDecomposition(transformMatrix, scale, stretchAxis);
		
			ndArray<ndHullOutput*>& hullArray = boneMeshHullApproximate->m_ouputHulls;

			ShapeHulls.SetNum(0);
			ndShapeCompound* const compound = new ndShapeCompound();
			compound->BeginAddRemove();
			for (ndInt32 i = ShapeHulls.Num() - 1; i >= 0; --i)
			{
				ndArray<ndVector> points;
				FndCachedHullPoints hullPoints;
				const ndHullOutput& hull = *hullArray[i];
				for (ndInt32 j = ndInt32 (hull.GetCount() - 1); j >= 0; --j)
				{
					//const ndVector p(float(hull[j].X), float(hull[j].Y), float(hull[j].Z), float(1.0f));
					const ndVector p(float(hull[i].m_x * UNREAL_INV_UNIT_SYSTEM), float(hull[i].m_y * UNREAL_INV_UNIT_SYSTEM), float(hull[i].m_z * UNREAL_INV_UNIT_SYSTEM), float(1.0f));
					const ndVector p1(transformMatrix.UntransformVector(p));
					//points.PushBack(transformMatrix.UntransformVector(p).Scale(UNREAL_INV_UNIT_SYSTEM));
					points.PushBack(p1);
					const FVector cachePoint(float(p1.m_x), float(p1.m_y), float(p1.m_z));
					hullPoints.Points.Push(cachePoint);
				}
				ShapeHulls.Push(hullPoints);
				ndShape* const shape = new ndShapeConvexHull(ndInt32(points.GetCount()), ndInt32(sizeof(ndVector)), boneMeshHullApproximate->m_tolerance, &points[0].m_x);
				ndShapeInstance* const subInstance = new ndShapeInstance(shape);
				compound->AddCollision(subInstance);
				delete subInstance;
			}
			compound->EndAddRemove();
		
			ndShapeInstance instance(compound);
			return instance;
		}
	}
	
	ndShapeInstance instance(new ndShapeNull());
	return instance;
}

ndShapeInstance UNewtonLinkCollisionConvexApproximate::CreateInstance(TObjectPtr<UStaticMesh> mesh) const
{
	FStaticMeshRenderData* const renderData = mesh->GetRenderData();
	check(renderData);
	FStaticMeshLODResources& modelLod = renderData->LODResources[0];
	FRawStaticIndexBuffer* const indexBuffer = &modelLod.IndexBuffer;

	const FStaticMeshVertexBuffers& staticMeshVertexBuffer = modelLod.VertexBuffers;
	const FPositionVertexBuffer& positBuffer = staticMeshVertexBuffer.PositionVertexBuffer;

	TSharedPtr<ndConvexApproximation> boneMeshHullApproximate(new ndConvexApproximation(64, true));
	boneMeshHullApproximate->m_maxPointPerHull = 64;

	ndHullInputMesh& boneMesh = boneMeshHullApproximate->m_inputMesh;
	for (int32 index = positBuffer.GetNumVertices() - 1; index >= 0; --index)
	{
		ndHullPoint hullPoint;
		const FVector p(positBuffer.VertexPosition(index));
		hullPoint.m_x = ndReal(p.X * UNREAL_INV_UNIT_SYSTEM);
		hullPoint.m_y = ndReal(p.Y * UNREAL_INV_UNIT_SYSTEM);
		hullPoint.m_z = ndReal(p.Z * UNREAL_INV_UNIT_SYSTEM);
		boneMesh.m_points.PushBack(hullPoint);
	}

	const FStaticMeshSectionArray& sections = modelLod.Sections;
	for (int32 index = sections.Num() - 1; index >= 0; --index)
	{
		const FStaticMeshSection& section = modelLod.Sections[index];
		for (ndInt32 i = 0; i < ndInt32(section.NumTriangles); ++i)
		{
			ndHullInputMesh::ndFace face;
			face.m_i0 = indexBuffer->GetIndex(section.FirstIndex + i * 3 + 0);
			face.m_i1 = indexBuffer->GetIndex(section.FirstIndex + i * 3 + 1);
			face.m_i2 = indexBuffer->GetIndex(section.FirstIndex + i * 3 + 2);
			boneMesh.m_faces.PushBack(face);
		}
	}

	boneMeshHullApproximate->Execute();
	ndArray<ndHullOutput*>& hullArray = boneMeshHullApproximate->m_ouputHulls;

	ShapeHulls.SetNum(0);
	ndShapeCompound* const compound = new ndShapeCompound();
	compound->BeginAddRemove();
	for (ndInt32 i = ndInt32 (hullArray.GetCount() - 1); i >= 0; --i)
	{
		ndArray<ndVector> points;
		FndCachedHullPoints hullPoints;
		const ndHullOutput& hull = *hullArray[i];
		for (ndInt32 j = ndInt32 (hull.GetCount() - 1); j >= 0; --j)
		{
			FVector q(float(hull[j].m_x), float(hull[j].m_y), float(hull[j].m_z));
			hullPoints.Points.Push(q);
			const ndVector p(float(q.X), float(q.Y), float(q.Z), float(1.0f));
			points.PushBack(p);
		}
		ShapeHulls.Push(hullPoints);
		ndShape* const shape = new ndShapeConvexHull(ndInt32(points.GetCount()), ndInt32(sizeof(ndVector)), boneMeshHullApproximate->m_tolerance, &points[0].m_x);
		ndShapeInstance* const subInstance = new ndShapeInstance(shape);
		compound->AddCollision(subInstance);
		delete subInstance;
	}
	compound->EndAddRemove();
	ndShapeInstance instance(compound);
	return instance;
}

TObjectPtr<USceneComponent> UNewtonLinkCollisionConvexApproximate::CreateBlueprintProxy() const
{
	TObjectPtr<USceneComponent> component(NewObject<UNewtonCollisionConvexApproximate>(UNewtonCollisionConvexApproximate::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkCollisionConvexApproximate::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	UNewtonLinkRigidBody* const parentBody = Cast<UNewtonLinkRigidBody>(Parent);
	check(parentBody);
	//if (parentBody && parentBody->BoneIndex > 0)
	if (parentBody)
	{
		check(ShapeHulls.Num());
		//const FMatrix refBoneMatrix(mesh->GetComposedRefPoseMatrix(parentBody->BoneIndex));
		//ndMatrix scaleMatrix(ToNewtonMatrix(refBoneMatrix));
		//
		//ndVector scale;
		//ndMatrix stretchAxis;
		//ndMatrix transformMatrix;
		//scaleMatrix.PolarDecomposition(transformMatrix, scale, stretchAxis);
		
		UNewtonCollisionConvexApproximate* const shape = Cast<UNewtonCollisionConvexApproximate>(component.Get());
		shape->ShapeHulls.SetNum(0);
		shape->NumberOfConvex = ShapeHulls.Num();

		for (ndInt32 j = 0; j < ShapeHulls.Num(); ++j)
		{
			FndCachedHullPoints dstHull;
			const FndCachedHullPoints& srcHull = ShapeHulls[j];
			for (ndInt32 i = 0; i < srcHull.Points.Num(); ++i)
			{
				const ndVector p(float(srcHull.Points[i].X), float(srcHull.Points[i].Y), float(srcHull.Points[i].Z), float(1.0f));
				//const ndVector q(transformMatrix.UntransformVector(p).Scale(UNREAL_INV_UNIT_SYSTEM));
				//dstHull.Points.Push(FVector(q.m_x, q.m_y, q.m_z));
				dstHull.Points.Push(FVector(p.m_x, p.m_y, p.m_z));
			}
			shape->ShapeHulls.Push(dstHull);
		}
	}
}