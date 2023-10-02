#pragma once
#include "vector3f.h"

namespace DC
{
	class Matrix;

	class Quaternion
	{
	public:
		// Default constructor, sets quaternion to identity (0, 0, 0, 1)
		Quaternion();

		// Sets quaternion from given values
		Quaternion(float X, float Y, float Z, float W);

		// Sets quaternion from given quaternion
		Quaternion(const Quaternion& quaternion);

		// Access each element
		float& operator[] (const int index);

		// Multiply this quaternion by another and return result
		const Quaternion operator * (const Quaternion& quaternion) const;

		// Multiply this quaternion by another
		const void operator *= (const Quaternion& quaternion);

		// Add two quaternions, return result
		const Quaternion operator+(const Quaternion& quaternion) const;

		// Subtract two quaternions, return result
		const Quaternion operator-(const Quaternion& quaternion) const;

		// Multiply by scalar, return result
		const Quaternion operator*(const float scalar) const;

		// Divide by scalar, return result
		const Quaternion operator/(const float scalar) const;

		// Multiply a vector, return result
		Vector3f operator *(const Vector3f& vector) const;

		// Set this quaternion to the one on the right of the operator
		const void operator = (const Quaternion& quaternion);
		
		// Comparison operator
		const bool operator == (const Quaternion& quaternion) const;
		
		// Comparison operator
		const bool operator != (const Quaternion& quaternion) const;

		// Sets quaternion to identity (0, 0, 0, 1)
		void setIdentity(void);

		// Gets magnitude of the quaternion
		float getMagnitude(void) const;

		// Normalizes the quaternion
		void normalise(void);

		// Returns a quaternion with this quaternion's xyz elements inverted.
		Quaternion getConjugate(void) const;

		// Returns a quaternion with this quaternion's elements inverted.
		Quaternion getNegative(void) const;

		// Sets this quaternion from an axis and an angle
		// This does not normalize the given axis
		void setFromAxisAngle(const Vector3f& axis, float angleRadians);

		// Returns this quaternion's axis
		// TODO Check this
		Vector3f getAxis(void) const;

		// Returns this quaternion's angle
		float getAngle(void) const;

		// Sets this quaternion from Euler angles
		void setFromEuler(float angleRadiansX, float angleRadiansY, float angleRadiansZ);

		// Returns a vector containing the amount of rotation around each local axis
		Vector3f getEuler(void) const;

		// Sets quaternion from given CMatrix
		void setFromMatrix(const Matrix& matrix);

		// Rotate this quaternion by another
		void rotate(const Quaternion& quaternion);

		// Return a quaternion which is interpolated between this quat and the one given using smooth SLERP
		Quaternion getSLERP(const Quaternion& quaternion, float interval) const;

		// Return a quaternion which is interpolated between this quat and the one given using linear interpolation
		Quaternion getLERP(const Quaternion& quaternion, float interval) const;

		float q[4];	// x, y, z and w
	};
}