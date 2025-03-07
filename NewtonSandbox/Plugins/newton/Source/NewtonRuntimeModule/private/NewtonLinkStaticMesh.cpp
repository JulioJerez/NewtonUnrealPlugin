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

#include "NewtonLinkStaticMesh.h"

#include "NewtonCommons.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkStaticMesh::UNewtonLinkStaticMesh()
	:Super()
{
	DebugScale = 1.0f;
	StaticMesh = nullptr;
	ExcludeFromBlueprint = false;
}

void UNewtonLinkStaticMesh::CreateWireFrameMesh(TArray<FVector>& wireFrameMesh) const
{
	FStaticMeshRenderData* const renderData = StaticMesh->GetRenderData();
	FStaticMeshLODResources& modelLod = renderData->LODResources[0];
	FRawStaticIndexBuffer* const indexBuffer = &modelLod.IndexBuffer;

	const FStaticMeshVertexBuffers& staticMeshVertexBuffer = modelLod.VertexBuffers;
	const FPositionVertexBuffer& positBuffer = staticMeshVertexBuffer.PositionVertexBuffer;
	
	union FilterKey
	{
		ndUnsigned64 m_key;
		struct
		{
			ndUnsigned32 m_i0;
			ndUnsigned32 m_i1;
		};
	};
	ndTree<ndInt32, ndUnsigned64> filter;

	const FStaticMeshSectionArray& sections = modelLod.Sections;
	for (int32 index = sections.Num() - 1; index >= 0; --index)
	{
		const FStaticMeshSection& section = modelLod.Sections[index];
		for (ndInt32 i = 0; i < ndInt32 (section.NumTriangles); ++i)
		{
			ndInt32 j0 = indexBuffer->GetIndex(section.FirstIndex + i * 3 + 2);
			for (ndInt32 j = 0; j < 3; ++j)
			{
				ndInt32 j1 = indexBuffer->GetIndex(section.FirstIndex + i * 3 + j);
				FilterKey key;
				key.m_i0 = j0;
				key.m_i1 = j1;
				if (!filter.Find(key.m_key))
				{
					filter.Insert(1, key.m_key);
					ndSwap(key.m_i0, key.m_i1);
					filter.Insert(1, key.m_key);

					const FVector p0(positBuffer.VertexPosition(j0));
					const FVector p1(positBuffer.VertexPosition(j1));
					wireFrameMesh.Push(p0);
					wireFrameMesh.Push(p1);
				}

				j0 = j1;
			}
		}
	}
}

TObjectPtr<USceneComponent> UNewtonLinkStaticMesh::CreateBlueprintProxy() const
{
	if (ExcludeFromBlueprint)
	{
		return TObjectPtr<UStaticMeshComponent>(nullptr);
	}
	TObjectPtr<UStaticMeshComponent> component(NewObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass(), Name, RF_Transient));
	return component;
}

void UNewtonLinkStaticMesh::InitBlueprintProxy(TObjectPtr<USceneComponent> component, TObjectPtr<USkeletalMesh> mesh) const
{
	TObjectPtr<UStaticMeshComponent> staticComponent(Cast<UStaticMeshComponent>(component));
	staticComponent->SetStaticMesh(StaticMesh);
}