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


#include "NewtonLink.h"

#include "NewtonAsset.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLink::UNewtonLink()
	:Super()
{
	Parent = nullptr;

	m_hidden = false;
	ShowDebug = true;
}

void UNewtonLink::AttachNode(UNewtonLink* const node)
{
	node->Parent = this;
	Children.Push(node);
}

void UNewtonLink::SetName(const TCHAR* const name)
{
	Name = FName(name);
}


TObjectPtr<USceneComponent> UNewtonLink::CreateBlueprintProxy()
{
	check(0);
	return nullptr;
}

FTransform UNewtonLink::CalculateGlobalTransform() const
{
	FTransform transform(Transform);
	for (UNewtonLink* parent = Parent; parent; parent = parent->Parent)
	{
		const FTransform parentTransform(parent->Transform);
		transform = transform * parentTransform;
	}
	return transform;
}

void UNewtonLink::SetTransform(TObjectPtr<USceneComponent> component)
{
	// the object is not attach to the world yet, therefore the relative transform is the global tranform.
	//component->SetRelativeTransform(Transform);
	component->SetRelativeTransform(CalculateGlobalTransform());
	component->UpdateComponentToWorld();
}