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
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class ISkeletonTree;
class FWorkflowCentricApplication;


/**
 * 
 */
class NewtonModelEditorMode: public FApplicationMode
{
	public:
	NewtonModelEditorMode(TSharedRef<FWorkflowCentricApplication> editorInterface, TSharedRef<ISkeletonTree> InSkeletonTree);
	~NewtonModelEditorMode();

	virtual void PostActivateMode() override;
	virtual void PreDeactivateMode() override;
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> inTabManager) override;

	private:
	FWorkflowAllowedTabSet m_tabs;
	TWeakPtr<FWorkflowCentricApplication> m_editor;

	static FName m_editorModelName;
	static FName m_editorVersionName;

	friend class FNewtonModelEditor;
};
