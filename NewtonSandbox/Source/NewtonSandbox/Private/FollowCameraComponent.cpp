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


#include "FollowCameraComponent.h"

#include "ndRigidBody/NewtonRigidBody.h"


void UFollowCameraComponent::PostLoad()
{
	Super::PostLoad();
	m_beginPlay = false;
	m_localTransform = GetRelativeTransform();
}

void UFollowCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	m_beginPlay = true;
}

void UFollowCameraComponent::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
	m_beginPlay = false;
}

void UFollowCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!m_beginPlay)
	{
		return;
	}
	const UNewtonRigidBody* const body = Cast<UNewtonRigidBody>(GetAttachParent());
	if (!body)
	{
		return;
	}
	const FTransform bodyTransform(body->GetComponentTransform());
	const FMatrix bodyMatrix(bodyTransform.ToMatrixNoScale());
	const FVector matrixRight(bodyMatrix.GetScaledAxis(EAxis::Y));

	const FVector upDir(0.0f, 0.0f, 1.0f);
	const FVector front(FVector::CrossProduct(matrixRight, upDir));
	const FVector frontDir(front.GetSafeNormal());
	const FVector rightDir(FVector::CrossProduct(upDir, frontDir));

	const FMatrix matrix(frontDir, rightDir, upDir, bodyTransform.GetLocation());
	const FMatrix localMatrix(matrix * bodyMatrix.Inverse());
	const FTransform transform(m_localTransform * FTransform(localMatrix));

	SetRelativeTransform(transform);
}