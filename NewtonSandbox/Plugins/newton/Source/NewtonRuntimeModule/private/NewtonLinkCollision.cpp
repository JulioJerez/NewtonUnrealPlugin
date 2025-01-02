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


#include "NewtonLinkCollision.h"

#include "NewtonModel.h"
#include "NewtonCommons.h"
#include "NewtonLinkRigidBody.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonLinkCollision::UNewtonLinkCollision()
	:Super()
{
}

void UNewtonLinkCollision::CreateWireFrameMesh(TArray<FVector>& wireFrameMesh, TObjectPtr<USkeletalMesh> mesh, int boneIndex) const
{
	class DebugWireframeMeshBuilder : public ndShapeDebugNotify
	{
		public:
		struct EdgeKey
		{
			EdgeKey(const FVector& p0, const FVector& p1)
			{
				m_data[0] = ndInt32 (p0.X * 100.0f);
				m_data[1] = ndInt32 (p0.Y * 100.0f);
				m_data[2] = ndInt32 (p0.Z * 100.0f);
				m_data[3] = ndInt32 (p1.X * 100.0f);
				m_data[4] = ndInt32 (p1.Y * 100.0f);
				m_data[5] = ndInt32 (p1.Z * 100.0f);
			}

			bool operator>(const EdgeKey& src) const
			{
				for (int i = 0; i < 6; ++i)
				{
					if (m_data[i] >= src.m_data[i])
					{
						if (m_data[i] > src.m_data[i])
						{
							return true;
						}
					}
				}
				return false;
			}

			bool operator<(const EdgeKey& src) const
			{
				for (int i = 0; i < 6; ++i)
				{
					if (m_data[i] <= src.m_data[i])
					{
						if (m_data[i] < src.m_data[i])
						{
							return true;
						}
					}
				}
				return false;
			}

			int m_data[6];
		};

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

				if (!m_filter.Find(EdgeKey(p0, p)))
				{
					m_wireFrameMesh.Push(p);
					m_wireFrameMesh.Push(p0);
					m_filter.Insert(0, EdgeKey(p, p0));
				}
				p0 = p;
			}
		}

		ndTree<ndInt32, EdgeKey> m_filter;
		TArray<FVector>& m_wireFrameMesh;
	};

	ndShapeInstance instance(CreateInstance(mesh, boneIndex));
	DebugWireframeMeshBuilder wireframe(wireFrameMesh);
	instance.DebugShape(ndGetIdentityMatrix(), wireframe);
}

ndShapeInstance UNewtonLinkCollision::CreateInstance(TObjectPtr<USkeletalMesh>, int) const
{
	check(0);
	ndShapeInstance instance(ndShapeInstance(new ndShapeNull()));
	return instance;
}