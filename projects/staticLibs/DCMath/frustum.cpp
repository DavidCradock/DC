#include "frustum.h"
#include "utilities.h"

namespace DC
{
	Frustum::Frustum()
	{

	}

	void Frustum::computeFromViewProjection(const Matrix& cameraViewMatrix, const Matrix& cameraProjectionMatrix)
	{
		/* Taken from Bard
		// Get the view and projection matrices.
		glm::mat4 viewMatrix = glm::lookAt(eye, center, up);
		glm::mat4 projectionMatrix = glm::perspective(fov, aspect, near, far);

		// Compute the frustum planes.
		std::vector<glm::Plane> frustumPlanes;
		frustumPlanes.push_back(glm::Plane(viewMatrix * projectionMatrix * glm::vec4(1, 0, 0, 0), -1));
		frustumPlanes.push_back(glm::Plane(viewMatrix * projectionMatrix * glm::vec4(-1, 0, 0, 0), 1));
		frustumPlanes.push_back(glm::Plane(viewMatrix * projectionMatrix * glm::vec4(0, 1, 0, 0), -1));
		frustumPlanes.push_back(glm::Plane(viewMatrix * projectionMatrix * glm::vec4(0, -1, 0, 0), 1));
		frustumPlanes.push_back(glm::Plane(viewMatrix * projectionMatrix * glm::vec4(0, 0, 1, 0), -1));
		frustumPlanes.push_back(glm::Plane(viewMatrix * projectionMatrix * glm::vec4(0, 0, -1, 0), 1));

		// Normalize the frustum planes.
		for (int i = 0; i < 6; i++) {
		  frustumPlanes[i].normalize();
		}
*/

		// Combine the view and projection matrices
		// Then create vectors with offsets for the six planes
		// Then multiply them by the combined matrices
		// and finally set the distance to origins accordingly
		Matrix matrixViewProj = cameraViewMatrix * cameraProjectionMatrix;
		Vector3f v;
		float fMag;

		v.set(1.0f, 0.0f, 0.0f);
		v = matrixViewProj.multiply(v);
		fMag = v.getMagnitude();
		v.normalise();
		right.set(v, -fMag);

		v.set(-1.0f,  0.0f,  0.0f);
		v = matrixViewProj.multiply(v);
		fMag = v.getMagnitude();
		v.normalise();
		left.set(v, fMag);

		v.set( 0.0f,  1.0f,  0.0f);
		v = matrixViewProj.multiply(v);
		fMag = v.getMagnitude();
		v.normalise();
		top.set(v, -fMag);

		v.set( 0.0f, -1.0f,  0.0f);
		v = matrixViewProj.multiply(v);
		fMag = v.getMagnitude();
		v.normalise();
		bottom.set(v, fMag);

		v.set( 0.0f,  0.0f,  1.0f);
		v = matrixViewProj.multiply(v);
		fMag = v.getMagnitude();
		v.normalise();
		near.set(v, -fMag);

		v.set( 0.0f,  0.0f, -1.0f);
		v = matrixViewProj.multiply(v);
		fMag = v.getMagnitude();
		v.normalise();
		far.set(v, fMag);
	}

	bool Frustum::isPointInside(const Vector3f& position) const
	{
		/* Taken from Bard
		bool isPointInsideFrustum(glm::vec3 point, std::vector<glm::Plane> frustumPlanes)
		{
			for (int i = 0; i < 6; i++)
			{
				if (frustumPlanes[i].dot(point) < 0)
				{
					return false;
				}
			}
			return true;
		}
		This code takes a point and a vector of frustum planes as input.
		It then iterates through the frustum planes and checks if the point is on the "wrong" side of any of the planes.
		If the point is on the wrong side of any of the planes, then it is not inside the frustum. Otherwise, the point is inside the frustum.
		
		The dot() function is used to calculate the dot product of the point and the plane normal.
		The dot product is a measure of the angle between two vectors.
		If the dot product is negative, then the point is on the "wrong" side of the plane.
		*/

		// Compute dot product between position and each plane's normal...
		// If returned dot product is...
		// > 0		vectors are pointing roughly in same direction (upto 90 degrees apart)
		// = 0		vectors are perpendicular to one another (90 degrees)
		// < 0		vectors are pointing in opposite directions (+90 degrees to 180 degrees)

		Vector3f v;
		v = near.getNormal() * abs(near.getDistanceToOrigin());
		if (v.getDot(position) < 0)
			return false;
		v = far.getNormal() * abs(far.getDistanceToOrigin());
		if (v.getDot(position) < 0)
			return false;
		v = left.getNormal() * abs(left.getDistanceToOrigin());
		if (v.getDot(position) < 0)
			return false;
		v = right.getNormal() * abs(right.getDistanceToOrigin());
		if (v.getDot(position) < 0)
			return false;
		v = top.getNormal() * abs(top.getDistanceToOrigin());
		if (v.getDot(position) < 0)
			return false;
		v = bottom.getNormal() * abs(bottom.getDistanceToOrigin());
		if (v.getDot(position) < 0)
			return false;
		return true;
	}

	bool Frustum::isAABBIntersecting(const AABB& aabb) const
	{
		/* Taken from Bard
		bool isAABBIntersectingFrustum(glm::vec3 min, glm::vec3 max, std::vector<glm::Plane> frustumPlanes)
		{
			for (int i = 0; i < 6; i++)
			{
				// Check if any of the AABB's vertices are on the wrong side of the plane.
				if (frustumPlanes[i].dot(min) < 0 || frustumPlanes[i].dot(max) > 0)
				{
					return false;
				}
			}
			return true;
		}
		This code takes an AABB and a vector of frustum planes as input.
		It then iterates through the frustum planes and checks if any of the AABB's vertices are on the "wrong" side of any of the planes.
		If any of the AABB's vertices are on the wrong side of any of the planes, then the AABB is not intersecting the frustum.
		Otherwise, the AABB is intersecting the frustum.

		The dot() function is used to calculate the dot product of the AABB's vertices and the plane normals.
		The dot product is a measure of the angle between two vectors. If the dot product is negative, then the vertex is on the "wrong" side of the plane.
		*/

		Vector3f v;
		Vector3f vMin = aabb.getMin();
		Vector3f vMax = aabb.getMax();

		v = near.getNormal() * abs(near.getDistanceToOrigin());
		if (v.getDot(vMin) < 0 || v.getDot(vMax) > 0)
			return false;
		v = far.getNormal() * abs(far.getDistanceToOrigin());
		if (v.getDot(vMin) < 0 || v.getDot(vMax) > 0)
			return false;
		v = left.getNormal() * abs(left.getDistanceToOrigin());
		if (v.getDot(vMin) < 0 || v.getDot(vMax) > 0)
			return false;
		v = right.getNormal() * abs(right.getDistanceToOrigin());
		if (v.getDot(vMin) < 0 || v.getDot(vMax) > 0)
			return false;
		v = top.getNormal() * abs(top.getDistanceToOrigin());
		if (v.getDot(vMin) < 0 || v.getDot(vMax) > 0)
			return false;
		v = bottom.getNormal() * abs(bottom.getDistanceToOrigin());
		if (v.getDot(vMin) < 0 || v.getDot(vMax) > 0)
			return false;
		return true;
	}
}