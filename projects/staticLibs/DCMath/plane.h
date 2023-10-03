#pragma once
#include "vector3f.h"

namespace DC
{
	// A representation of a plane in 3D space.
	// A plane is defined by two things.
	// 1. Distance to the origin
	// 2. A normal vector
	class Plane
	{
	public:
		Plane();
		~Plane();

		Plane(const Vector3f& Normal, float DistanceToOrigin);

		// Sets plane to given values
		void set(const Vector3f& Normal, float DistanceToOrigin);

		// Returns normal of plane
		Vector3f getNormal(void) const;

		// Returns distance from origin
		float getDistanceToOrigin(void) const;

		// Uses to dot product to compute the distance of a point from the plane.
		// If the given point is on the side of the plane it's normal is pointing,
		// a positive value is returned, else negative.
		float getDistanceFromPlane(const Vector3f& point) const;

	private:
		Vector3f normal;
		float distanceToOrigin;
	};
}