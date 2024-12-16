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


#include "ndModel/NewtonModelNodeCollision.h"

#include "ndModel/NewtonModelNodeRigidBody.h"

UNewtonModelNodeCollision::UNewtonModelNodeCollision()
	:Super()
{
}

void UNewtonModelNodeCollision::CreateWireFrameMesh(TArray<FVector>& wireFrameMesh) const
{
	class DebugWireframeMeshBuilder : public ndShapeDebugNotify
	{
		public:
		DebugWireframeMeshBuilder(TArray<FVector>& wireFrameMesh)
			:ndShapeDebugNotify()
			,m_wireFrameMesh(wireFrameMesh)
		{
			m_wireFrameMesh.Empty();
		}

		void DrawPolygon(ndInt32 vertexCount, const ndVector* const faceArray, const ndEdgeType* const)
		{
			const ndVector q0(faceArray[vertexCount - 1].Scale(UNREAL_UNIT_SYSTEM));

			FVector p0(float(q0.m_x), float(q0.m_y), float(q0.m_z));
			for (ndInt32 i = 0; i < vertexCount; ++i)
			{
				const ndVector q(faceArray[i].Scale(UNREAL_UNIT_SYSTEM));
				const FVector p(float(q.m_x), float(q.m_y), float(q.m_z));

				m_wireFrameMesh.Push(p);
				m_wireFrameMesh.Push(p0);
				p0 = p;
			}
		}

		TArray<FVector>& m_wireFrameMesh;
	};

	ndShapeInstance instance(CreateInstance());
	DebugWireframeMeshBuilder wireframe(wireFrameMesh);
	instance.DebugShape(ndGetIdentityMatrix(), wireframe);
}

//void UNewtonModelNodeCollision::BuildDebugMesh(TArray<FVector>& wireFrameMesh, const ndShapeInstance& instance) const
//{
//	class DebugWireframeMeshBuilder : public ndShapeDebugNotify
//	{
//		public:
//		DebugWireframeMeshBuilder(TArray<FVector>& wireFrameMesh)
//			:ndShapeDebugNotify()
//			,m_wireFrameMesh(wireFrameMesh)
//		{
//			m_wireFrameMesh.Empty();
//		}
//	
//		void DrawPolygon(ndInt32 vertexCount, const ndVector* const faceArray, const ndEdgeType* const)
//		{
//			const ndVector q0(faceArray[vertexCount - 1].Scale(UNREAL_UNIT_SYSTEM));
//
//			FVector p0(float (q0.m_x), float(q0.m_y), float(q0.m_z));
//			for (ndInt32 i = 0; i < vertexCount; ++i)
//			{
//				const ndVector q(faceArray[i].Scale(UNREAL_UNIT_SYSTEM));
//				const FVector p(float(q.m_x), float(q.m_y), float(q.m_z));
//
//				m_wireFrameMesh.Push(p);
//				m_wireFrameMesh.Push(p0);
//				p0 = p;
//			}
//		}
//
//		TArray<FVector>& m_wireFrameMesh;
//	};
//
//	DebugWireframeMeshBuilder wireframe(wireFrameMesh);
//	instance.DebugShape(ndGetIdentityMatrix(), wireframe);
//}

ndShapeInstance UNewtonModelNodeCollision::CreateInstance() const
{
	check(0);
	ndShapeInstance instance(ndShapeInstance(new ndShapeNull()));
	return instance;
}