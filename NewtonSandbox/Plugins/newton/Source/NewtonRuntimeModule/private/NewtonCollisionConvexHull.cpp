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


#include "NewtonCollisionConvexHull.h"

#include "NewtonSceneActor.h"
#include "NewtonRuntimeModule.h"
#include "NewtonRigidBody.h"
#include "NewtonSceneRigidBody.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonCollisionConvexHull::UNewtonCollisionConvexHull()
	:Super()
	,Tolerance(1.0e-4f)
	,MaxVertexCount(128)
{
}

long long UNewtonCollisionConvexHull::CalculateHash() const
{
	long long hash = ndCRC64(ndShapeConvexHull::StaticClassName(), strlen(ndShapeConvexHull::StaticClassName()), 0);
	if (ShapeHull.Points.Num())
	{
		const FVector* const vexterBuffer = &ShapeHull.Points[0];
		hash = ndCRC64(vexterBuffer, ShapeHull.Points.Num() * sizeof(FVector), hash);
	}
	return hash;
}

ndShape* UNewtonCollisionConvexHull::CreateShape() const
{
	if (ShapeHull.Points.Num())
	{
		ndArray<ndVector> points;
		for (ndInt32 i = ShapeHull.Points.Num() - 1; i >= 0; --i)
		{
			const ndVector p(ndFloat32(ShapeHull.Points[i].X), ndFloat32(ShapeHull.Points[i].Y), ndFloat32(ShapeHull.Points[i].Z), ndFloat32(0.0f));
			points.PushBack(p);
		}
		ndShape* const shape = new ndShapeConvexHull(ShapeHull.Points.Num(), sizeof(ndVector), Tolerance, &points[0].m_x, MaxVertexCount);
		return shape;
	}
	return new ndShapeNull();
}

void UNewtonCollisionConvexHull::GenerateMesh(const USceneComponent* const meshComponent)
{
	const UStaticMeshComponent* const staticMeshComponent = Cast<UStaticMeshComponent>(meshComponent);
	check(staticMeshComponent);
	UStaticMesh* const staticMesh = staticMeshComponent->GetStaticMesh().Get();
	
	const FStaticMeshRenderData* const renderData = staticMesh->GetRenderData();
	const FStaticMeshLODResourcesArray& renderResource = renderData->LODResources;
	
	const FVector uScale(meshComponent->GetComponentTransform().GetScale3D());
	const ndVector scale(ndFloat32(uScale.X), ndFloat32(uScale.Y), ndFloat32(uScale.Z), ndFloat32(0.0f));
	const ndVector bakedScale(scale.Scale(UNREAL_INV_UNIT_SYSTEM));
	
	const FStaticMeshLODResources& renderLOD = renderResource[0];
	const FStaticMeshVertexBuffers& staticMeshVertexBuffer = renderLOD.VertexBuffers;;
	const FPositionVertexBuffer& positBuffer = staticMeshVertexBuffer.PositionVertexBuffer;
	
	ndArray<ndBigVector> points;
	for (ndInt32 i = positBuffer.GetNumVertices() - 1; i >= 0; --i)
	{
		const FVector3f p(positBuffer.VertexPosition(i));
		const ndVector q(ndFloat32(p.X), ndFloat32(p.Y), ndFloat32(p.Z), ndFloat32(1.0f));
		points.PushBack(q * bakedScale);
	}
	ndConvexHull3d convexHull(&points[0].m_x, sizeof(ndBigVector), points.GetCount(), Tolerance, MaxVertexCount);
	const ndArray<ndBigVector>& convexVertex = convexHull.GetVertexPool();
	
	ShapeHull.Points.SetNum(0);
	for (ndInt32 i = convexVertex.GetCount() - 1; i >= 0; --i)
	{
		FVector p(convexVertex[i].m_x, convexVertex[i].m_y, convexVertex[i].m_z);
		ShapeHull.Points.Push(p);
	}
}

void UNewtonCollisionConvexHull::SetTransform(const USceneComponent* const meshComponent)
{
	FTransform bodyTransform(GetComponentTransform());
	for (USceneComponent* parent = GetAttachParent(); parent; parent = parent->GetAttachParent())
	{
		bodyTransform = parent->GetComponentTransform();
		if (Cast<UNewtonRigidBody>(parent))
		{
			break;
		}
	}
	FTransform meshGlobalTransform(meshComponent->GetComponentTransform());
	meshGlobalTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
	const FTransform localTransform(meshGlobalTransform * bodyTransform.Inverse());

	SetComponentToWorld(meshGlobalTransform);

	// for some reason, this does not work in the unreal editor
	SetRelativeRotation_Direct(localTransform.Rotator());
	SetRelativeScale3D_Direct(localTransform.GetScale3D());
	SetRelativeLocation_Direct(localTransform.GetLocation());
}

void UNewtonCollisionConvexHull::InitStaticMeshCompoment(const USceneComponent* const meshComponent)
{
	SetTransform(meshComponent);
	GenerateMesh(meshComponent);
}

void UNewtonCollisionConvexHull::ApplyPropertyChanges()
{
	if (!ShapeHull.Points.Num())
	{
		if (!Cast<ANewtonSceneActor>(GetOwner()))
		{
			const USceneComponent* const parent = Cast<UStaticMeshComponent>(GetAttachParent());
			if (parent)
			{
				const FTransform localTransform;
				SetComponentToWorld(parent->GetComponentTransform());
				SetRelativeRotation_Direct(localTransform.Rotator());
				SetRelativeScale3D_Direct(localTransform.GetScale3D());
				SetRelativeLocation_Direct(localTransform.GetLocation());
				GenerateMesh(parent);
			}
		} 
		else
		{
			check(0);
			//UNewtonRigidBody* const parentBody = Cast<UNewtonRigidBody>(GetAttachParent());
			//if (parentBody && parentBody->RefSceneActor)
			//{
			//	const USceneComponent* const sceneComponent = Cast<UStaticMeshComponent>(parentBody->RefSceneActor->GetRootComponent());
			//	check(sceneComponent);
			//	const FTransform actorTransform(sceneComponent->GetComponentTransform());
			//	const FTransform localTransform(actorTransform * parentBody->GetComponentTransform().Inverse());
			//
			//	SetComponentToWorld(actorTransform);
			//	SetRelativeRotation_Direct(localTransform.Rotator());
			//	SetRelativeScale3D_Direct(localTransform.GetScale3D());
			//	SetRelativeLocation_Direct(localTransform.GetLocation());
			//	GenerateMesh(sceneComponent);
			//}
		}
	}
	m_debugVisualIsDirty = true;
	
	BuildNewtonShape();
	Super::ApplyPropertyChanges();
}

ndShapeInstance* UNewtonCollisionConvexHull::CreateInstanceShape() const
{
	ndShapeInstance* const instance = new ndShapeInstance(m_shape);

	ndVector scale(1.0f);
	UNewtonRigidBody* const bodyOwner = GetRigidBodyParent();
	if (bodyOwner && !Cast<UNewtonSceneRigidBody>(bodyOwner))
	{
		const FVector uScale(GetComponentTransform().GetScale3D());
		scale.m_x = ndFloat32(1.0f / uScale.X);
		scale.m_y = ndFloat32(1.0f / uScale.Y);
		scale.m_z = ndFloat32(1.0f / uScale.Z);
	}

	instance->SetScale(scale);
	instance->SetLocalMatrix(ndGetIdentityMatrix());
	return instance;
}

ndShapeInstance* UNewtonCollisionConvexHull::CreateBodyInstanceShape(const ndMatrix& bodyMatrix) const
{
	ndShapeInstance* const instance = new ndShapeInstance(m_shape);

	const FTransform transform(GetComponentTransform());
	const ndMatrix matrix(ToNewtonMatrix(transform) * bodyMatrix.OrthoInverse());

	instance->SetLocalMatrix(matrix);
	instance->SetScale(ndVector(ndFloat32(1.0f)));
	return instance;
}
