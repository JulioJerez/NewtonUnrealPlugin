// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "NewtonSkeletalMeshFactory.generated.h"

/**
 * 
 */
UCLASS()
class NEWTONEDITORMODULE_API UNewtonSkeletalMeshFactory : public UFactory
{
	GENERATED_BODY()
	public:

	UNewtonSkeletalMeshFactory();

	virtual bool CanCreateNew() const override;
	virtual FText GetDisplayName() const override;
	virtual bool ShouldShowInNewMenu() const override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
};
