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
#include "GameFramework/Actor.h"
#include "NewtonSceneActor.generated.h"

class ALandscapeProxy;

UCLASS(ClassGroup = NewtonActors, meta=(BlueprintSpawnableComponent), HideCategories = (Physics, Collision))
class NEWTONRUNTIMEMODULE_API ANewtonSceneActor : public AActor
{
	GENERATED_BODY()

	public:	
	// Sets default values for this actor's properties
	ANewtonSceneActor();

	virtual void PostLoad() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void ApplyPropertyChanges();

	protected:
	UPROPERTY(EditAnywhere, Category = Newton)
	bool GenerateShapes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NewtonScene, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNewtonSceneRigidBody> RootBody;

	private:
	void GenerateStaticMeshCollision(const AActor* const actor);
	void GenerateLandScapeCollision(const ALandscapeProxy* const landscape);
	void CreateCollisionFromUnrealPrimitive(TObjectPtr<UStaticMeshComponent> staticComponent);


	void DebugNewonModel();

	bool m_propertyChanged;
	friend class FNewtonRuntimeModule;
};
