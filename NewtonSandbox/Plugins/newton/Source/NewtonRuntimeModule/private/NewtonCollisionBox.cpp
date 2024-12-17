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


#include "NewtonCollisionBox.h"

#include "NewtonRuntimeModule.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonCollisionBox::UNewtonCollisionBox()
	:Super()
	,SizeX(100.0f)
	,SizeY(100.0f)
	,SizeZ(100.0f)
{
}

ndShape* UNewtonCollisionBox::CreateShape() const
{
	return new ndShapeBox(SizeX * UNREAL_INV_UNIT_SYSTEM, SizeY * UNREAL_INV_UNIT_SYSTEM, SizeZ * UNREAL_INV_UNIT_SYSTEM);
}

void UNewtonCollisionBox::InitStaticMeshCompoment(const USceneComponent* const meshComponent)
{
	const UStaticMeshComponent* const staticMeshComponent = Cast<UStaticMeshComponent>(meshComponent);
	check(staticMeshComponent);
	const UStaticMesh* const staticMesh = staticMeshComponent->GetStaticMesh().Get();
	const UBodySetup* const bodySetup = staticMesh->GetBodySetup();
	const FKAggregateGeom& aggGeom = bodySetup->AggGeom;
	check(aggGeom.BoxElems.Num() == 1);
	const FKBoxElem& element = aggGeom.BoxElems[0];

	SetTransform(meshComponent);
	const FTransform localTransformOffset(element.GetTransform());
	const FTransform globalTransform(localTransformOffset * GetComponentToWorld());
	SetComponentToWorld(globalTransform);

	const AActor* const owner = GetOwner();
	const FTransform bodyTransform(owner->GetRootComponent()->GetComponentToWorld());
	const FTransform localTransform(globalTransform * bodyTransform.Inverse());

	SetRelativeScale3D_Direct(localTransform.GetScale3D());
	SetRelativeRotation_Direct(FRotator(localTransform.GetRotation()));
	SetRelativeLocation_Direct(localTransform.GetLocation());

	SizeX = element.X;
	SizeY = element.Y;
	SizeZ = element.Z;
}

long long UNewtonCollisionBox::CalculateHash() const
{
	long long hash = ndCRC64(ndShapeBox::StaticClassName(), strlen(ndShapeBox::StaticClassName()), 0);
	hash = ndCRC64(&SizeX, sizeof(float), hash);
	hash = ndCRC64(&SizeY, sizeof(float), hash);
	hash = ndCRC64(&SizeZ, sizeof(float), hash);
	return hash;
}

void UNewtonCollisionBox::ApplyPropertyChanges()
{
	//if (BestFit)
	//{
	//	UStaticMeshComponent* const meshComp = Cast<UStaticMeshComponent>(GetAttachParent());
	//	if (meshComp && meshComp->GetStaticMesh())
	//	{
	//		//FBoxSphereBounds bounds(meshComp->CalcBounds(FTransform()));
	//		//SizeX = ndMax(float(bounds.BoxExtent.X * 2.0f), 2.0f);
	//		//SizeY = ndMax(float(bounds.BoxExtent.Y * 2.0f), 2.0f);
	//		//SizeZ = ndMax(float(bounds.BoxExtent.Z * 2.0f), 2.0f);
	//		//FTransform tranform;
	//		//tranform.SetLocation(bounds.Origin);
	//		//SetRelativeTransform(tranform);
	//	}
	//}

	BuildNewtonShape();
	Super::ApplyPropertyChanges();
}
