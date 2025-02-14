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

#include "NewtonLink.h"
#include "NewtonAsset.h"
#include "NewtonLinkJointLoop.h"
#include "NewtonLinkJoint.h"
#include "NewtonLinkCollision.h"
#include "NewtonLinkJointHinge.h"
#include "NewtonLinkJointWheel.h"
#include "NewtonLinkJointSlider.h"
#include "NewtonLinkJointRoller.h"
#include "NewtonLinkCollisionBox.h"
#include "NewtonLinkRigidBodyRoot.h"
#include "NewtonLinkCollisionWheel.h"
#include "NewtonLinkCollisionSphere.h"
#include "NewtonLinkCollisionCapsule.h"
#include "NewtonLinkJointVehicleTire.h"
#include "NewtonLinkLoopEffector6dof.h"
#include "NewtonLinkCollisionCylinder.h"
#include "NewtonLinkJointDifferential.h"
#include "NewtonLinkCollisionConvexhull.h"
#include "NewtonLinkJointLoopDifferentialAxle.h"
#include "NewtonLinkCollisionConvexApproximate.h"

#define NEWTON_EDITOR_DEBUG_THICKENESS  0.4f
#define NEWTON_EDITOR_DEBUG_JOINT_COLOR	FColor::Red
#define NEWTON_EDITOR_DEBUG_LOOP_COLOR	FColor::Yellow

#define NEWTON_INIT_RTTI(RTTI)									\
	static const FName& GetRtti()								\
	{															\
		static FName m_rtti(TEXT(#RTTI));						\
		return m_rtti;											\
	}															\
	virtual bool IsOfRttiByName(const FName& rttiName) const	\
	{															\
		return RTTI::GetRtti() == rttiName;						\
	}															\
	virtual FName GetRttiName() const							\
	{															\
		return RTTI::GetRtti();									\
	}															

#define NEWTON_ADD_RTTI(RTTI, PARENT_RTTI)								\
	static const FName& GetRtti()										\
	{																	\
		static FName m_rtti(TEXT(#RTTI));								\
		return m_rtti;													\
	}																	\
	virtual bool IsOfRttiByName(const FName& rttiName) const override	\
	{																	\
		if (RTTI::GetRtti() == rttiName)								\
		{																\
			return true;												\
		}																\
		return PARENT_RTTI::IsOfRttiByName(rttiName);					\
	}																	\
	virtual FName GetRttiName() const									\
	{																	\
		return RTTI::GetRtti();											\
	}															

class FNewtonModelEditorCommon
{
	public:
	FNewtonModelEditorCommon();
	~FNewtonModelEditorCommon();
};
