// Fill out your copyright notice in the Description page of Project Settings.


#include "NewtonModelAcyclicGraph.h"


#include "NewtonModel.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonModelAcyclicGraph::UNewtonModelAcyclicGraph()
	:Super()
{
	RootNode = nullptr;
	Title = FText::FromString(TEXT("AcyclicGraph"));
}

