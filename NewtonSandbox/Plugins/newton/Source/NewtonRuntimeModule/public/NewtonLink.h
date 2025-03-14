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
#include "UObject/NoExportTypes.h"
#include "NewtonLink.generated.h"


UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonLink : public UObject
{
	GENERATED_BODY()

	public:
	UNewtonLink();

	virtual void AttachNode(UNewtonLink* const node);
	virtual void PostCreate(const UNewtonLink* const parentNde);

	virtual TObjectPtr<USceneComponent> CreateBlueprintProxy() const;
	virtual void InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const;

	FTransform CalculateGlobalTransform() const;

	UPROPERTY(EditAnywhere, Category = Newton)
	float DebugScale;

	UPROPERTY(EditAnywhere, Category = Newton)
	bool ShowDebug;

	UPROPERTY(EditAnywhere, Category = Newton)
	FName Name;

	// local space transform
	UPROPERTY(EditAnywhere, Category = Newton)
	FTransform Transform;

	UPROPERTY()
	UNewtonLink* Parent;

	UPROPERTY()
	TArray<UNewtonLink*> Children;

	UPROPERTY()
	bool m_hidden;
};
