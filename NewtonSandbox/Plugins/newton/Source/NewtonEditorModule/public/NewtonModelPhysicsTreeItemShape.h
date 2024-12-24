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

#include "NewtonModelPhysicsTreeItem.h"

class FNewtonModelPhysicsTreeItemShape : public FNewtonModelPhysicsTreeItem
{
	public:
	NEWTON_ADD_RTTI(FNewtonModelPhysicsTreeItemShape, FNewtonModelPhysicsTreeItem)

	FNewtonModelPhysicsTreeItemShape(const FNewtonModelPhysicsTreeItemShape& src);
	FNewtonModelPhysicsTreeItemShape(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode);

	virtual FName BrushName() const override;
	virtual FNewtonModelPhysicsTreeItem* Clone() const override;

	virtual bool HaveSelection() const override;
	virtual bool ShouldDrawWidget() const override;

	virtual void OnPropertiChange(const FPropertyChangedEvent& event) override;
	virtual void ApplyDeltaTransform(const FVector& inDrag, const FRotator& inRot, const FVector& inScale) override;
	virtual void DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const override;

	TArray<FVector> m_wireFrameMesh;
};
