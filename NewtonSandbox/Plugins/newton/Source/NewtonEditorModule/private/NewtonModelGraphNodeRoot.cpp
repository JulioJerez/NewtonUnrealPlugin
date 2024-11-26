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


#include "NewtonModelGraphNodeRoot.h"

#include "NewtonModelEditorCommon.h"

const FName UNewtonModelGraphNodeRoot::m_subCategory(TEXT("UNewtonModelGraphNodeRoot"));
const FName UNewtonModelGraphNodeRoot::m_nodeClassName(TEXT("Newton Model Root Node"));
const FName UNewtonModelGraphNodeRoot::m_nodeInformation(TEXT("create the root Node"));

UNewtonModelGraphNodeRoot::UNewtonModelGraphNodeRoot()
	:Super()
{
}

FLinearColor UNewtonModelGraphNodeRoot::GetNodeTitleColor() const
{
	return FLinearColor::Red;
}

void UNewtonModelGraphNodeRoot::Initialize(const UNewtonModelInfo* const srcInfo)
{
	m_ouputPin = CreateNodePin(EEdGraphPinDirection::EGPD_Output);

	m_nodeInfo = NewObject<UNewtonModelInfo>(this);
	m_nodeInfo->Title = FText::FromString(TEXT("root node"));
	if (srcInfo)
	{
		m_nodeInfo->Initialize(srcInfo);
	}
}

void UNewtonModelGraphNodeRoot::SyncPinsWithResponses()
{
	// at this time we are not changing the pin connections, since they are fixed
	//const UNewtonModelInfo* const nodeInfo = GetNodeInfo();
	//const TArray<UEdGraphPin*>& pins = GetAllPins();
	//check(pins.Num() == nodeInfo->Responses.Num());
	//
	//const TArray<FText>& reponses = nodeInfo->Responses;
	//pins[0]->PinName = FName(reponses[0].ToString());
}