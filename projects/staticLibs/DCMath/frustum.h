#pragma once
#include "plane.h"
#include "matrix.h"
#include "AABB.h"

namespace DC
{
	// A frustum is defined by six planes.
	// It can be used to determine whether various geometric shapes and points are inside of it
	class Frustum
	{
	public:
		// Default constructor, does nothing
		Frustum();

		// Sets this frustum's planes from the given matrices which are from a camera's view and projection matrices.
		void computeFromViewProjection(const Matrix& cameraViewMatrix, const Matrix& cameraProjectionMatrix);

		// Returns true if the given position is inside the frustum, else false.
		bool isPointInside(const Vector3f& position) const;

		// Returns true if the given Axis Aligned Bounding Box is intersecting this frustum, else false.
		bool isAABBIntersecting(const AABB& aabb) const;

		Plane near;
		Plane far;
		Plane left;
		Plane right;
		Plane top;
		Plane bottom;
	};
}