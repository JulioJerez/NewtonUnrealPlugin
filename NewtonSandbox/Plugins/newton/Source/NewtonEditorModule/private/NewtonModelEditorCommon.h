// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NewtonModel.h"
#include "GraphTestGraph.h"
#include "NewtonModelNode.h"
#include "NewtonModelNodeJoint.h"
#include "NewtonModelNodeCollision.h"
#include "NewtonModelNodeRigidBodyRoot.h"


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


///**
// * Order is important here!
// * This enum is used internally to the filtering logic and represents an ordering of most filtered (hidden) to
// * least filtered (highlighted).
// */
////enum class ESkeletonTreeFilterResult : int32
//enum ENewtonModelPhysicsTreeFilterResult
//{
//	/** Hide the item */
//	Hidden,
//
//	/** Show the item because child items were shown */
//	ShownDescendant,
//
//	/** Show the item */
//	Shown,
//
//	/** Show the item and highlight search text */
//	ShownHighlighted,
//};

class FNewtonModelEditorCommon
{
	public:
	FNewtonModelEditorCommon();
	~FNewtonModelEditorCommon();
};
