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

#pragma once

#include "CoreMinimal.h"

//#include <functional>
#include "UObject/Object.h"
#include "NewtonCommons.h"
#include "NewtonModel.generated.h"

class UGraphTestGraph;
class UNewtonModelNodeRigidBodyRoot;

UCLASS(ClassGroup = Newton, BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), HideCategories = (Physics, Collision))
class NEWTONRUNTIMEMODULE_API UNewtonModel : public UObject
{
	GENERATED_BODY()
	public:
	UNewtonModel();

	//virtual void PreSave(FObjectPreSaveContext saveContext) override;
	//void SetPreSaveListeners(std::function<void()> onPreSaveListener);

	UPROPERTY(EditAnywhere, Category = NewtonModel, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMesh> SkeletalMeshAsset;

	// to be removed 
	UPROPERTY()
	UNewtonModelNodeRigidBodyRoot* RootBody;

	//private:
	//std::function<void()> m_onPresaveListener;
};
