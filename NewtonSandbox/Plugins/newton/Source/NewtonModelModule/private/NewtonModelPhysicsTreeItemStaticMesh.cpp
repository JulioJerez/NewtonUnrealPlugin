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


#include "NewtonModelPhysicsTreeItemStaticMesh.h"

#include "NewtonModelEditor.h"
#include "NewtonModelPhysicsTreeItemShape.h"
#include "NewtonModelPhysicsTreeItemAcyclicGraphs.h"


FNewtonModelPhysicsTreeItemStaticMesh::FNewtonModelPhysicsTreeItemStaticMesh(const FNewtonModelPhysicsTreeItemStaticMesh& src)
	:FNewtonModelPhysicsTreeItem(src)
{
}

FNewtonModelPhysicsTreeItemStaticMesh::FNewtonModelPhysicsTreeItemStaticMesh(TSharedPtr<FNewtonModelPhysicsTreeItem> parentNode, TObjectPtr<UNewtonLink> modelNode, FNewtonModelEditor* const editor)
	:FNewtonModelPhysicsTreeItem(parentNode, modelNode, editor)
{
	CreateWireFrameMesh();
}

FNewtonModelPhysicsTreeItem* FNewtonModelPhysicsTreeItemStaticMesh::Clone() const
{
	return new FNewtonModelPhysicsTreeItemStaticMesh(*this);
}

FName FNewtonModelPhysicsTreeItemStaticMesh::BrushName() const
{
	return TEXT("bodyIcon.png");
}

void FNewtonModelPhysicsTreeItemStaticMesh::CreateWireFrameMesh() const
{
	m_wireFrameMesh.Empty();
	UNewtonLinkStaticMesh* const meshNodeInfo = Cast<UNewtonLinkStaticMesh>(m_node);
	check(meshNodeInfo);
	if (meshNodeInfo->StaticMesh)
	{
		check(m_parent);
		const FTransform parentTranform(m_parent->CalculateGlobalTransform());
		const FVector scale(parentTranform.GetScale3D());
		meshNodeInfo->Transform.SetScale3D(FVector(1.0f / scale.X, 1.0f / scale.Y, 1.0f / scale.Z));

		meshNodeInfo->CreateWireFrameMesh(m_wireFrameMesh);
	}
}

void FNewtonModelPhysicsTreeItemStaticMesh::OnPropertyChange(const FPropertyChangedEvent& event)
{
	// do not call base class 
	FProperty* const property = event.Property;
	if (property->GetName() == TEXT("StaticMesh"))
	{
		CreateWireFrameMesh();
	}
}

void FNewtonModelPhysicsTreeItemStaticMesh::DebugDrawSolid(FPrimitiveDrawInterface* const pdi) const
{
	const UNewtonLinkStaticMesh* const meshNode = Cast<UNewtonLinkStaticMesh>(m_node);

	const FStaticMeshRenderData* const renderData = meshNode->StaticMesh->GetRenderData();
	const FStaticMeshLODResources& modelLod = renderData->LODResources[0];
	const FVertexFactory* const vertexFactory = &renderData->LODVertexFactories[0].VertexFactory;
	const FRawStaticIndexBuffer* const indexBuffer = &modelLod.IndexBuffer;

	const FStaticMeshSectionArray& sections = modelLod.Sections;
	const TArray<FStaticMaterial>& materials = meshNode->StaticMesh->GetStaticMaterials();

	// Draw the static mesh elements.
	for (int32 index = sections.Num() - 1; index >= 0; --index)
	{
		const FStaticMeshSection& section = modelLod.Sections[index];

		//const FStaticMeshLODResources& LOD = renderData->LODResources[0];
		////const FStaticMeshVertexFactories& VFs = renderData->LODVertexFactories[0];
		//const FStaticMeshSection& Section = LOD.Sections[index];
		//const FLODInfo& ProxyLODInfo = LODs[index];
		//UMaterialInterface* MaterialInterface = ProxyLODInfo.Sections[SectionIndex].Material;
		//const FMaterialRenderProxy* MaterialRenderProxy = MaterialInterface->GetRenderProxy();

		const FStaticMaterial& material = materials[section.MaterialIndex];
		TObjectPtr<class UMaterialInterface> materialInterface(material.MaterialInterface);
		const FMaterialRenderProxy* const materialRenderProxy = materialInterface->GetRenderProxy();

		FMeshBatch meshBatch;
		FMeshBatchElement& element = meshBatch.Elements[0];

		meshBatch.bWireframe = 1;
		meshBatch.Type = PT_TriangleList;
		meshBatch.VertexFactory = vertexFactory;
		meshBatch.MaterialRenderProxy = materialRenderProxy;
		meshBatch.DepthPriorityGroup = SDPG_Foreground;

		element.IndexBuffer = indexBuffer;
		element.FirstIndex = section.FirstIndex;
		element.NumPrimitives = section.NumTriangles;
		element.MinVertexIndex = section.MinVertexIndex;
		element.MaxVertexIndex = section.MaxVertexIndex;
		element.PrimitiveUniformBufferResource = &GIdentityPrimitiveUniformBuffer;
		pdi->DrawMesh(meshBatch);
	}
}

void FNewtonModelPhysicsTreeItemStaticMesh::DebugDrawWireFrame(FPrimitiveDrawInterface* const pdi) const
{
	UNewtonLinkStaticMesh* const meshNode = Cast<UNewtonLinkStaticMesh>(m_node);
	if (m_wireFrameMesh.Num())
	{
		const FLinearColor color(FColor::Black);
		float thickness = NEWTON_EDITOR_DEBUG_THICKENESS * 2.0f;

		const FTransform transform(CalculateGlobalTransform());
		for (int i = m_wireFrameMesh.Num() - 2; i >= 0; i -= 2)
		{
			FVector4 p0(transform.TransformFVector4(m_wireFrameMesh[i + 0]));
			FVector4 p1(transform.TransformFVector4(m_wireFrameMesh[i + 1]));
			pdi->DrawLine(p0, p1, color, SDPG_Foreground, thickness);
		}
	}
}

void FNewtonModelPhysicsTreeItemStaticMesh::DebugDraw(const FSceneView* const view, FViewport* const viewport, FPrimitiveDrawInterface* const pdi) const
{
	const UNewtonLinkStaticMesh* const meshNode = Cast<UNewtonLinkStaticMesh>(m_node);
	if (meshNode->ShowDebug && meshNode && meshNode->StaticMesh)
	{
		//DebugDrawSolid(pdi);
		DebugDrawWireFrame(pdi);
	}
}

bool FNewtonModelPhysicsTreeItemStaticMesh::HaveSelection() const
{
	const UNewtonLinkStaticMesh* const meshNode = Cast<UNewtonLinkStaticMesh>(m_node);
	check(meshNode);
	return meshNode->ShowDebug;
}

bool FNewtonModelPhysicsTreeItemStaticMesh::ShouldDrawWidget() const
{
	const UNewtonLinkStaticMesh* const meshNode = Cast<UNewtonLinkStaticMesh>(m_node);
	check(meshNode);
	return meshNode->ShowDebug;
}

FMatrix FNewtonModelPhysicsTreeItemStaticMesh::GetWidgetMatrix() const
{
	return FNewtonModelPhysicsTreeItem::GetWidgetMatrix();
}

void FNewtonModelPhysicsTreeItemStaticMesh::ApplyDeltaTransform(const FVector& inDrag, const FRotator& inRot, const FVector& inScale)
{
	UNewtonLinkStaticMesh* const meshNode = Cast<UNewtonLinkStaticMesh>(m_node);
	check(meshNode);

	//UE_LOG(LogTemp, Warning, TEXT("%d %f %f %f"), mode, inDrag.X, inDrag.Y, inDrag.Z);
	if ((inDrag.X != 0.0f) || (inDrag.Y != 0.0f) || (inDrag.Z != 0.0f))
	{
		const FTransform parentTransform(m_parent->CalculateGlobalTransform());
		FTransform globalTransform(m_node->Transform * parentTransform);

		globalTransform.SetLocation(globalTransform.GetLocation() + inDrag);
		meshNode->Transform.SetLocation((globalTransform * parentTransform.Inverse()).GetLocation());
	}

	if ((inRot.Pitch != 0.0f) || (inRot.Yaw != 0.0f) || (inRot.Roll != 0.0f))
	{
		const FTransform deltaTransform(inRot);
		const FTransform parentTransform(m_parent->CalculateGlobalTransform());

		FTransform newTransform(meshNode->Transform * parentTransform * deltaTransform * parentTransform.Inverse());
		newTransform.NormalizeRotation();
		meshNode->Transform.SetRotation(newTransform.GetRotation());
	}

	if ((inScale.X != 0.0f) || (inScale.Y != 0.0f) || (inScale.Z != 0.0f))
	{
		FVector scale(meshNode->Transform.GetScale3D() + inScale);
		scale.X = (scale.X < 0.01f) ? 0.01f : scale.X;
		scale.Y = (scale.Y < 0.01f) ? 0.01f : scale.Y;
		scale.Z = (scale.Z < 0.01f) ? 0.01f : scale.Z;
		meshNode->Transform.SetScale3D(scale);
	}
}
