// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GraphTestGraph.h"
#include "ndModel/NewtonModel.h"
#include "ndModel/NewtonModelNode.h"
#include "ndModel/NewtonModelNodeJoint.h"
#include "ndModel/NewtonModelNodeCollision.h"
#include "ndModel/NewtonModelNodeJointHinge.h"
#include "ndModel/NewtonModelNodeCollisionBox.h"
#include "ndModel/NewtonModelNodeRigidBodyRoot.h"
#include "ndModel/NewtonModelNodeCollisionSphere.h"


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
