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

#include "NewtonCollisionLandscape.h"
#include "Chaos/HeightField.h"
#include "LandscapeInfo.h"
#include "LandscapeStreamingProxy.h"
#include "LandscapeHeightfieldCollisionComponent.h"

#include "NewtonRuntimeModule.h"
#include "ThirdParty/newtonLibrary/Public/dNewton/ndNewton.h"

UNewtonCollisionLandscape::UNewtonCollisionLandscape()
	:Super()
{
	m_scale_x = 0.0f;
	m_scale_y = 0.0f;
	m_scale_z = 0.0f;
	m_tileSize_x = 32;
	m_tileSize_y = 32;
}

void UNewtonCollisionLandscape::Serialize(FArchive& ar)
{
	Super::Serialize(ar);

	ar << m_scale_x;
	ar << m_scale_y;
	ar << m_scale_z;
	ar << m_minHeight;
	ar << m_tileSize_x;
	ar << m_tileSize_y;

	ar << m_materials;
	ar << m_heightfield;
}

void UNewtonCollisionLandscape::InitStaticMeshCompoment(const USceneComponent* const meshComponent)
{
	SetTransform(meshComponent);
	
	const ULandscapeHeightfieldCollisionComponent* const tile = Cast<ULandscapeHeightfieldCollisionComponent>(meshComponent);
	check(tile);
	
	const ULandscapeHeightfieldCollisionComponent::FHeightfieldGeometryRef* const heightfieldRef = tile->HeightfieldRef;
	const Chaos::FHeightField* const mapData = heightfieldRef->HeightfieldGeometry;
	check(mapData);
	const Chaos::FHeightField::FData<uint16>& elevationData = mapData->GeomData;
	
	const Chaos::FVec3 elevationScale(elevationData.Scale);
	// this is not a bug, y and z are swapped after the mapping
	ndFloat32 xScale = elevationScale[0] * UNREAL_INV_UNIT_SYSTEM;
	ndFloat32 zScale = elevationScale[1] * UNREAL_INV_UNIT_SYSTEM;
	
	ndShapeHeightfield* const heightfield = new ndShapeHeightfield(
		ndInt32(mapData->GetNumCols()), ndInt32(mapData->GetNumRows()),
		ndShapeHeightfield::m_normalDiagonals, xScale, zScale);
	
	ndReal minValue = elevationData.MinValue;
	ndReal highScale = elevationData.HeightPerUnit;
	
	const ndShape* const shape = heightfield;
	const ndShapeInfo info(shape->GetShapeInfo());
	ndReal* const dstHeight = info.m_heightfield.m_elevation;
	ndInt8* const attributes = info.m_heightfield.m_atributes;
	
	ndReal yScale = ndReal(elevationScale[2] * UNREAL_INV_UNIT_SYSTEM);
	
	ndInt32 dstRow = 0;
	ndInt32 srcRow = (ndInt32(elevationData.NumRows) - 1) * elevationData.NumCols;

	ndReal minHeight = ndReal(1.0e10f);
	ndReal maxHeight = ndReal(-1.0e10f);
	for (ndInt32 row = ndInt32(elevationData.NumRows) - 1; row >= 0; --row)
	{
		for (ndInt32 colum = 0; colum < elevationData.NumCols; ++colum)
		{
			ndReal h = minValue + ndReal(elevationData.Heights[srcRow + colum]) * highScale;
			attributes[dstRow + colum] = 0;
			h *= yScale;
			dstHeight[dstRow + colum] = h;
			minHeight = ndMin(minHeight, h);
			maxHeight = ndMax(maxHeight, h);

		}
		srcRow -= elevationData.NumCols;
		dstRow += elevationData.NumCols;
	}
	
	//m_scale_z = 1.0f;
	m_minHeight = minHeight;
	m_scale_z = (maxHeight - minHeight + 1.0f);
	m_scale_x = info.m_heightfield.m_horizonalScale_x;
	m_scale_y = info.m_heightfield.m_horizonalScale_z;
	m_tileSize_x = info.m_heightfield.m_width;
	m_tileSize_y = info.m_heightfield.m_height;

	ndInt32 size = m_tileSize_x * m_tileSize_y;
	ndReal invScale = ndReal(256.0f * 256.0f - 1.0f)/ m_scale_z;

	for (ndInt32 i = 0; i < size; ++i)
	{
	//	m_heightfield.Push(dstHeight[i]);
		m_materials.Push(attributes[i]);
		ndFloat32 x = (dstHeight[i] - m_minHeight) * invScale;
		m_heightfield.Push(unsigned short(x));
	}
	
	delete heightfield;
}

void UNewtonCollisionLandscape::InitStaticMeshCompoment(const ALandscapeProxy* const landscapeProxy)
{
	check(landscapeProxy->GetLandscapeInfo());
	ULandscapeInfo* const info = landscapeProxy->GetLandscapeInfo();

	ndInt32 minSectionBaseX = 0x7fffffff;
	ndInt32 minSectionBaseY = 0x7fffffff;
	const USceneComponent* originComponent = nullptr;
	for (TMap<FIntPoint, ULandscapeHeightfieldCollisionComponent*>::TIterator it(info->XYtoCollisionComponentMap.CreateIterator()); it; ++it)
	{
		const ULandscapeHeightfieldCollisionComponent* const tile = it->Value;
		if ((tile->SectionBaseX < minSectionBaseX) && (tile->SectionBaseY < minSectionBaseY))
		{
			originComponent = tile;
			minSectionBaseX = tile->SectionBaseX;
			minSectionBaseY = tile->SectionBaseY;
		}
	}
	check(originComponent);
	SetTransform(originComponent);

	ndInt32 minX = 0x7fffffff;
	ndInt32 minY = 0x7fffffff;
	ndInt32 maxX = -0x7fffffff;
	ndInt32 maxY = -0x7fffffff;
	ndInt32 quadSize = -0x7fffffff;
	ndFloat32 scalex = ndFloat32(-1.0f);
	ndFloat32 scaley = ndFloat32(-1.0f);
	ndFloat32 scalez = ndFloat32(-1.0f);
	for (TMap<FIntPoint, ULandscapeHeightfieldCollisionComponent*>::TIterator it(info->XYtoCollisionComponentMap.CreateIterator()); it; ++it)
	{
		const ULandscapeHeightfieldCollisionComponent* const tile = it->Value;
		quadSize = ndMax(quadSize, tile->CollisionSizeQuads);
		minX = ndMin(minX, tile->SectionBaseX);
		minY = ndMin(minY, tile->SectionBaseY);
		maxX = ndMax(maxX, tile->SectionBaseX + quadSize);
		maxY = ndMax(maxY, tile->SectionBaseY + quadSize);

		const ULandscapeHeightfieldCollisionComponent::FHeightfieldGeometryRef* const heightfieldRef = tile->HeightfieldRef;
		const Chaos::FHeightField* const mapData = heightfieldRef->HeightfieldGeometry;
		const Chaos::FHeightField::FData<uint16>& elevationData = mapData->GeomData;

		const Chaos::FVec3 elevationScale(elevationData.Scale);
		scalex = ndMax (scalex, ndFloat32(elevationScale[0]));
		scaley = ndMax (scaley, ndFloat32(elevationScale[1]));
		scalez = ndMax(scalez, ndFloat32(elevationScale[2]));
	}
	scalex *= UNREAL_INV_UNIT_SYSTEM;
	scaley *= UNREAL_INV_UNIT_SYSTEM;
	scalez *= UNREAL_INV_UNIT_SYSTEM;

	ndArray<ndReal> elevations;
	ndInt32 numRows = (maxY - minY) + 1;
	ndInt32 numColum = (maxY - minY) + 1;
	m_materials.SetNum(numColum * numRows);
	m_heightfield.SetNum(numColum * numRows);
	elevations.SetCount(numColum * numRows);

	m_scale_x = scalex;
	m_scale_y = scaley;
	m_tileSize_y = numRows;
	m_tileSize_x = numColum;
	//m_scale_z = ndFloat32(1.0f);
	
	ndReal minHeight = ndReal(1.0e10f);
	ndReal maxHeight = ndReal(-1.0e10f);
	for (TMap<FIntPoint, ULandscapeHeightfieldCollisionComponent*>::TIterator it(info->XYtoCollisionComponentMap.CreateIterator()); it; ++it)
	{
		const ULandscapeHeightfieldCollisionComponent* const tile = it->Value;
		const ULandscapeHeightfieldCollisionComponent::FHeightfieldGeometryRef* const heightfieldRef = tile->HeightfieldRef;
		const Chaos::FHeightField* const mapData = heightfieldRef->HeightfieldGeometry;
		const Chaos::FHeightField::FData<uint16>& elevationData = mapData->GeomData;

		ndReal minValue = elevationData.MinValue;
		ndReal highScale = elevationData.HeightPerUnit;

		ndInt32 originX = tile->SectionBaseX - minX;
		ndInt32 originY = tile->SectionBaseY - minY;
		ndInt32 dstRow = numColum * originY + originX;
		ndInt32 srcRow = (ndInt32(elevationData.NumRows) - 1) * elevationData.NumCols;

		char* const attributes = &m_materials[dstRow];
		//float* const dstHeight = &m_heightfield[dstRow];
		ndReal* const dstHeight = &elevations[dstRow];
	
		ndInt32 rowOffset = 0;
		for (ndInt32 row = ndInt32(elevationData.NumRows) - 1; row >= 0; --row)
		{
			check((dstRow + rowOffset + elevationData.NumCols) <= m_heightfield.Num());
			for (ndInt32 colum = 0; colum < elevationData.NumCols; ++colum)
			{
				ndReal h = minValue + ndReal(elevationData.Heights[srcRow + colum]) * highScale;
				attributes[rowOffset + colum] = 0;

				h *= scalez;
				dstHeight[rowOffset + colum] = h;
				minHeight = ndMin(minHeight, h);
				maxHeight = ndMax(maxHeight, h);
			}
			rowOffset += numColum;
			srcRow -= elevationData.NumCols;
		}
	}
	m_minHeight = minHeight;
	m_scale_z = (maxHeight - minHeight + 1.0f);
	ndReal invScale = ndReal (256.0f * 256.0f - 1.0f) / m_scale_z;

	for (ndInt32 i = ndInt32(elevations.GetCount()) - 1; i >= 0; --i)
	{
		ndFloat32 x = (elevations[i] - m_minHeight) * invScale;
		m_heightfield[i] = unsigned short(x);
	}
}

long long UNewtonCollisionLandscape::CalculateHash() const
{
	long long hash = ndCRC64(ndShapeHeightfield::StaticClassName(), strlen(ndShapeHeightfield::StaticClassName()), 0);

	if (m_heightfield.Num())
	{
		const char* const materialBuffer = &m_materials[0];
		const unsigned short* const elevations = &m_heightfield[0];
		hash = ndCRC64(materialBuffer, m_materials.Num() * sizeof(char), hash);
		hash = ndCRC64(elevations, m_heightfield.Num() * sizeof(unsigned short), hash);
	}
	return hash;
}

void UNewtonCollisionLandscape::ApplyPropertyChanges()
{
	BuildNewtonShape();
	Super::ApplyPropertyChanges();
}

ndShape* UNewtonCollisionLandscape::CreateShape() const
{
	if (!m_heightfield.Num())
	{
		return new ndShapeNull();
	}
	
	ndShapeHeightfield* const shape = new ndShapeHeightfield(
		m_tileSize_x, m_tileSize_y,
		ndShapeHeightfield::m_normalDiagonals, m_scale_x, m_scale_y);
	
	const ndShapeInfo info(((ndShape*)shape)->GetShapeInfo());
	ndInt8* const attributes = info.m_heightfield.m_atributes;
	ndReal* const dstHeigh = info.m_heightfield.m_elevation;
	
	ndInt32 size = m_tileSize_x * m_tileSize_y;
	ndReal scale = m_scale_z / ndReal(256.0f * 256.0f - 1.0f);
	for (ndInt32 i = 0; i < size; ++i)
	{
		ndReal x = ndReal(ndFloat32(m_heightfield[i]) * scale + m_minHeight);
		dstHeigh[i] = x;
		attributes[i] = ndInt8(m_materials[i]);
	}
	shape->UpdateElevationMapAabb();
	return shape;
}

ndShapeInstance* UNewtonCollisionLandscape::CreateInstanceShape() const
{
	ndShapeInstance* const instance = new ndShapeInstance(m_shape);
	const FVector uScale(GetComponentTransform().GetScale3D());
	ndMatrix origin(ndGetIdentityMatrix());
	origin.m_posit.m_y = ndFloat32(ndFloat32(m_tileSize_y - 1) * m_scale_y * uScale.Y * UNREAL_INV_UNIT_SYSTEM);
	
	const ndMatrix alignment (ndPitchMatrix(ndPi * 0.5f));
	const ndMatrix tileMatrix(alignment * origin);
	instance->SetLocalMatrix(tileMatrix);
	
	ndMatrix scaleMatrix(ndGetIdentityMatrix());
	scaleMatrix[0][0] = ndFloat32(1.0f / uScale.X);
	scaleMatrix[1][1] = ndFloat32(1.0f / uScale.Y);
	scaleMatrix[2][2] = ndFloat32(1.0f / uScale.Z);
	const ndMatrix instanceMatrix(tileMatrix * scaleMatrix * tileMatrix.OrthoInverse());
	instance->SetGlobalScale(instanceMatrix);
	return instance;
}

ndShapeInstance* UNewtonCollisionLandscape::CreateBodyInstanceShape(const ndMatrix& bodyMatrix) const
{
	ndShapeInstance* const instance = new ndShapeInstance(m_shape);
	const FTransform transform(GetComponentTransform());
	const ndMatrix matrix (ToNewtonMatrix(transform));
	const FVector uScale(transform.GetScale3D());
	ndMatrix origin(ndGetIdentityMatrix());
	origin.m_posit.m_y = ndFloat32(ndFloat32(m_tileSize_y - 1) * m_scale_y * uScale.Y * UNREAL_INV_UNIT_SYSTEM);
	
	const ndMatrix alignment (ndPitchMatrix(ndPi * 0.5f));
	const ndMatrix tileMatrix(alignment * origin * matrix * bodyMatrix);
	instance->SetLocalMatrix(tileMatrix);
	
	return instance;
}
