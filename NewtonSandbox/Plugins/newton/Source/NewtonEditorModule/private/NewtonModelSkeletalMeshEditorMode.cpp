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


#include "NewtonModelSkeletalMeshEditorMode.h"
#include "IPersonaEditMode.h"
#include "IPersonaToolkit.h"
#include "Selection.h"
#include "HitProxies.h"
#include "IPersonaPreviewScene.h"
#include "AssetEditorModeManager.h"

#include "NewtonModelEditor.h"

#define LOCTEXT_NAMESPACE "NewtonModelSkeletalMeshEditorMode"

FEditorModeID UNewtonModelSkeletalMeshEditorMode::m_id("NewtonModelSkeletalMeshEditorMode");


class UNewtonModelSkeletalMeshEditorMode::FSkeletonSelectionEditMode : public IPersonaEditMode
{
	public:
	FSkeletonSelectionEditMode();

#if 0
	void SetSharedData(const TSharedRef<FPhysicsAssetEditor>& InPhysicsAssetEditor, FPhysicsAssetEditorSharedData& InSharedData) { PhysicsAssetEditorPtr = InPhysicsAssetEditor;  SharedData = &InSharedData; };

	/** IPersonaEditMode interface */
	virtual bool GetCameraTarget(FSphere& OutTarget) const override;
	virtual void GetOnScreenDebugInfo(TArray<FText>& OutDebugInfo) const override;

	/** FEdMode interface */
	virtual bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual bool InputAxis(FEditorViewportClient* inViewportClient, FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime) override;
	
	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	virtual void DrawHUD(FEditorViewportClient* ViewportClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;
	virtual bool AllowWidgetMove() override;
	
	virtual bool GetCustomInputCoordinateSystem(FMatrix& InMatrix, void* InData) override;
	virtual bool IsCompatibleWith(FEditorModeID OtherModeID) const override { return true; }
	virtual bool ReceivedFocus(FEditorViewportClient* ViewportClient, FViewport* Viewport);
	virtual bool LostFocus(FEditorViewportClient* ViewportClient, FViewport* Viewport);

	// Start IGizmoEdModeInterface overrides
	virtual bool BeginTransform(const FGizmoState& InState) override;
	virtual bool EndTransform(const FGizmoState& InState) override;
	// End IGizmoEdModeInterface overrides

private:
	/** Simulation mouse forces */
	bool SimMousePress(FEditorViewportClient* inViewportClient, FKey Key);
	void SimMouseMove(FEditorViewportClient* inViewportClient, float DeltaX, float DeltaY);
	bool SimMouseRelease();
	bool SimMouseWheelUp(FEditorViewportClient* inViewportClient);
	bool SimMouseWheelDown(FEditorViewportClient* inViewportClient);

	/** Changes the orientation of a constraint */
	void CycleSelectedConstraintOrientation();

	/** Scales a collision body */
	void ModifyPrimitiveSize(int32 BodyIndex, EAggCollisionShape::Type PrimType, int32 PrimIndex, FVector DeltaSize);

	/** Called when no scene proxy is hit, deselects everything */
	void HitNothing(FEditorViewportClient* inViewportClient);

	void CycleTransformMode();

	void OpenBodyMenu(FEditorViewportClient* inViewportClient);

	void OpenConstraintMenu(FEditorViewportClient* inViewportClient);

	void OpenSelectionMenu(FEditorViewportClient* inViewportClient);

	/** Returns the identifier for the constraint frame (child or parent) in which the manipulator widget should be drawn. */
	EConstraintFrame::Type GetConstraintFrameForWidget() const;

	// Manage the start and end of a transform action in the viewport.
	bool HandleBeginTransform();
	bool HandleEndTransform(FEditorViewportClient* inViewportClient) const;

	// Update the Center of Mass position after body transforms have been manipulated in the view port.
	void UpdateCoM();

	// Calculate a Center of Mass nudge (offset) for a given body that will locate that body's CoM at the supplied position in world space.
	FVector CalculateCoMNudgeForWorldSpacePosition(const int32 BodyIndex, const FVector& CoMPositionWorldSpace) const;

private:
	/** Shared data */
	FPhysicsAssetEditorSharedData* SharedData;

	/** Font used for drawing debug text to the viewport */
	UFont* PhysicsAssetEditorFont;

	/** Misc consts */
	const float	MinPrimSize;
	const float PhysicsAssetEditor_TranslateSpeed;
	const float PhysicsAssetEditor_RotateSpeed;
	const float PhysicsAssetEditor_LightRotSpeed;
	const float	SimHoldDistanceChangeDelta;
	const float	SimMinHoldDistance;
	const float SimGrabMoveSpeed;

	/** Simulation mouse forces */
	float SimGrabPush;
	float SimGrabMinPush;
	FVector SimGrabLocation;
	FVector SimGrabX;
	FVector SimGrabY;
	FVector SimGrabZ;

	/** Members used for interacting with the asset while the simulation is running */
	TArray<FTransform> StartManRelConTM;
	TArray<FTransform> StartManParentConTM;
	TArray<FTransform> StartManChildConTM;

	float DragX;
	float DragY;

	TWeakPtr<FPhysicsAssetEditor> PhysicsAssetEditorPtr;
#else

	bool ShouldDrawWidget() const override;
	virtual bool UsesTransformWidget() const override;
	virtual IPersonaPreviewScene& GetAnimPreviewScene() const override;
	virtual bool UsesTransformWidget(UE::Widget::EWidgetMode CheckMode) const override;
	virtual bool HandleClick(FEditorViewportClient* viewportClient, HHitProxy* hitProxy, const FViewportClick& click) override;

	virtual FVector GetWidgetLocation() const override;
	virtual bool GetCustomDrawingCoordinateSystem(FMatrix& matrix, void*) override;

	virtual bool StartTracking(FEditorViewportClient* inViewportClient, FViewport* inViewport) override;
	virtual bool EndTracking(FEditorViewportClient* inViewportClient, FViewport* inViewport) override;
	virtual bool InputDelta(FEditorViewportClient* inViewportClient, FViewport* inViewport, FVector& inDrag, FRotator& inRot, FVector& inScale) override;

	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	FNewtonModelEditor* m_editor;

	bool m_inTransaction;
#endif

};

//************************************************************************************************
//
//
//************************************************************************************************
UNewtonModelSkeletalMeshEditorMode::UNewtonModelSkeletalMeshEditorMode()
	:Super()
{
	Info = FEditorModeInfo(m_id,
		NSLOCTEXT("NewtonModelSkeletalMesh", "NewtonModelSkeletalMeshEditorModeName", "newton Model Editor Mode"),
		FSlateIcon(),
		false);

	Toolkit = nullptr;
}

TSharedRef<FLegacyEdModeWidgetHelper> UNewtonModelSkeletalMeshEditorMode::CreateWidgetHelper()
{
	TSharedRef<FLegacyEdModeWidgetHelper> helper(MakeShared<FSkeletonSelectionEditMode>());
	return helper;
}

void UNewtonModelSkeletalMeshEditorMode::SetEditor(FNewtonModelEditor* const editor)
{
	FSkeletonSelectionEditMode* const helper = (FSkeletonSelectionEditMode*)WidgetHelper.Get();
	check(helper);

	m_editor = editor;
	helper->m_editor = m_editor;
	helper->m_inTransaction = false;
}

bool UNewtonModelSkeletalMeshEditorMode::StartTracking(FEditorViewportClient* inViewportClient, FViewport* inViewport)
{
	FSkeletonSelectionEditMode* const helper = (FSkeletonSelectionEditMode*)WidgetHelper.Get();
	check(helper);
	return helper->StartTracking(inViewportClient, inViewport);
}

bool UNewtonModelSkeletalMeshEditorMode::EndTracking(FEditorViewportClient* inViewportClient, FViewport* inViewport)
{
	FSkeletonSelectionEditMode* const helper = (FSkeletonSelectionEditMode*)WidgetHelper.Get();
	check(helper);
	return helper->EndTracking(inViewportClient, inViewport);
}

//**********************************************************************************************
//
//**********************************************************************************************
UNewtonModelSkeletalMeshEditorMode::FSkeletonSelectionEditMode::FSkeletonSelectionEditMode()
	:IPersonaEditMode()
{
	m_editor = nullptr;
	m_inTransaction = false;
}

bool UNewtonModelSkeletalMeshEditorMode::FSkeletonSelectionEditMode::UsesTransformWidget() const
{
	return ShouldDrawWidget();
}

bool UNewtonModelSkeletalMeshEditorMode::FSkeletonSelectionEditMode::UsesTransformWidget(UE::Widget::EWidgetMode CheckMode) const
{
	//if (SharedData->GetSelectedConstraint() && CheckMode == UE::Widget::WM_Scale)
	//{
	//	return false;
	//}
	//
	//if (SharedData->GetSelectedCoM() && (CheckMode != UE::Widget::WM_Translate))
	//{
	//	return false;
	//}

	bool traformTest = (CheckMode == UE::Widget::WM_None ||
						CheckMode == UE::Widget::WM_Scale ||
						CheckMode == UE::Widget::WM_Rotate || 
						CheckMode == UE::Widget::WM_Translate);
	return ShouldDrawWidget() && traformTest;
}

IPersonaPreviewScene& UNewtonModelSkeletalMeshEditorMode::FSkeletonSelectionEditMode::GetAnimPreviewScene() const
{
	return *static_cast<IPersonaPreviewScene*>(static_cast<FAssetEditorModeManager*>(Owner)->GetPreviewScene());
}

bool UNewtonModelSkeletalMeshEditorMode::FSkeletonSelectionEditMode::HandleClick(FEditorViewportClient* viewportClient, HHitProxy* hitProxy, const FViewportClick& click)
{
	if (click.GetKey() == EKeys::LeftMouseButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("xxxxxxxxx"));
		UE_LOG(LogTemp, Warning, TEXT("function:%s line:%d"), TEXT(__FUNCTION__), __LINE__);
	}
	else if (click.GetKey() == EKeys::RightMouseButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("xxxxxxxxx"));
		UE_LOG(LogTemp, Warning, TEXT("function:%s line:%d"), TEXT(__FUNCTION__), __LINE__);
	}

	return false;
}

void UNewtonModelSkeletalMeshEditorMode::FSkeletonSelectionEditMode::Render(const FSceneView* view, FViewport* viewport, FPrimitiveDrawInterface* pdi)
{
	check(m_editor);

	//EPhysicsAssetEditorMeshViewMode MeshViewMode = SharedData->GetCurrentMeshViewMode(SharedData->bRunningSimulation);
	//
	//if (MeshViewMode != EPhysicsAssetEditorMeshViewMode::None)
	//{
	//	SharedData->EditorSkelComp->SetVisibility(true);
	//
	//	if (MeshViewMode == EPhysicsAssetEditorMeshViewMode::Wireframe)
	//	{
	//		SharedData->EditorSkelComp->SetForceWireframe(true);
	//	}
	//	else
	//	{
	//		SharedData->EditorSkelComp->SetForceWireframe(false);
	//	}
	//}
	//else
	//{
	//	SharedData->EditorSkelComp->SetVisibility(false);
	//}
	//
	// Draw phat skeletal component.
	//SharedData->EditorSkelComp->DebugDraw(View, PDI);

	IPersonaEditMode::Render(view, viewport, pdi);
	m_editor->DebugDraw(view, viewport, pdi);
}

bool UNewtonModelSkeletalMeshEditorMode::FSkeletonSelectionEditMode::ShouldDrawWidget() const
{
	check(m_editor);
	return m_editor->ShouldDrawWidget();
}

bool UNewtonModelSkeletalMeshEditorMode::FSkeletonSelectionEditMode::GetCustomDrawingCoordinateSystem(FMatrix& matrix, void*)
{
	check(m_editor);
	matrix = m_editor->GetWidgetMatrix();
	matrix.SetOrigin(FVector(0.0, 0.0, 0.0));
	return true;
}

FVector UNewtonModelSkeletalMeshEditorMode::FSkeletonSelectionEditMode::GetWidgetLocation() const
{
	FMatrix matrix(m_editor->GetWidgetMatrix());
	return matrix.GetOrigin();
}

bool UNewtonModelSkeletalMeshEditorMode::FSkeletonSelectionEditMode::StartTracking(FEditorViewportClient* inViewportClient, FViewport* inViewport)
{
	check(m_editor);
	if (m_editor->HaveSelection())
	{
		//UE_LOG(LogTemp, Warning, TEXT("function:%s line:%d"), TEXT(__FUNCTION__), __LINE__);
		const UE::Widget::EWidgetMode widgetMode = inViewportClient->GetWidgetMode();

		if (widgetMode == UE::Widget::WM_Rotate)
		{
			GEditor->BeginTransaction(LOCTEXT("AnimationEditorViewport_RotateBone", "Rotate Bone"));
		}
		else
		{
			GEditor->BeginTransaction(LOCTEXT("AnimationEditorViewport_TranslateBone", "Translate Bone"));
		}
		m_inTransaction = true;
		return true;
	}

	return false;
}

bool UNewtonModelSkeletalMeshEditorMode::FSkeletonSelectionEditMode::EndTracking(FEditorViewportClient* inViewportClient, FViewport* inViewport)
{
	check(m_editor);
	if (m_inTransaction)
	{
		//UE_LOG(LogTemp, Warning, TEXT("function:%s line:%d"), TEXT(__FUNCTION__), __LINE__);
		m_inTransaction = false;
		GEditor->EndTransaction();
		return true;
	}

	return false;
}

bool UNewtonModelSkeletalMeshEditorMode::FSkeletonSelectionEditMode::InputDelta(FEditorViewportClient* inViewportClient, FViewport* inViewport, FVector& inDrag, FRotator& inRot, FVector& inScale)
{
	bool bHandled = false;
	const EAxisList::Type currentAxis = inViewportClient->GetCurrentWidgetAxis();
	if (m_inTransaction && (currentAxis != EAxisList::None))
	{
		//UE_LOG(LogTemp, Warning, TEXT("function:%s line:%d"), TEXT(__FUNCTION__), __LINE__);
		bHandled = true;

		//const UE::Widget::EWidgetMode widgetMode = inViewportClient->GetWidgetMode();
		//const ECoordSystem coordSystem = inViewportClient->GetWidgetCoordSystemSpace();
		m_editor->ApplyDeltaTransform(inDrag, inRot, inScale);
		inViewport->Invalidate();
	}

	return bHandled;
}

#undef LOCTEXT_NAMESPACE
