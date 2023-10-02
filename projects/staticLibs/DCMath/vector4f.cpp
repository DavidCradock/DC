#include "vector4f.h"
#include <math.h>

namespace DC
{
	Vector4f::Vector4f()
	{
		x = y = z = w = 0.0f;
	}

	Vector4f::Vector4f(float fX, float fY, float fZ, float fW)
	{
		x = fX;
		y = fY;
		z = fZ;
		w = fW;
	}

	Vector4f Vector4f::operator +(const Vector4f& vec) const
	{
		return Vector4f(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
	}

	Vector4f& Vector4f::operator +=(const Vector4f& vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.w;
		return *this;
	}

	Vector4f Vector4f::operator -(const Vector4f& vec) const
	{
		return Vector4f(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
	}

	Vector4f& Vector4f::operator -=(const Vector4f& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		w -= vec.w;
		return *this;
	}

	const Vector4f Vector4f::operator*(const float f) const
	{
		return Vector4f(x * f, y * f, z * f, w * f);
	}

	void Vector4f::operator*=(const float f)
	{
		x = x * f;
		y = y * f;
		z = z * f;
		w = w * f;
	}

	const Vector4f Vector4f::operator *(const Vector4f& v) const
	{
		Vector4f result;
		result.x = x * v.x;
		result.y = y * v.y;
		result.z = z * v.z;
		result.w = w * v.w;
		return result;
	}

	bool Vector4f::operator ==(const Vector4f& vec) const
	{
		return x == vec.x && y == vec.y && z == vec.z && w == vec.w;
	}

	bool Vector4f::operator !=(const Vector4f& vec) const
	{
		return x != vec.x || y != vec.y || z != vec.z || w != vec.w;
	}

	void Vector4f::set(float fX, float fY, float fZ, float fW)
	{
		x = fX;
		y = fY;
		z = fZ;
		w = fW;
	}

	void Vector4f::setZero(void)
	{
		x = y = z = w = 0.0f;
	}

	bool Vector4f::isZero(void) const
	{
		return x == 0 && y == 0 && z == 0 && w == 0;
	}

	void Vector4f::negate(void)
	{
		x = -x;
		y = -y;
		z = -z;
		w = -w;
	}

	float Vector4f::getMagnitude(void) const
	{
		float fMagnitude = x * x;
		fMagnitude += y * y;
		fMagnitude += z * z;
		fMagnitude += w * w;
		fMagnitude = sqrtf(fMagnitude);
		return fMagnitude;
	}

	void Vector4f::normalise(void)
	{
		// Compute magnitude aka length
		float fMagnitude = x * x;
		fMagnitude += y * y;
		fMagnitude += z * z;
		fMagnitude += w * w;
		fMagnitude = sqrtf(fMagnitude);
		if (fMagnitude == 0.0f)	// Prevent divide by zero
			return;
		float fReciprocal = 1.0f / fMagnitude;
		x *= fReciprocal;
		y *= fReciprocal;
		z *= fReciprocal;
		w *= fReciprocal;
	}

	void Vector4f::getAsArray(float* pArray) const
	{
		pArray[0] = x;
		pArray[1] = y;
		pArray[2] = z;
		pArray[3] = w;
	}

	void Vector4f::multiply(float f)
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;
	}
}