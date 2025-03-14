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

#include "NewtonCollisionWheel.h"

#include "NewtonRuntimeModule.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonCollisionWheel::UNewtonCollisionWheel()
	:Super()
{
	Radio = 50.0f;
	Width = 50.0f;
}

ndShape* UNewtonCollisionWheel::CreateShape() const
{
	return new ndShapeChamferCylinder(ndFloat32(0.75f), ndFloat32(0.5f));
}

long long UNewtonCollisionWheel::CalculateHash() const
{
	long long hash = ndCRC64(ndShapeChamferCylinder::StaticClassName(), strlen(ndShapeChamferCylinder::StaticClassName()), 0);
	hash = ndCRC64(&Radio, sizeof(float), hash);
	hash = ndCRC64(&Width, sizeof(float), hash);
	return hash;
}

void UNewtonCollisionWheel::ApplyPropertyChanges()
{
	if (BestFit)
	{
		for (USceneComponent* parent = GetAttachParent(); parent; parent = parent->GetAttachParent())
		{
			UStaticMeshComponent* const meshComp = Cast<UStaticMeshComponent>(parent);
			if (meshComp && meshComp->GetStaticMesh())
			{
				//FBoxSphereBounds bounds(meshComp->CalcBounds(FTransform()));
				//SizeX = ndMax(float(bounds.BoxExtent.X * 2.0f), 2.0f);
				//SizeY = ndMax(float(bounds.BoxExtent.Y * 2.0f), 2.0f);
				//SizeZ = ndMax(float(bounds.BoxExtent.Z * 2.0f), 2.0f);
				//SetRelativeTransform(FTransform());
				break;
			}
		}
	}
	BuildNewtonShape();

	Super::ApplyPropertyChanges();
}

ndShapeInstance* UNewtonCollisionWheel::CreateInstanceShape() const
{
	ndShapeInstance* const instance = Super::CreateInstanceShape();

	const ndVector scale(ndFloat32(Width * UNREAL_INV_UNIT_SYSTEM), ndFloat32(Radio * UNREAL_INV_UNIT_SYSTEM), ndFloat32(Radio * UNREAL_INV_UNIT_SYSTEM), ndFloat32 (0.0f));
	instance->SetScale(scale);

	//const ndMatrix alignment (ndYawMatrix(ndPi * 0.5f));
	const ndMatrix alignment(ndGetIdentityMatrix());
	instance->SetLocalMatrix(alignment * instance->GetLocalMatrix());
	return instance;
}

ndShapeInstance* UNewtonCollisionWheel::CreateBodyInstanceShape(const ndMatrix& bodyMatrix) const
{
	ndShapeInstance* const instance = Super::CreateBodyInstanceShape(bodyMatrix);

	const FVector uScale(GetComponentTransform().GetScale3D());
	const ndVector scale1(ndFloat32(uScale.X), ndFloat32(uScale.Y), ndFloat32(uScale.Z), ndFloat32(1.0f));
	const ndVector scale2(ndFloat32(Width * UNREAL_INV_UNIT_SYSTEM), ndFloat32(Radio * UNREAL_INV_UNIT_SYSTEM), ndFloat32(Radio * UNREAL_INV_UNIT_SYSTEM), ndFloat32(0.0f));
	instance->SetScale(scale1 * scale2);

	const ndMatrix alignment(ndGetIdentityMatrix());
	instance->SetLocalMatrix(alignment * instance->GetLocalMatrix());
	return instance;
}
