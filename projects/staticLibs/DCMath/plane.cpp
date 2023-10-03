#include "plane.h"

namespace DC
{
	Plane::Plane()
	{
		normal.set(0.0f, 1.0f, 0.0f);
		distanceToOrigin = 0.0f;
	}

	Plane::~Plane()
	{

	}

	Plane::Plane(const Vector3f& Normal, float DistanceToOrigin)
	{
		normal = Normal;
		distanceToOrigin = DistanceToOrigin;
	}

	void Plane::set(const Vector3f& Normal, float DistanceToOrigin)
	{
		normal = Normal;
		distanceToOrigin = DistanceToOrigin;
	}

	Vector3f Plane::getNormal(void) const
	{
		return normal;
	}

	float Plane::getDistanceToOrigin(void) const
	{
		return distanceToOrigin;
	}

	float Plane::getDistanceFromPlane(const Vector3f& point) const
	{
		// If returned dot product is...
		// > 0		vectors are pointing roughly in same direction (upto 90 degrees apart)
		// = 0		vectors are perpendicular to one another (90 degrees)
		// < 0		vectors are pointing in opposite directions (+90 degrees to 180 degrees)
		return normal.getDot(point) - distanceToOrigin;
	}
}