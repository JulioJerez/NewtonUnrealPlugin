// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonSkeletalMesh.h"

#include "NewtonRuntimeModuleModule.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonSkeletalMesh::UNewtonSkeletalMesh()
	:Super()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetMobility(EComponentMobility::Movable);

	ShowDebug = false;
}
