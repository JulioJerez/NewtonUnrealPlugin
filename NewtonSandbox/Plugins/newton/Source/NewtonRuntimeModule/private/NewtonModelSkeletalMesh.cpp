// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelSkeletalMesh.h"

#include "NewtonJoint.h"
#include "NewtonRigidBody.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonModelSkeletalMesh::UNewtonModelSkeletalMesh()
	:Super()
{
	PrimaryComponentTick.bCanEverTick = true;

	m_matrixPallete.Empty();
};

void UNewtonModelSkeletalMesh::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* tickFunction)
{
	Super::TickComponent(deltaTime, tickType, tickFunction);

	const int bonesCount = GetNumBones();
	if (!bonesCount)
	{
		return;
	}
	if (bonesCount != m_matrixPallete.Num())
	{
		m_matrixPallete.SetNum(bonesCount);
	}

	bool isSleeping = true;
	AActor* const owner = GetOwner();
	check(owner->GetRootComponent());

	ndFixSizeArray<USceneComponent*, 256> stack;
	stack.PushBack(owner->GetRootComponent());

	FTransform invTransfrom;
	while (stack.GetCount())
	{
		USceneComponent* const component = stack.Pop();
		UNewtonRigidBody* const rigidBodyBone = Cast<UNewtonRigidBody>(component);
		if (rigidBodyBone && (rigidBodyBone->BoneIndex >= 0))
		{
			FTransform boneTransform;
			const USceneComponent* const parent = Cast<UNewtonJoint>(rigidBodyBone->GetAttachParent());
			if (!parent)
			{
				invTransfrom = rigidBodyBone->GetComponentToWorld().Inverse();
			}
			else
			{
				//boneTransform = rigidBodyBone->GetRelativeTransform() * parent->GetRelativeTransform();
				boneTransform = rigidBodyBone->GetComponentToWorld() * invTransfrom;
			}

			const FTransform boneTM(GetBoneTransform(rigidBodyBone->BoneIndex));
			boneTransform.SetScale3D(boneTM.GetScale3D());
			m_matrixPallete[rigidBodyBone->BoneIndex] = boneTransform;
		}
		const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
		for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
		{
			stack.PushBack(childrenComp[i].Get());
		}
	}
}

bool UNewtonModelSkeletalMesh::ShouldUpdateTransform(bool lodHasChanged) const
{
	Super::ShouldUpdateTransform(lodHasChanged);
	return true;
}

void UNewtonModelSkeletalMesh::FinalizeBoneTransform()
{
	if (GetNumBones() == m_matrixPallete.Num())
	{
		TArray<FTransform>& matrixPallete = GetEditableComponentSpaceTransforms();
		for (int i = GetNumBones() - 1; i >= 0; --i)
		{
			matrixPallete[i] = m_matrixPallete[i];
		}
	}

	Super::FinalizeBoneTransform();
}

