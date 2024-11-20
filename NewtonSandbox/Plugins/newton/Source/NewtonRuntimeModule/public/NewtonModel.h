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
#include <functional>
#include "UObject/Object.h"
#include "NewtonModel.generated.h"

// ***************************************************************************** 
//
// runtime support for NewtonModel asset
// 
// ***************************************************************************** 
UCLASS(BlueprintType)
class NEWTONRUNTIMEMODULE_API UNewtonModelInfo : public UObject
{
	GENERATED_BODY()
	public:
	UNewtonModelInfo();

	void Initialize(const UNewtonModelInfo* const srcInfo);

	UPROPERTY(EditAnywhere)
	FText Title;

	UPROPERTY(EditAnywhere)
	TArray<FText> Responses;
};

//**********************************************************************************
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonModelPin : public UObject
{
	GENERATED_BODY()
	public:
	UNewtonModelPin();

	UPROPERTY()
	TArray<UNewtonModelPin*> Connections;
};

//**********************************************************************************
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonModelNode : public UObject
{
	GENERATED_BODY()

	public:
	UNewtonModelNode();

	virtual void Initialize(const UNewtonModelInfo* const srcInfo);
	UNewtonModelPin* GetInputPin() const;
	UNewtonModelPin* GetOuputPin() const;

	UPROPERTY()
	FVector2D Posit;

	UPROPERTY()
	UNewtonModelInfo* Info;

	protected:
	UPROPERTY()
	UNewtonModelPin* InputPin;

	UPROPERTY()
	UNewtonModelPin* OutputPin;
};

UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonModelNodeRoot : public UNewtonModelNode
{
	GENERATED_BODY()

	public:
	UNewtonModelNodeRoot();
	virtual void Initialize(const UNewtonModelInfo* const srcInfo);
};

//**********************************************************************************
UCLASS()
class NEWTONRUNTIMEMODULE_API UNewtonModelGraph : public UObject
{
	GENERATED_BODY()

	public:
	UNewtonModelGraph();

	UPROPERTY()
	TArray<UNewtonModelNode*> NodesArray;
};

// ***************************************************************************** 
//
// runtime NewtonModel asset
// 
// ***************************************************************************** 
UCLASS(ClassGroup = Newton, BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), HideCategories = (Physics, Collision))
class NEWTONRUNTIMEMODULE_API UNewtonModel : public UObject
{
	GENERATED_BODY()
	public:
	UNewtonModel();

	//virtual void PreSave(FObjectPreSaveContext saveContext) override;
	//void SetPreSaveListeners(std::function<void()> onPreSaveListener);

	UPROPERTY(EditAnywhere, Category = NewtonModel, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMesh> SkeletalMeshAsset;

	UPROPERTY()
	UNewtonModelGraph* Graph;

	//private:
	//std::function<void()> m_onPresaveListener;
};

