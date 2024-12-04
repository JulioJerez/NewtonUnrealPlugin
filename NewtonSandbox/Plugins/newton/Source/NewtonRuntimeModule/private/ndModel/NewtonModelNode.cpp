// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelNode.h"


#include "NewtonModel.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonModelNode::UNewtonModelNode()
	:Super()
{
	Parent = nullptr;
}


void UNewtonModelNode::AttachNode(UNewtonModelNode* const node)
{
	node->Parent = this;
	Children.Push(node);
}

