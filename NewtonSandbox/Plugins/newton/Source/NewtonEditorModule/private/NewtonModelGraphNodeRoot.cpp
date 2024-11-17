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

const FName UNewtonModelGraphNodeRoot::m_subCategory(TEXT("UNewtonModelGraphNodeRoot"));
const FName UNewtonModelGraphNodeRoot::m_nodeClassName(TEXT("Newton Model Root Node"));
const FName UNewtonModelGraphNodeRoot::m_nodeInformation(TEXT("create the root Node"));


UNewtonModelGraphNodeRoot::UNewtonModelGraphNodeRoot()
	:Super()
{
}

FText UNewtonModelGraphNodeRoot::GetNodeTitle(ENodeTitleType::Type titleType) const
{
	return FText::FromString(TEXT("root node"));
}

FLinearColor UNewtonModelGraphNodeRoot::GetNodeTitleColor() const
{
	return FLinearColor::Red;
}

UEdGraphPin* UNewtonModelGraphNodeRoot::CreateNodePin(EEdGraphPinDirection direction)
{
	check (direction == EGPD_Output);
	const FName name(TEXT("child"));
	const FName category(TEXT("output"));

	UEdGraphPin* const pin = CreatePin(direction, category, name);
	pin->PinType.PinSubCategory = m_subCategory;
	return pin;
}
