// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NewtonModel.generated.h"

class UNewtonModelNode;
class UNewtonModelGraph;

UCLASS(ClassGroup = Newton, BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), HideCategories = (Physics, Collision))
class NEWTONRUNTIMEMODULE_API UNewtonModel : public UObject
{
	GENERATED_BODY()
	public:
	UNewtonModel();

	UPROPERTY(EditAnywhere, Category = NewtonModel, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMesh> SkeletalMeshAsset;

	UPROPERTY()
	UNewtonModelGraph* Graph;
};


// ***************************************************************************** 
//
// runtime support for newtonModel asset
// 
// ***************************************************************************** 
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonModelPin : public UObject
{
	GENERATED_BODY()
	public:
	UNewtonModelPin();

	//UPROPERTY()
	//FName Name;

	UPROPERTY()
	FGuid Id;

	UPROPERTY()
	TArray<UNewtonModelPin*> Connections;
};

UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonModelNode : public UObject
{
	GENERATED_BODY()

	public:
	UNewtonModelNode();

	virtual void CreatePinNodes();
	virtual UNewtonModelPin* GetInputPin() const;
	virtual UNewtonModelPin* GetOuputPin() const;

	UPROPERTY()
	FVector2D Posit;

	protected:
	UPROPERTY()
	UNewtonModelPin* InputPin;

	UPROPERTY()
	UNewtonModelPin* OutputPin;
};

UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonModelGraph : public UObject
{
	GENERATED_BODY()

	public:
	UNewtonModelGraph();

	UPROPERTY()
	TArray<UNewtonModelNode*> NodesArray;
};
