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

#include "NewtonLinkJointLoop.h"

#include "NewtonJoint.h"
#include "NewtonCommons.h"
#include "NewtonJointLoop.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkJointLoop::UNewtonLinkJointLoop()
	:Super()
{
	BoneIndex = -1;
	BoneName = TEXT("None");
	TargetFrame = FTransform();
}

void UNewtonLinkJointLoop::SetCommonProperties(UNewtonJoint* const joint) const
{
	UNewtonJointLoop* const loop = Cast<UNewtonJointLoop>(joint);
	check(loop);

	loop->ShowDebug = true;
	loop->TargetFrame = TargetFrame;
	loop->ReferencedBodyName = BoneName;
}
