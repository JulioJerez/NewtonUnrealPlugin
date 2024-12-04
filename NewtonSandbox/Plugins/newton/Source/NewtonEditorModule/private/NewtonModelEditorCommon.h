// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NewtonModel.h"
#include "GraphTestGraph.h"
#include "NewtonModelNode.h"
#include "NewtonModelNodeJoint.h"
#include "NewtonModelNodeCollision.h"
#include "NewtonModelNodeRigidBodyRoot.h"

/**
 * Order is important here!
 * This enum is used internally to the filtering logic and represents an ordering of most filtered (hidden) to
 * least filtered (highlighted).
 */
//enum class ESkeletonTreeFilterResult : int32
enum ENewtonModelPhysicsTreeFilterResult
{
	/** Hide the item */
	Hidden,

	/** Show the item because child items were shown */
	ShownDescendant,

	/** Show the item */
	Shown,

	/** Show the item and highlight search text */
	ShownHighlighted,
};

class FNewtonModelEditorCommon
{
	public:
	FNewtonModelEditorCommon();
	~FNewtonModelEditorCommon();
};
