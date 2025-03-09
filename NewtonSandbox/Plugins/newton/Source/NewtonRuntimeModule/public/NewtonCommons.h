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

#define UNREAL_UNIT_SYSTEM			float (100.0f)
#define UNREAL_INV_UNIT_SYSTEM		float (1.0f / UNREAL_UNIT_SYSTEM)

#define ND_STACK_DEPTH				2048
#define ND_HULL_TOLERANCE			float(1.0e-3f)
#define ND_DEBUG_MESH_COLOR			FColor(255.0f, 0.0f, 0.0f, 128.0f)

#define ND_MODEL_UPDATE_EVENT_NAME "OnNewtonModelUpdate"
#define ND_RIGID_BODIES_EVENT_NAME "OnNewtonForceAndTorque"


#include "Engine/Texture.h"
#include "Engine/Texture2DArray.h"

class ndShape;
class ndMatrix;
class ANewtonWorldActor;

template <typename T>
class ndSharedPtr;

enum ndPluginVersion
{
	m_firstVersion,
};

class ndHullPoints : public TArray<FVector3f>
{
	public:
	ndHullPoints();
	~ndHullPoints();
};

class ndConvexHullSet : public TArray<ndHullPoints*>
{
	public:
	ndConvexHullSet();
	~ndConvexHullSet();
};

FMatrix ToUnrealMatrix(const ndMatrix& matrix);
ndMatrix ToNewtonMatrix(const FMatrix& transform);

ndMatrix ToNewtonMatrix(const FTransform& transform);
FTransform ToUnrealTransform(const ndMatrix& matrix);
