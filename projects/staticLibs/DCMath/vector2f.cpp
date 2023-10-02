#include "vector2f.h"
#include "../DCCommon/error.h"

namespace DC
{
	Vector2f::Vector2f()
	{
		x = y = 0.0f;
	}

	Vector2f::Vector2f(float X, float Y)
	{
		x = X;
		y = Y;
	}

	Vector2f Vector2f::operator +(const Vector2f& vector) const
	{
		return Vector2f(x + vector.x, y + vector.y);
	}

	Vector2f& Vector2f::operator +=(const Vector2f& vector)
	{
		x += vector.x;
		y += vector.y;
		return *this;
	}

	Vector2f Vector2f::operator -(const Vector2f& vector) const
	{
		return Vector2f(x - vector.x, y - vector.y);
	}

	Vector2f& Vector2f::operator -=(const Vector2f& vector)
	{
		x -= vector.x;
		y -= vector.y;
		return *this;
	}

	const Vector2f Vector2f::operator*(const float scalar) const
	{
		return Vector2f(x * scalar, y * scalar);
	}

	void Vector2f::operator*=(const float scalar)
	{
		x = x * scalar;
		y = y * scalar;
	}

	bool Vector2f::operator ==(const Vector2f& vector) const
	{
		return x == vector.x && y == vector.y;
	}

	bool Vector2f::operator !=(const Vector2f& vector) const
	{
		return x != vector.x || y != vector.y;
	}

	void Vector2f::divide(float scalar)
	{
		ErrorIfTrue(scalar < 0.0000001f, L"Vector2f::divide() given scalar of zero which creates a divide by zero.");
		float f1Over = 1.0f / scalar;
		x *= f1Over;
		y *= f1Over;
	}

	void Vector2f::multiply(float scalar)
	{
		x *= scalar;
		y *= scalar;
	}

	void Vector2f::set(float X, float Y)
	{
		x = X;
		y = Y;
	}

	void Vector2f::setZero(void)
	{
		x = y = 0.0f;
	}

	bool Vector2f::isZero(void) const
	{
		return x == 0 && y == 0;
	}

	void Vector2f::negate(void)
	{
		x = -x;
		y = -y;
	}

	float Vector2f::getMagnitude(void) const
	{
		float fMagnitude = x * x;
		fMagnitude += y * y;
		fMagnitude = sqrtf(fMagnitude);
		return fMagnitude;
	}

	void Vector2f::normalise(void)
	{
		// Compute magnitude aka length
		float fMagnitude = x * x;
		fMagnitude += y * y;
		fMagnitude = sqrtf(fMagnitude);
		if (fMagnitude == 0.0f)	// Prevent divide by zero
			return;
		float fReciprocal = float(1.0f / fMagnitude);
		x *= fReciprocal;
		y *= fReciprocal;
	}

	float Vector2f::getDistance(const Vector2f& vector) const
	{
		float fx = x - vector.x;
		float fy = y - vector.y;
		return sqrtf(fx * fx + fy * fy);
	}

	float Vector2f::getDistanceSquared(const Vector2f& vector) const
	{
		float fx = x - vector.x;
		float fy = y - vector.y;
		return fx * fx + fy * fy;
	}

	float Vector2f::getSign(const Vector2f& vOther) const
	{
		if (y * vOther.x > x * vOther.y)
		{
			return 1.0f;
		}
		else
		{
			return -1.0f;
		}
	}

	float Vector2f::getDot(const Vector2f& vector) const
	{
		return x * vector.x + y * vector.y;
	}

	float Vector2f::getAngleRadiansUnsigned(const Vector2f& vOther) const
	{
		float fDot = getDot(vOther);
		if (fDot < -1.0f)
			fDot = 1.0f;
		else if (fDot > 1.0f)
			fDot = 1.0f;
		return acosf(fDot);
	}

	float Vector2f::getAngleRadiansSigned(const Vector2f& vOther) const
	{
		Vector2f vOtherNorm = vOther;
		vOtherNorm.normalise();

		float fDot = getDot(vOtherNorm);
		// Make sure fDot is valid
		if (fDot < -1.0f)
			fDot = -1.0f;
		else if (fDot > 1.0f)
			fDot = 1.0f;
		float fAngleRadians = acosf(fDot);
		fAngleRadians *= getSign(vOtherNorm);
		return fAngleRadians;
	}

	float Vector2f::getAngleDegreesSigned(const Vector2f& vOther) const
	{
		// Normalise this vector
		Vector2f vThisNorm(x, y);
		vThisNorm.normalise();

		// Normalise other vector
		Vector2f vOtherNorm(vOther.x, vOther.y);
		vOtherNorm.normalise();

		// Compute dot
		float fTmp = vThisNorm.x * vOtherNorm.x + vThisNorm.y * vOtherNorm.y;

		// Convert to unsigned radians
		// make sure fTmp is valid
		if (fTmp < -1.0f)
			fTmp = -1.0f;
		else if (fTmp > 1.0f)
			fTmp = 1.0f;
		fTmp = (float)acosf(fTmp);

		// Convert to signed radians
		if (vThisNorm.y * vOtherNorm.x < vThisNorm.x * vOtherNorm.y)
			fTmp *= -1.0f;

		// Convert to degrees
		// NOTE: 180.0f / Pi is used to convert radians to degrees
		// and it is a value of : 57.2957795
		return fTmp * 57.2957795f;
	}

	float Vector2f::getAngleDegrees360(void) const
	{
		// Normalise this vector
		Vector2f vThisNorm(x, y);
		vThisNorm.normalise();

		Vector2f vNorth(0.0f, 1.0f);

		// Compute dot
		float fTmp = vThisNorm.x * vNorth.x + vThisNorm.y * vNorth.y;

		// Convert to unsigned radians
		// First, make sure fTmp is valid
		if (fTmp < -1.0f)
			fTmp = -1.0f;
		else if (fTmp > 1.0f)
			fTmp = 1.0f;
		fTmp = (float)acosf(fTmp);

		// Convert to degrees
		// NOTE: 180.0f / Pi is used to convert radians to degrees
		// and it is a value of : 57.2957795
		fTmp *= 57.2957795f;

		if (vThisNorm.y * vNorth.x < vThisNorm.x * vNorth.y)
		{
			return fTmp;
		}
		return 180.0f + (180.0f - fTmp);
	}

	void Vector2f::rotate(float degrees)
	{
		float fRad = degrees * -0.01745329251994329576923690768489f;
		float fOldX = x;
		float fOldY = y;
		float fSin, fCos;
		fSin = sinf(fRad);
		fCos = cosf(fRad);
		x = fOldX * fCos - fOldY * fSin;
		y = fOldX * fSin + fOldY * fCos;
	}

	void Vector2f::limitLength(float maxLength)
	{
		float fLen = getMagnitude();
		if (fLen <= maxLength)
			return;
		divide(fLen);	// Normalise
		multiply(maxLength);
	}

	Vector2f Vector2f::getPerpendicular(void) const
	{
		//Vector2f vPerpendicular(y, -x);
		Vector2f vPerpendicular(-y, x);
		return vPerpendicular;
	}

	Vector2f Vector2f::interpolate(const Vector2f& v1, const Vector2f& v2, float f) const
	{
		if (f < 0.0f)
			return v2;
		else if (f > 1.0f)
			return v1;
		Vector2f vA = v1;
		vA.multiply(f);
		Vector2f vB = v2;
		vB.multiply(1.0f - f);
		return vA + vB;
	}

	void Vector2f::getAsArray(float* pArray) const
	{
		pArray[0] = x;
		pArray[1] = y;
	}
}