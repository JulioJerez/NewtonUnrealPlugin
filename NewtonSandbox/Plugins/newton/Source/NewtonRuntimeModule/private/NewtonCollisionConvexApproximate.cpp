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


#include "NewtonCollisionConvexApproximate.h"

#include "NewtonRigidBody.h"
#include "NewtonSceneActor.h"
#include "NewtonRuntimeModule.h"
#include "NewtonCollisionConvexHull.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"
#include "ThirdParty/newtonLibrary/Public/thirdParty/ndConvexApproximation.h"

//#define SHOW_VHACD_PROGRESS_BAR

class UNewtonCollisionConvexApproximate::ConvexVhacdGenerator : public ndConvexApproximation
{
	public:
	ConvexVhacdGenerator(ndInt32 maxConvexes, bool quality)
		:ndConvexApproximation(maxConvexes, quality)
		,m_progressBar(nullptr)
		,m_acc(0.0f)
	{
		#ifdef SHOW_VHACD_PROGRESS_BAR  
		// for some reason the progress bar invalidate some UClasses
		// I need to report this some day to unreal.
		// for now just do not report progress.
		m_progressBar = new FScopedSlowTask(100.0f, NSLOCTEXT("Newton", "Newton", "Generation Convex Approximation"));
		m_progressBar->MakeDialog();
		#endif
	}

	~ConvexVhacdGenerator()
	{
		if (m_progressBar)
		{
			delete m_progressBar;
			m_progressBar = nullptr;
		}
	}

	virtual void ShowProgress() override
	{
		if (m_progressBar)
		{
			m_acc += 1.0f;
			if (m_acc < 99.0f)
			{
				m_progressBar->EnterProgressFrame();
			}
		}
	}

	FScopedSlowTask* m_progressBar;
	float m_acc;
};

UNewtonCollisionConvexApproximate::UNewtonCollisionConvexApproximate()
	:Super()
{
	Generate = false;
	MaxConvexes = 16;
	Tolerance = 0.0f;
	NumberOfConvex = 0;
	HighResolution = false;
	MaxVertexPerConvex = 32;
	m_generateFlipFlop = false;
}

const FStaticMeshLODResources* UNewtonCollisionConvexApproximate::GetRenderLOD() const
{
	const UStaticMeshComponent* const staticComponent = Cast<UStaticMeshComponent>(GetAttachParent());
	if (staticComponent)
	{
		const UStaticMesh* const staticMesh = staticComponent->GetStaticMesh().Get();
		if (staticMesh)
		{
			const FStaticMeshRenderData* const renderData = staticMesh->GetRenderData();
			if (renderData)
			{
				return &renderData->LODResources[0];
			}
		}
	}
	return nullptr;
}

//long long UNewtonCollisionConvexApproximate::CalculateStaticMeshHash() const
//{
//	long long hash = ndCRC64(ndShapeCompound::StaticClassName(), strlen(ndShapeConvexHull::StaticClassName()), 0);
//	const FStaticMeshLODResources* const renderLOD = GetRenderLOD();
//	if (renderLOD)
//	{
//		const FVector uScale(GetComponentTransform().GetScale3D());
//		const ndVector scale(ndFloat32(uScale.X), ndFloat32(uScale.Y), ndFloat32(uScale.Z), ndFloat32(0.0f));
//		const ndVector bakedScale(scale.Scale(UNREAL_INV_UNIT_SYSTEM));
//
//		const FStaticMeshVertexBuffers& staticMeshVertexBuffer = renderLOD->VertexBuffers;;
//		const FPositionVertexBuffer& positBuffer = staticMeshVertexBuffer.PositionVertexBuffer;
//		for (ndInt32 i = 0; i < ndInt32(positBuffer.GetNumVertices()); ++i)
//		{
//			const FVector3f p(positBuffer.VertexPosition(i));
//			FVector3f q;
//			q.X = ndReal(p.X * bakedScale.m_x);
//			q.Y = ndReal(p.Y * bakedScale.m_y);
//			q.Z = ndReal(p.Z * bakedScale.m_z);
//			hash = ndCRC64(&q, sizeof(FVector3f), hash);
//		}
//		const FRawStaticIndexBuffer& indexBuffer = renderLOD->IndexBuffer;
//		for (ndInt32 i = 0; i < ndInt32(indexBuffer.GetNumIndices()); i += 3)
//		{
//			ndInt32 j = indexBuffer.GetIndex(i);
//			hash = ndCRC64(&j, sizeof(ndInt32), hash);
//		}
//		hash = ndCRC64(&HighResolution, sizeof(bool), hash);
//		hash = ndCRC64(&MaxVertexPerConvex, sizeof(int), hash);
//		hash = ndCRC64(&MaxConvexes, sizeof(int), hash);
//		hash = ndCRC64(&Tolerance, sizeof(float), hash);
//	}
//	return hash;
//}

ndConvexHullSet* UNewtonCollisionConvexApproximate::CreateConvexApproximationShapes() const
{
	const UStaticMeshComponent* const staticComponent = Cast<UStaticMeshComponent>(GetAttachParent());
	if (!staticComponent)
	{
		check(0);
		return nullptr;
	}

	ConvexVhacdGenerator* const vhacdHullSet = new ConvexVhacdGenerator(MaxConvexes, HighResolution);
	vhacdHullSet->m_tolerance = 1.0e-3f + Tolerance * 0.1f;
	vhacdHullSet->m_maxPointPerHull = MaxVertexPerConvex;

	const FStaticMeshLODResources* const renderLOD = GetRenderLOD();
	check(renderLOD);

	const FVector uScale(GetComponentTransform().GetScale3D());
	const ndVector scale(ndFloat32(uScale.X), ndFloat32(uScale.Y), ndFloat32(uScale.Z), ndFloat32(0.0f));
	const ndVector bakedScale(scale.Scale(UNREAL_INV_UNIT_SYSTEM));

	const FStaticMeshVertexBuffers& staticMeshVertexBuffer = renderLOD->VertexBuffers;
	const FPositionVertexBuffer& positBuffer = staticMeshVertexBuffer.PositionVertexBuffer;
	
	ndHullInputMesh& inputMesh = vhacdHullSet->m_inputMesh;
	for (ndInt32 i = 0; i < ndInt32(positBuffer.GetNumVertices()); ++i)
	{
		ndHullPoint q;
		const FVector3f p(positBuffer.VertexPosition(i));
	
		q.m_x = ndReal(p.X * bakedScale.m_x);
		q.m_y = ndReal(p.Y * bakedScale.m_y);
		q.m_z = ndReal(p.Z * bakedScale.m_z);
		inputMesh.m_points.PushBack(q);
	}
	
	const FRawStaticIndexBuffer& indexBuffer = renderLOD->IndexBuffer;
	for (ndInt32 i = 0; i < ndInt32(indexBuffer.GetNumIndices()); i += 3)
	{
		ndHullInputMesh::ndFace face;
		face.m_i0 = indexBuffer.GetIndex(i + 0);
		face.m_i1 = indexBuffer.GetIndex(i + 1);
		face.m_i2 = indexBuffer.GetIndex(i + 2);
		check(face.m_i0 != face.m_i1);
		check(face.m_i0 != face.m_i2);
		check(face.m_i1 != face.m_i2);
		inputMesh.m_faces.PushBack(face);
	}
	
	vhacdHullSet->Execute();

	ndConvexHullSet* const hullSet = new ndConvexHullSet;
	ndArray<ndHullOutput*>& hullArray = vhacdHullSet->m_ouputHulls;
	for (ndInt32 i = hullArray.GetCount() - 1; i >= 0; --i)
	{
		ndHullPoints* const pointsSet = new ndHullPoints;
		const ndHullOutput* const convexHull = hullArray[i];
		
		ndHullPoints& dstPoints = *pointsSet;
		const ndArray<ndHullPoint>& srcPoints = *convexHull;
		for (ndInt32 j = ndInt32(srcPoints.GetCount()) - 1; j >= 0; --j)
		{
			FVector3f p(srcPoints[j].m_x, srcPoints[j].m_y, srcPoints[j].m_z);
			dstPoints.Push(p);
		}
		hullSet->Push(pointsSet);
	}

	UE_LOG(LogTemp, Display, TEXT("number of vhcd convex generated: %d"), hullArray.GetCount());
	delete vhacdHullSet;
	return hullSet;
}

long long UNewtonCollisionConvexApproximate::CalculateHash() const
{
	long long hash = ndCRC64(ndShapeCompound::StaticClassName(), strlen(ndShapeConvexHull::StaticClassName()), 0);
	for (ndInt32 i = ShapeHulls.Num()-1; i >= 0; --i)
	{
		const FVector* const vexterBuffer = &ShapeHulls[i].Points[0];
		hash = ndCRC64(vexterBuffer, ShapeHulls[i].Points.Num() * sizeof(FVector), hash);
	}
	return hash;
}

ndShape* UNewtonCollisionConvexApproximate::CreateShape() const
{
	if (ShapeHulls.Num())
	{
		ndShapeCompound* const compound = new ndShapeCompound();
		compound->BeginAddRemove();
		for (ndInt32 j = ShapeHulls.Num() - 1; j >= 0; --j)
		{
			ndArray<ndVector> points;
			const FndCachedHullPoints& hull = ShapeHulls[j];
			for (ndInt32 i = hull.Points.Num() - 1; i >= 0; --i)
			{
				const ndVector p(ndFloat32(hull.Points[i].X), ndFloat32(hull.Points[i].Y), ndFloat32(hull.Points[i].Z), ndFloat32(0.0f));
				points.PushBack(p);
			}
			ndShape* const shape = new ndShapeConvexHull(hull.Points.Num(), sizeof(ndVector), Tolerance, &points[0].m_x);
			ndShapeInstance* const subInstance = new ndShapeInstance(shape);
			compound->AddCollision(subInstance);
			delete subInstance;
		}
		compound->EndAddRemove();
		return compound;
	}
	return new ndShapeNull();
}

void UNewtonCollisionConvexApproximate::InitStaticMeshCompoment(const USceneComponent* const meshComponent)
{
	check(0);
}

ndShapeInstance* UNewtonCollisionConvexApproximate::CreateBodyInstanceShape(const ndMatrix& bodyMatrix) const
{
	ndShapeInstance* const instance = CreateInstanceShape();

	const ndVector scale(ndFloat32(1.0f));
	const FTransform transform(GetComponentToWorld());
	const ndMatrix matrix(ToNewtonMatrix(transform) * bodyMatrix.OrthoInverse());

	instance->SetScale(scale);
	instance->SetLocalMatrix(matrix);
	return instance;
}

ndShapeInstance* UNewtonCollisionConvexApproximate::CreateInstanceShape() const
{
	ndShapeInstance* const instance = new ndShapeInstance(m_shape);
	const FVector uScale(GetComponentTransform().GetScale3D());
	const ndVector scale(ndFloat32(1.0f / uScale.X), ndFloat32(1.0f / uScale.Y), ndFloat32(1.0f / uScale.Z), ndFloat32(0.0f));
	instance->SetScale(scale);
	return instance;
}

ndVector UNewtonCollisionConvexApproximate::GetVolumePosition(const ndMatrix& bodyMatrix) const
{
	ndVector posit(0.0f);
	ndShapeInstance* const instance = CreateInstanceShape();
	if (instance)
	{
		instance->SetScale(ndVector(1.0f));

		const ndMatrix inertia(instance->CalculateInertia());
		posit = inertia.m_posit;

		posit.m_w = instance->GetVolume();
		delete instance;
	}
	return posit;
}

void UNewtonCollisionConvexApproximate::ApplyPropertyChanges()
{
	if (Generate && !m_generateFlipFlop)
	{
		check(0);
		ShapeHulls.SetNum(0);
	}

	if (!ShapeHulls.Num())
	{
		TSharedPtr<ndConvexHullSet> convexApproximantion(CreateConvexApproximationShapes());
		for (int j = convexApproximantion->Num() - 1; j >= 0; --j)
		{
			const ndHullPoints& hull = *(*convexApproximantion)[j];

			FndCachedHullPoints chachedHull;
			for (int i = hull.Num() - 1; i >= 0; --i)
			{
				const FVector p(hull[i].X, hull[i].Y, hull[i].Z);
				chachedHull.Points.Push(p);
			}
			ShapeHulls.Push(chachedHull);
		}
		m_debugVisualIsDirty = true;

		NumberOfConvex = ShapeHulls.Num();
	}

	MarkRenderDynamicDataDirty();
	NotifyMeshUpdated();

	m_generateFlipFlop = Generate;
	Generate = false;

	BuildNewtonShape();
	Super::ApplyPropertyChanges();
}