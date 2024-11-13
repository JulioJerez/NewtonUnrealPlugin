// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonSkeletalMeshFactory.h"

#include "NewtonSkeletalMesh.h"

UNewtonSkeletalMeshFactory::UNewtonSkeletalMeshFactory()
	:UFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UNewtonSkeletalMesh::StaticClass();
}

FText UNewtonSkeletalMeshFactory::GetDisplayName() const
{
	const FText factoryName(FText::FromString(TEXT("NewtonSkeletalMesh")));
	return factoryName;
}

bool UNewtonSkeletalMeshFactory::ShouldShowInNewMenu() const
{
	return true;
}

bool UNewtonSkeletalMeshFactory::CanCreateNew() const
{
	return true;
}

UObject* UNewtonSkeletalMeshFactory::FactoryCreateNew(UClass* inClass, UObject* inParent, FName inName, EObjectFlags flags, UObject* context, FFeedbackContext* Warn, FName callingContext)
{
	UNewtonSkeletalMesh* const asset = NewObject<UNewtonSkeletalMesh>(inParent, inName, flags);
	return asset;
}
