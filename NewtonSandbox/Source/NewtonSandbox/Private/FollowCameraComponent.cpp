// Fill out your copyright notice in the Description page of Project Settings.


#include "FollowCameraComponent.h"

#include "NewtonRigidBody.h"


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