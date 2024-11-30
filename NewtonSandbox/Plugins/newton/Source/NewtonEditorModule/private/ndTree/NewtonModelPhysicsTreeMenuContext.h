// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NewtonModelPhysicsTreeMenuContext.generated.h"

class FNewtonModelPhysicsTree;

UCLASS()
class UNewtonModelPhysicsTreeMenuContext : public UObject
{
	GENERATED_BODY()
	UNewtonModelPhysicsTreeMenuContext();

public:
	/** The skeleton tree we reference */
	TWeakPtr<FNewtonModelPhysicsTree> SkeletonTree;
};