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


#include "NewtonModelGraph.h"
#include "UObject/ObjectSaveContext.h"

#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonModelInfo::UNewtonModelInfo()
	:Super()
	,Title(FText::FromString(TEXT("child node")))
	,Responses()
{
}

void UNewtonModelInfo::Initialize(const UNewtonModelInfo* const srcInfo)
{
	Title = srcInfo->Title;
}

//**********************************************************************************
UNewtonModelPin::UNewtonModelPin()
	:Super()
	,Connections()
{
}

//**********************************************************************************
UNewtonModelNode::UNewtonModelNode()
	:Super()
	,Posit(0.0f, 0.0f)
	,Info(nullptr)
	,InputPin(nullptr)
	,OutputPin(nullptr)
{
}

void UNewtonModelNode::Initialize(const UNewtonModelInfo* const srcInfo)
{
	check(!Info);
	check(srcInfo);
	check(!InputPin);
	check(!OutputPin);

	Info = NewObject<UNewtonModelInfo>(this);
	Info->Initialize(srcInfo);

	InputPin = NewObject<UNewtonModelPin>(this);
	OutputPin = NewObject<UNewtonModelPin>(this);
}

UNewtonModelPin* UNewtonModelNode::GetInputPin() const
{
	return InputPin;
}

UNewtonModelPin* UNewtonModelNode::GetOuputPin() const
{
	return OutputPin;
}

//**********************************************************************************
UNewtonModelNodeRoot::UNewtonModelNodeRoot()
	:Super()
{
}

void UNewtonModelNodeRoot::Initialize(const UNewtonModelInfo* const srcInfo)
{
	check(!Info);
	check(srcInfo);
	check(!InputPin);
	check(!OutputPin);

	Info = NewObject<UNewtonModelInfo>(this);
	Info->Initialize(srcInfo);

	OutputPin = NewObject<UNewtonModelPin>(this);
}

//**********************************************************************************
UNewtonModelGraph::UNewtonModelGraph()
	:Super()
	,NodesArray()
{
}
