#pragma once
#include "vector3f.h"

namespace DC
{
	// An Axis Aligned Bounding Box
	// An AABB is simply a box with it's axis aligned to the world.
	// It is similar to the Rect class, but in 3D and is used for culling with the OctTree class
	class AABB
	{
	public:
		// Default constructor, sets to a 1x1x1 cube centered at origin
		AABB();

		// Sets box with given values
		AABB(const Vector3f& Min, const Vector3f& Max);

		// Sets box position and dims with given values
		void setMinMax(const Vector3f& Min, const Vector3f& Max);

		// Sets box position and dimes with given values
		void setPosDims(const Vector3f& position, const Vector3f& dimensions);

		// Sets position of box, leaving dimensions alone
		void setPosition(const Vector3f& position);

		// Sets dimensions of box, leaving position alone
		void setDimensions(const Vector3f& dimensions);

		// Returns dimensions of the box
		Vector3f getDimensions(void) const;

		// Returns half dimensions of the box
		Vector3f getHalfDimensions(void) const;

		// Returns position of the box
		Vector3f getPosition(void) const;

		// Returns true if the given point is inside of this box
		bool getPointIsInside(const Vector3f& point) const;

		// Returns true if the given AABB intersects with this one
		bool getAABBintersects(const AABB& aabb) const;

		// Resizes the area so that it's dimensions are multiplied by the given float
		// Example:
		// If min = (0,0) and max = (10,10) (total dims of (10,10)) and the given float is 2 then the new values would be
		// min = (-5,-5) and max = (15,15) (total dims of (20, 20))
		void resizeArea(float multiplier);

		// Returns _mvMin
		Vector3f getMin(void) const;

		// Returns _mvMax
		Vector3f getMax(void) const;
	private:
		Vector3f min;	// Minimum positions of the corners of the box
		Vector3f max;	// Maximum positions of the corners of the box
	};
}