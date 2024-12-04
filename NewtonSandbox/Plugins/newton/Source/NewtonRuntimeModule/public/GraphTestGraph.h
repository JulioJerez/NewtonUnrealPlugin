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
#include "NewtonCommons.h"

#include <functional>
#include "UObject/Object.h"
#include "GraphTestGraph.generated.h"

// ***************************************************************************** 
//
// runtime support for GraphTest asset
// 
// ***************************************************************************** 
UCLASS(BlueprintType)
class NEWTONRUNTIMEMODULE_API UGraphTestInfo : public UObject
{
	GENERATED_BODY()
	public:
	UGraphTestInfo();

	void Initialize(const UGraphTestInfo* const srcInfo);

	UPROPERTY(EditAnywhere)
	FText Title;

	UPROPERTY(EditAnywhere)
	TArray<FText> Responses;
};

//**********************************************************************************
UCLASS()
class NEWTONRUNTIMEMODULE_API UGraphTestPin : public UObject
{
	GENERATED_BODY()
	public:
	UGraphTestPin();

	UPROPERTY()
	TArray<UGraphTestPin*> Connections;
};

//**********************************************************************************
UCLASS()
class NEWTONRUNTIMEMODULE_API UGraphTestGraphNode : public UObject
{
	GENERATED_BODY()

	public:
	UGraphTestGraphNode();

	virtual void Initialize(const UGraphTestInfo* const srcInfo);
	UGraphTestPin* GetInputPin() const;
	UGraphTestPin* GetOuputPin() const;

	UPROPERTY()
	FVector2D Posit;

	UPROPERTY()
	UGraphTestInfo* Info;

	protected:
	UPROPERTY()
	UGraphTestPin* InputPin;

	UPROPERTY()
	UGraphTestPin* OutputPin;
};

UCLASS()
class NEWTONRUNTIMEMODULE_API UGraphTestGraphNodeRoot : public UGraphTestGraphNode
{
	GENERATED_BODY()

	public:
	UGraphTestGraphNodeRoot();
	virtual void Initialize(const UGraphTestInfo* const srcInfo);
};

//**********************************************************************************
UCLASS()
class NEWTONRUNTIMEMODULE_API UGraphTestGraph : public UObject
{
	GENERATED_BODY()

	public:
	UGraphTestGraph();

	UPROPERTY()
	TArray<UGraphTestGraphNode*> NodesArray;
};

