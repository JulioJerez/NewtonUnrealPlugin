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


#include "NewtonSceneRigidBody.h"

#include "NewtonCollision.h"
#include "NewtonWorldActor.h"
#include "NewtonRuntimeModule.h"
#include "NewtonCollisionCollection.h"
#include "ThirdParty/newtonLibrary/Public/thirdParty/ndConvexApproximation.h"

UNewtonSceneRigidBody::UNewtonSceneRigidBody()
	:Super()
{
}

void UNewtonSceneRigidBody::PostLoad()
{
	Super::PostLoad();

	const FTransform transform;
	SetComponentToWorld(transform);
	SetRelativeRotation_Direct(transform.Rotator());
	SetRelativeScale3D_Direct(transform.GetScale3D());
	SetRelativeLocation_Direct(transform.GetLocation());
}

ndShapeInstance* UNewtonSceneRigidBody::CreateCollision(const ndMatrix& bodyMatrix) const
{
	ndArray<const UNewtonCollision*> subShapes;
	const TArray<TObjectPtr<USceneComponent>>& children = GetAttachChildren();

	for (ndInt32 i = children.Num() - 1; i >= 0; --i)
	{
		UNewtonCollisionCollection* const collection = Cast<UNewtonCollisionCollection>(children[i]);
		if (collection)
		{
			const TArray<TObjectPtr<USceneComponent>>& collectionChildren = collection->GetAttachChildren();
			for (ndInt32 j = collectionChildren.Num() - 1; j >= 0; --j)
			{
				const UNewtonCollision* const shape = Cast<UNewtonCollision>(collectionChildren[j]);
				subShapes.PushBack(shape);
			}
		}
		else
		{
			const UNewtonCollision* const shape = Cast<UNewtonCollision>(children[i]);
			if (shape)
			{
				subShapes.PushBack(shape);
			}
		}
	}

	if (subShapes.GetCount() == 0)
	{
		return new ndShapeInstance(new ndShapeNull());
	}
	
	if (subShapes.GetCount() == 1)
	{
		return subShapes[0]->CreateBodyInstanceShape(bodyMatrix);
	}
	
	ndShapeInstance* const compoundInstance = new ndShapeInstance(new ndShapeCompound());
	ndShapeCompound* const compound = compoundInstance->GetShape()->GetAsShapeCompound();
	compound->BeginAddRemove();
	for (ndInt32 i = subShapes.GetCount() - 1; i >= 0; --i)
	{
		ndShapeInstance* const shapeInstance = subShapes[i]->CreateBodyInstanceShape(bodyMatrix);
		compound->AddCollision(shapeInstance);
		delete shapeInstance;
	}
	compound->EndAddRemove();
	return compoundInstance;
}
