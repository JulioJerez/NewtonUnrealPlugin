// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelSkeletalMesh.h"

#include "NewtonJoint.h"
#include "NewtonCommons.h"
#include "NewtonRigidBody.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonModelSkeletalMesh::UNewtonModelSkeletalMesh()
	:Super()
{
	PrimaryComponentTick.bCanEverTick = true;

	m_boneCount = 0;
	m_boneIndex.Empty();
	m_matrixPallete.Empty();
};

bool UNewtonModelSkeletalMesh::ShouldUpdateTransform(bool lodHasChanged) const
{
	Super::ShouldUpdateTransform(lodHasChanged);
	return true;
}


void UNewtonModelSkeletalMesh::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* tickFunction)
{
	Super::TickComponent(deltaTime, tickType, tickFunction);

	//UE_LOG(LogTemp, Warning, TEXT("function: %s"), TEXT(__FUNCTION__));
	const int bonesCount = GetNumBones();
	if (!bonesCount)
	{
		return;
	}
	if (bonesCount != m_matrixPallete.Num())
	{
		m_boneIndex.SetNum(bonesCount);
		m_matrixPallete.SetNum(bonesCount);
	}
	
	bool isSleeping = true;
	AActor* const owner = GetOwner();
	check(owner->GetRootComponent());
	
	ndFixSizeArray<USceneComponent*, ND_STACK_DEPTH> stack;
	stack.PushBack(owner->GetRootComponent());

	int index = 0;
	FTransform rootInverseTransfrom;
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
				rootInverseTransfrom = rigidBodyBone->m_globalTransform.Inverse();
			}
			else
			{
				boneTransform = rigidBodyBone->m_globalTransform * rootInverseTransfrom;
			}
	
			const FTransform boneTM(GetBoneTransform(rigidBodyBone->BoneIndex));
			boneTransform.SetScale3D(boneTM.GetScale3D());

			m_boneIndex[index] = rigidBodyBone->BoneIndex;
			m_matrixPallete[index] = boneTransform;
			index++;
		}
		const TArray<TObjectPtr<USceneComponent>>& childrenComp = component->GetAttachChildren();
		for (ndInt32 i = childrenComp.Num() - 1; i >= 0; --i)
		{
			stack.PushBack(childrenComp[i].Get());
		}
	}
	m_boneCount = index;
}

void UNewtonModelSkeletalMesh::FinalizeBoneTransform()
{
	TArray<FTransform>& matrixPallete = GetEditableComponentSpaceTransforms();
	for (int i = m_boneCount - 1; i >= 0; --i)
	{
		int index = m_boneIndex[i];
		matrixPallete[index] = m_matrixPallete[i];
	}

	Super::FinalizeBoneTransform();
}

