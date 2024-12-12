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

#if 0
class UNewtonModelSkeletalMeshEditorMode::ModeWidgetHelper : public FLegacyEdModeWidgetHelper
{
	public:
	ModeWidgetHelper()
		:FLegacyEdModeWidgetHelper()
	{
	}

	FVector GetWidgetLocation() const
	{
		if (const AActor* SelectedActor = GetCurrentSelectedActor())
		{
			return SelectedActor->GetActorLocation();
		}
		else
		{
			return FLegacyEdModeWidgetHelper::GetWidgetLocation();
		}
	}

	AActor* GetCurrentSelectedActor() const
	{
		if (!ensure(Owner))
		{
			return nullptr;
		}
		
		if (USelection* currentSelection = Owner->GetSelectedActors())
		{
		//	ensureMsgf(CurrentSelection->Num() < 2, TEXT("CVD does not support multi selection yet"));
		//
		//	return CurrentSelection->GetTop<AActor>();
		}

		return nullptr;
	}

	bool InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale)
	{
		check(0);
		bool bHandled = false;
		//if (InViewportClient->bWidgetAxisControlledByDrag)
		//{
		//	if (AActor* SelectedActor = GetCurrentSelectedActor())
		//	{
		//
		//		FActorElementEditorViewportInteractionCustomization::ApplyDeltaToActor(
		//			SelectedActor, true, &InDrag,
		//			&InRot, &InScale, SelectedActor->GetActorLocation(), FInputDeviceState());
		//
		//		bHandled = true;
		//	}
		//}
		//
		//if (bHandled)
		//{
		//	return true;
		//}
		//else
		//{
		//	return FLegacyEdModeWidgetHelper::InputDelta(InViewportClient, InViewport, InDrag, InRot, InScale);
		//}
		return bHandled;
	}

	bool UsesTransformWidget(UE::Widget::EWidgetMode CheckMode) const
	{
		return UsesTransformWidget();
	}

	bool UsesTransformWidget() const
	{
		if (const AActor* selectedActor = GetCurrentSelectedActor())
		{
		//	// Particle Actors cannot be moved
		//	return !SelectedActor->IsA(AChaosVDParticleActor::StaticClass());
		}
		check(0);
		return false;
	}
};
#else


class UNewtonModelSkeletalMeshEditorMode::ModeWidgetHelper : public IPersonaEditMode
{
public:
//	static FName ModeName;

	ModeWidgetHelper();


#if 0
	void SetSharedData(const TSharedRef<FPhysicsAssetEditor>& InPhysicsAssetEditor, FPhysicsAssetEditorSharedData& InSharedData) { PhysicsAssetEditorPtr = InPhysicsAssetEditor;  SharedData = &InSharedData; };

	/** IPersonaEditMode interface */
	virtual bool GetCameraTarget(FSphere& OutTarget) const override;
	virtual void GetOnScreenDebugInfo(TArray<FText>& OutDebugInfo) const override;

	/** FEdMode interface */
	virtual bool StartTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport) override;
	virtual bool EndTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport) override;
	virtual bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual bool InputAxis(FEditorViewportClient* InViewportClient, FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime) override;
	virtual bool InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) override;
	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	virtual void DrawHUD(FEditorViewportClient* ViewportClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;
	virtual bool AllowWidgetMove() override;
	virtual FVector GetWidgetLocation() const override;
	virtual bool GetCustomDrawingCoordinateSystem(FMatrix& InMatrix, void* InData) override;
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
	bool SimMousePress(FEditorViewportClient* InViewportClient, FKey Key);
	void SimMouseMove(FEditorViewportClient* InViewportClient, float DeltaX, float DeltaY);
	bool SimMouseRelease();
	bool SimMouseWheelUp(FEditorViewportClient* InViewportClient);
	bool SimMouseWheelDown(FEditorViewportClient* InViewportClient);

	/** Changes the orientation of a constraint */
	void CycleSelectedConstraintOrientation();

	/** Scales a collision body */
	void ModifyPrimitiveSize(int32 BodyIndex, EAggCollisionShape::Type PrimType, int32 PrimIndex, FVector DeltaSize);

	/** Called when no scene proxy is hit, deselects everything */
	void HitNothing(FEditorViewportClient* InViewportClient);

	void CycleTransformMode();

	void OpenBodyMenu(FEditorViewportClient* InViewportClient);

	void OpenConstraintMenu(FEditorViewportClient* InViewportClient);

	void OpenSelectionMenu(FEditorViewportClient* InViewportClient);

	/** Returns the identifier for the constraint frame (child or parent) in which the manipulator widget should be drawn. */
	EConstraintFrame::Type GetConstraintFrameForWidget() const;

	// Manage the start and end of a transform action in the viewport.
	bool HandleBeginTransform();
	bool HandleEndTransform(FEditorViewportClient* InViewportClient) const;

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

	virtual class IPersonaPreviewScene& GetAnimPreviewScene() const override;

	bool ShouldDrawWidget() const override;
	virtual bool UsesTransformWidget() const override;
	virtual bool UsesTransformWidget(UE::Widget::EWidgetMode CheckMode) const override;
	virtual bool HandleClick(FEditorViewportClient* viewportClient, HHitProxy* hitProxy, const FViewportClick& click) override;

	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;

	FNewtonModelEditor* m_editor;
#endif

};

#endif


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
	TSharedRef<FLegacyEdModeWidgetHelper> helper(MakeShared<ModeWidgetHelper>());
	return helper;
}

void UNewtonModelSkeletalMeshEditorMode::SetEditor(FNewtonModelEditor* const editor)
{
	m_editor = editor;

	ModeWidgetHelper* const helper = (ModeWidgetHelper*)WidgetHelper.Get();
	helper->m_editor = m_editor;
}

//**********************************************************************************************
//
//**********************************************************************************************
UNewtonModelSkeletalMeshEditorMode::ModeWidgetHelper::ModeWidgetHelper()
	:IPersonaEditMode()
{
}

bool UNewtonModelSkeletalMeshEditorMode::ModeWidgetHelper::ShouldDrawWidget() const
{
	//return !SharedData->bRunningSimulation && (SharedData->GetSelectedBody() || SharedData->GetSelectedConstraint() || SharedData->GetSelectedCoM());
	return true;
}

bool UNewtonModelSkeletalMeshEditorMode::ModeWidgetHelper::UsesTransformWidget() const
{
	return ShouldDrawWidget();
}

bool UNewtonModelSkeletalMeshEditorMode::ModeWidgetHelper::UsesTransformWidget(UE::Widget::EWidgetMode CheckMode) const
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

IPersonaPreviewScene& UNewtonModelSkeletalMeshEditorMode::ModeWidgetHelper::GetAnimPreviewScene() const
{
	return *static_cast<IPersonaPreviewScene*>(static_cast<FAssetEditorModeManager*>(Owner)->GetPreviewScene());
}

bool UNewtonModelSkeletalMeshEditorMode::ModeWidgetHelper::HandleClick(FEditorViewportClient* viewportClient, HHitProxy* hitProxy, const FViewportClick& click)
{
	if (click.GetKey() == EKeys::LeftMouseButton)
	{
		check(0);
		//if (hitProxy && hitProxy->IsA(HPhysicsAssetEditorEdBoneProxy::StaticGetType()))
		//{
		//	HPhysicsAssetEditorEdBoneProxy* BoneProxy = (HPhysicsAssetEditorEdBoneProxy*)hitProxy;
		//
		//	SharedData->HitBone(BoneProxy->BodyIndex, BoneProxy->PrimType, BoneProxy->PrimIndex, InViewportClient->IsCtrlPressed() || InViewportClient->IsShiftPressed());
		//	return true;
		//}
		//else if (hitProxy && hitProxy->IsA(HPhysicsAssetEditorEdConstraintProxy::StaticGetType()))
		//{
		//	HPhysicsAssetEditorEdConstraintProxy* ConstraintProxy = (HPhysicsAssetEditorEdConstraintProxy*)hitProxy;
		//
		//	SharedData->HitConstraint(ConstraintProxy->ConstraintIndex, InViewportClient->IsCtrlPressed() || InViewportClient->IsShiftPressed());
		//	return true;
		//}
		//else if (hitProxy && hitProxy->IsA(HPhysicsAssetEditorEdCoMProxy::StaticGetType()))
		//{
		//	HPhysicsAssetEditorEdCoMProxy* CoMProxy = (HPhysicsAssetEditorEdCoMProxy*)hitProxy;
		//
		//	SharedData->HitCoM(CoMProxy->BodyIndex, InViewportClient->IsCtrlPressed() || InViewportClient->IsShiftPressed());
		//	return true;
		//}
		//else
		//{
		//	HitNothing(InViewportClient);
		//}
	}
	else if (click.GetKey() == EKeys::RightMouseButton)
	{
		check(0);
		//if (hitProxy && hitProxy->IsA(HPhysicsAssetEditorEdBoneProxy::StaticGetType()))
		//{
		//	HPhysicsAssetEditorEdBoneProxy* BoneProxy = (HPhysicsAssetEditorEdBoneProxy*)hitProxy;
		//
		//	// Select body under cursor if not already selected	(if ctrl is held down we only add, not remove)
		//	FPhysicsAssetEditorSharedData::FSelection Selection(BoneProxy->BodyIndex, BoneProxy->PrimType, BoneProxy->PrimIndex);
		//	if (!SharedData->IsBodySelected(Selection))
		//	{
		//		if (!InViewportClient->IsCtrlPressed())
		//		{
		//			SharedData->ClearSelectedBody();
		//			SharedData->ClearSelectedConstraints();
		//		}
		//
		//		SharedData->SetSelectedBody(Selection, true);
		//	}
		//
		//	// Pop up menu, if we have a body selected.
		//	if (SharedData->GetSelectedBody())
		//	{
		//		OpenBodyMenu(InViewportClient);
		//	}
		//
		//	return true;
		//}
		//else if (hitProxy && hitProxy->IsA(HPhysicsAssetEditorEdConstraintProxy::StaticGetType()))
		//{
		//	HPhysicsAssetEditorEdConstraintProxy* ConstraintProxy = (HPhysicsAssetEditorEdConstraintProxy*)hitProxy;
		//
		//	// Select constraint under cursor if not already selected (if ctrl is held down we only add, not remove)
		//	if (!SharedData->IsConstraintSelected(ConstraintProxy->ConstraintIndex))
		//	{
		//		if (!InViewportClient->IsCtrlPressed())
		//		{
		//			SharedData->ClearSelectedBody();
		//			SharedData->ClearSelectedConstraints();
		//		}
		//
		//		SharedData->SetSelectedConstraint(ConstraintProxy->ConstraintIndex, true);
		//	}
		//
		//	// Pop up menu, if we have a constraint selected.
		//	if (SharedData->GetSelectedConstraint())
		//	{
		//		OpenConstraintMenu(InViewportClient);
		//	}
		//
		//	return true;
		//}
		//else
		//{
		//	OpenSelectionMenu(InViewportClient);
		//	return true;
		//}
	}

	return false;
}

void UNewtonModelSkeletalMeshEditorMode::ModeWidgetHelper::Render(const FSceneView* view, FViewport* viewport, FPrimitiveDrawInterface* pdi)
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

	// Draw phat skeletal component.
	//SharedData->EditorSkelComp->DebugDraw(View, PDI);

	m_editor->DebugDraw(view, viewport, pdi);

}



#undef LOCTEXT_NAMESPACE
