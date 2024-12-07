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


#include "factories/NewtonModelFactory.h"

#include "NewtonEditorModule.h"
#include "NewtonModelEditorCommon.h"

UNewtonModelFactory::UNewtonModelFactory()
	:UFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UNewtonModel::StaticClass();
}

FText UNewtonModelFactory::GetDisplayName() const
{
	const FText factoryName(FText::FromString(ND_MESH_EDITOR_NAME));
	return factoryName;
}

bool UNewtonModelFactory::ShouldShowInNewMenu() const
{
	return true;
}

bool UNewtonModelFactory::CanCreateNew() const
{
	return true;
}

UObject* UNewtonModelFactory::FactoryCreateNew(UClass* inClass, UObject* inParent, FName inName, EObjectFlags flags, UObject* context, FFeedbackContext* Warn, FName callingContext)
{
	UNewtonModel* const newtonModel = NewObject<UNewtonModel>(inParent, inName, flags);
	newtonModel->SkeletalMeshAsset = NewObject<USkeletalMesh>(newtonModel, FName(TEXT("emptyMesh")));
	newtonModel->SkeletalMeshAsset->SetSkeleton(NewObject<USkeleton>(newtonModel->SkeletalMeshAsset, FName(TEXT("emptySkeleton"))));

	newtonModel->RootBody = NewObject<UNewtonModelNodeRigidBodyRoot>(newtonModel);
#if 0
	UE_LOG(LogTemp, Warning, TEXT("TODO: remember to remove this:%s  file:%s line:%d"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__);
	newtonModel->RootBody->AttachNode(NewObject<UNewtonModelNodeCollisionBox>(newtonModel->RootBody));
	newtonModel->RootBody->AttachNode(NewObject<UNewtonModelNodeCollisionSphere>(newtonModel->RootBody));

	UNewtonModelNodeJoint* const hinge = NewObject<UNewtonModelNodeJointHinge>(newtonModel->RootBody);
	newtonModel->RootBody->AttachNode(hinge);

	UNewtonModelNodeRigidBody* const body = NewObject<UNewtonModelNodeRigidBody>(hinge);
	body->AttachNode(NewObject<UNewtonModelNodeCollisionBox>(body));
	hinge->AttachNode (body);
#endif

	return newtonModel;
}
