#include "AABB.h"

namespace DC
{
	AABB::AABB()
	{
		min.set(-0.5f, -0.5f, -0.5f);
		max.set(0.5f, 0.5f, 0.5f);
	}

	AABB::AABB(const Vector3f& minParam, const Vector3f& maxPARAM)
	{
		min = minParam;
		max = maxPARAM;
	}

	void AABB::setMinMax(const Vector3f& minParam, const Vector3f& maxPARAM)
	{
		min = minParam;
		max = maxPARAM;
	}

	void AABB::setPosDims(const Vector3f& position, const Vector3f& dimensions)
	{
		Vector3f vHalfDims = dimensions * 0.5f;
		min = position - vHalfDims;
		max = position + vHalfDims;
	}

	void AABB::setPosition(const Vector3f& position)
	{
		Vector3f vHalfDims = max - min;
		vHalfDims *= 0.5f;

		min = position - vHalfDims;
		max = position + vHalfDims;
	}

	void AABB::setDimensions(const Vector3f& dimensions)
	{
		Vector3f vOldHalfDims = max - min;
		vOldHalfDims *= 0.5f;
		Vector3f vPos = min + vOldHalfDims;

		Vector3f vHalfDims = dimensions * 0.5f;
		min = vPos - vHalfDims;
		max = vPos + vHalfDims;
	}

	Vector3f AABB::getDimensions(void) const
	{
		return max - min;
	}

	Vector3f AABB::getHalfDimensions(void) const
	{
		Vector3f vHalfDims = max - min;
		vHalfDims *= 0.5f;
		return vHalfDims;
	}

	Vector3f AABB::getPosition(void) const
	{
		Vector3f vHalfDims = max - min;
		vHalfDims *= 0.5f;

		Vector3f vPos = min + vHalfDims;
		return vPos;
	}

	bool AABB::getPointIsInside(const Vector3f& point) const
	{
		if (point.x < min.x)
			return false;
		if (point.x > max.x)
			return false;
		if (point.y < min.y)
			return false;
		if (point.y > max.y)
			return false;
		if (point.z < min.z)
			return false;
		if (point.z > max.z)
			return false;
		return true;
	}

	bool AABB::getAABBintersects(const AABB& aabb) const
	{
		if (min.x > aabb.max.x)	// This is to right of other
			return false;
		if (min.y > aabb.max.y)	// This is above other
			return false;
		if (min.z > aabb.max.z)	// This is behind other
			return false;
		if (max.x < aabb.min.x)	// This is to left of other
			return false;
		if (max.y < aabb.min.y)	// This is below other
			return false;
		if (max.z < aabb.min.z)	// This if infront of other
			return false;
		return true;
	}

	void AABB::resizeArea(float multiplier)
	{
		// Get current dimensions of the AABB
		Vector3f vCurrentDims = max - min;

		// Compute new dimensions
		Vector3f vNewDims = vCurrentDims * multiplier;

		// Compute amount to offset _mvMin and _mvMax by
		Vector3f vOffset = vNewDims * 0.25f;

		// Offset _mvMin and _mvMax
		min -= vOffset;
		max += vOffset;
	}

	Vector3f AABB::getMin(void) const
	{
		return min;
	}

	Vector3f AABB::getMax(void) const
	{
		return max;
	}
}