#include "vector3f.h"
#include <cmath>

namespace DC
{
	Vector3f::Vector3f()
	{
		x = y = z = 0.0f;
	}

	Vector3f::Vector3f(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}

	Vector3f::Vector3f(const Vector3f& vector)
	{
		x = vector.x;
		y = vector.y;
		z = vector.z;
	}

	Vector3f Vector3f::operator +(const Vector3f& vector) const
	{
		return Vector3f(x + vector.x, y + vector.y, z + vector.z);
	}

	Vector3f& Vector3f::operator +=(const Vector3f& vector)
	{
		x += vector.x;
		y += vector.y;
		z += vector.z;
		return *this;
	}

	Vector3f Vector3f::operator -(const Vector3f& vector) const
	{
		return Vector3f(x - vector.x, y - vector.y, z - vector.z);
	}

	Vector3f& Vector3f::operator -=(const Vector3f& vector)
	{
		x -= vector.x;
		y -= vector.y;
		z -= vector.z;
		return *this;
	}

	const Vector3f Vector3f::operator*(const float scalar) const
	{
		return Vector3f(x * scalar, y * scalar, z * scalar);
	}

	void Vector3f::operator*=(const float scalar)
	{
		x = x * scalar;
		y = y * scalar;
		z = z * scalar;
	}

	bool Vector3f::operator ==(const Vector3f& vector) const
	{
		return x == vector.x && y == vector.y && z == vector.z;
	}

	bool Vector3f::operator !=(const Vector3f& vector) const
	{
		return x != vector.x || y != vector.y || z != vector.z;
	}

	void Vector3f::set(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}

	void Vector3f::setZero(void)
	{
		x = y = z = 0.0f;
	}

	bool Vector3f::isZero(void) const
	{
		return x == 0 && y == 0 && z == 0;
	}

	void Vector3f::negate(void)
	{
		x = -x;
		y = -y;
		z = -z;
	}

	float Vector3f::getMagnitude(void) const
	{
		float fMagnitude = x * x;
		fMagnitude += y * y;
		fMagnitude += z * z;
		fMagnitude = sqrtf(fMagnitude);
		return fMagnitude;
	}

	void Vector3f::normalise(void)
	{
		// Compute magnitude aka length
		float fMagnitude = x * x;
		fMagnitude += y * y;
		fMagnitude += z * z;
		fMagnitude = sqrtf(fMagnitude);
		if (fMagnitude == 0.0f)	// Prevent divide by zero
			return;
		float fReciprocal = 1.0f / fMagnitude;
		x *= fReciprocal;
		y *= fReciprocal;
		z *= fReciprocal;
	}

	float Vector3f::getDot(const Vector3f& vector) const
	{
		return x * vector.x + y * vector.y + z * vector.z;
	}

	Vector3f Vector3f::getCross(const Vector3f& vector) const
	{
		Vector3f vCross;
		vCross.x = y * vector.z - z * vector.y;
		vCross.y = z * vector.x - x * vector.z;
		vCross.z = x * vector.y - y * vector.x;
		return vCross;
	}

	float Vector3f::getAngle(const Vector3f& vector) const
	{
		return acosf(getDot(vector));
	}

	float Vector3f::getDistance(const Vector3f& vector) const
	{
		float fx = x - vector.x;
		float fy = y - vector.y;
		float fz = z - vector.z;
		return sqrtf(fx * fx + fy * fy + fz * fz);
	}

	float Vector3f::getDistanceSquared(const Vector3f& vector) const
	{
		float fx = x - vector.x;
		float fy = y - vector.y;
		float fz = z - vector.z;
		return fx * fx + fy * fy + fz * fz;
	}

	void Vector3f::getAsArray(float* pArray) const
	{
		pArray[0] = x;
		pArray[1] = y;
		pArray[2] = z;
	}

	void Vector3f::multiply(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
	}
}