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


#include "NewtonModelVehicleFactory.h"

#include "NewtonModelModule.h"
#include "NewtonModelEditorCommon.h"
#include "NewtonModelVehicleAsset.h"

UNewtonModelVehicleFactory::UNewtonModelVehicleFactory()
	:UFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UNewtonModelVehicleAsset::StaticClass();
}

FText UNewtonModelVehicleFactory::GetDisplayName() const
{
	const FText factoryName(FText::FromString(ND_NEWTON_MODEL_VEHICLE_DISPLAY_NAME));
	return factoryName;
}

bool UNewtonModelVehicleFactory::ShouldShowInNewMenu() const
{
	return true;
}

bool UNewtonModelVehicleFactory::CanCreateNew() const
{
	return true;
}

UObject* UNewtonModelVehicleFactory::FactoryCreateNew(UClass* inClass, UObject* inParent, FName inName, EObjectFlags flags, UObject* context, FFeedbackContext* Warn, FName callingContext)
{
	UNewtonAsset* const newtonAsset = NewObject<UNewtonModelVehicleAsset>(inParent, inName, flags);
	newtonAsset->SkeletalMeshAsset = NewObject<USkeletalMesh>(newtonAsset, FName(TEXT("emptyMesh")));
	newtonAsset->SkeletalMeshAsset->SetSkeleton(NewObject<USkeleton>(newtonAsset->SkeletalMeshAsset, FName(TEXT("emptySkeleton"))));
	newtonAsset->RootBody = NewObject<UNewtonLinkRigidBodyRoot>(newtonAsset);

	return newtonAsset;
}
