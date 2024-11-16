// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModel.h"


#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonModel::UNewtonModel()
	:Super()
	,SkeletalMeshAsset(nullptr)
	,Graph(nullptr)
{
}

// ***************************************************************************** 
//
// runtime support for newtonModel asset
// 
// ***************************************************************************** 

UNewtonModelPin::UNewtonModelPin()
	:Super()
	//,Name()
	,Id()
	,Connections()
{
}

UNewtonModelNode::UNewtonModelNode()
	:Super()
	,Posit(0.0f, 0.0f)
	,InputPin(nullptr)
	,OutputPin(nullptr)
{
}

void UNewtonModelNode::CreatePinNodes()
{
	check(!InputPin);
	check(!OutputPin);
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


UNewtonModelGraph::UNewtonModelGraph()
	:Super()
	,NodesArray()
{
}
