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
#include "Components/ActorComponent.h"
#include "NewtonModel.generated.h"

class UNewtonAsset;
class ANewtonWorldActor;
class ndModelArticulation;

UCLASS(ClassGroup = Newton, BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent), HideCategories = (Physics, Collision))
class NEWTONRUNTIMEMODULE_API UNewtonModel : public UActorComponent
{
	GENERATED_BODY()
	public:
	class ModelNotify;

	UNewtonModel();

	virtual ndModelArticulation* CreateSubClassModel() const;
	virtual ndModelArticulation* CreateModel(ANewtonWorldActor* const worldActor);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = Newton)
	bool RegenerateBluePrint;

	UPROPERTY(EditAnywhere, Category = Newton)
	bool ShowDebug;

	UPROPERTY(EditAnywhere, Category = Newton)
	UNewtonAsset* NewtonAsset;

	ndModelArticulation* m_model;
};
