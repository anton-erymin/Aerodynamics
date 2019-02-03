#pragma once

#include "export.h"

#include "lpMath.h"

class DLL_EXPORT lpAxisAlignedBoundingBox
{
public:
	lpVec3 m_center;
	lpVec3 m_radius;

	lpAxisAlignedBoundingBox(void);
	lpAxisAlignedBoundingBox(const lpVec3 &radius);
	~lpAxisAlignedBoundingBox(void);

	bool overlapsAABB(const lpAxisAlignedBoundingBox &other);
	static void transformAABB(const lpAxisAlignedBoundingBox &aabb, lpAxisAlignedBoundingBox &newAabb, const lpMat3 &transform, const lpVec3 &translation);
};
