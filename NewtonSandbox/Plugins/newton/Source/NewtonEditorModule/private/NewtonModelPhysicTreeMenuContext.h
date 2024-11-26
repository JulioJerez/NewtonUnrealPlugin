// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "NewtonModelPhysicTreeMenuContext.generated.h"

class FNewtonModelPhysicTree;

UCLASS()
class UNewtonModelPhysicTreeMenuContext : public UObject
{
	GENERATED_BODY()

public:
	/** The skeleton tree we reference */
	TWeakPtr<FNewtonModelPhysicTree> SkeletonTree;
};