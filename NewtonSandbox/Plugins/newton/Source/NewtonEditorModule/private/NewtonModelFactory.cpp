// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelFactory.h"

#include "NewtonModel.h"
#include "NewtonEditorModule.h"

UNewtonModelFactory::UNewtonModelFactory()
	:UFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UNewtonModel::StaticClass();
}

FText UNewtonModelFactory::GetDisplayName() const
{
	const FText factoryName(FText::FromString(ND_MESH_EDITOR_NAME));
	return factoryName;
}

bool UNewtonModelFactory::ShouldShowInNewMenu() const
{
	return true;
}

bool UNewtonModelFactory::CanCreateNew() const
{
	return true;
}

UObject* UNewtonModelFactory::FactoryCreateNew(UClass* inClass, UObject* inParent, FName inName, EObjectFlags flags, UObject* context, FFeedbackContext* Warn, FName callingContext)
{
	UNewtonModel* const asset = NewObject<UNewtonModel>(inParent, inName, flags);
	return asset;
}
