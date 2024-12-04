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


#include "GraphTestGraph.h"
#include "UObject/ObjectSaveContext.h"

#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UGraphTestInfo::UGraphTestInfo()
	:Super()
	,Title(FText::FromString(TEXT("child node")))
	,Responses()
{
}

void UGraphTestInfo::Initialize(const UGraphTestInfo* const srcInfo)
{
	Title = srcInfo->Title;
}

//**********************************************************************************
UGraphTestPin::UGraphTestPin()
	:Super()
	,Connections()
{
}

//**********************************************************************************
UGraphTestGraphNode::UGraphTestGraphNode()
	:Super()
	,Posit(0.0f, 0.0f)
	,Info(nullptr)
	,InputPin(nullptr)
	,OutputPin(nullptr)
{
}

void UGraphTestGraphNode::Initialize(const UGraphTestInfo* const srcInfo)
{
	check(!Info);
	check(srcInfo);
	check(!InputPin);
	check(!OutputPin);

	Info = NewObject<UGraphTestInfo>(this);
	Info->Initialize(srcInfo);

	InputPin = NewObject<UGraphTestPin>(this);
	OutputPin = NewObject<UGraphTestPin>(this);
}

UGraphTestPin* UGraphTestGraphNode::GetInputPin() const
{
	return InputPin;
}

UGraphTestPin* UGraphTestGraphNode::GetOuputPin() const
{
	return OutputPin;
}

//**********************************************************************************
UGraphTestGraphNodeRoot::UGraphTestGraphNodeRoot()
	:Super()
{
}

void UGraphTestGraphNodeRoot::Initialize(const UGraphTestInfo* const srcInfo)
{
	check(!Info);
	check(srcInfo);
	check(!InputPin);
	check(!OutputPin);

	Info = NewObject<UGraphTestInfo>(this);
	Info->Initialize(srcInfo);

	OutputPin = NewObject<UGraphTestPin>(this);
}

//**********************************************************************************
UGraphTestGraph::UGraphTestGraph()
	:Super()
	,NodesArray()
{
}
