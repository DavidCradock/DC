#pragma once

namespace DC
{
	// Vector class, using floats
	class Vector3f
	{
	public:
		// Constructor, sets all values to zero
		Vector3f();

		// Constructor, sets all values to the ones given
		Vector3f(float X, float Y, float Z);

		// Constructor, sets all values from the given Vector3f
		Vector3f(const Vector3f& vector);

		// Addition operator which adds two vectors together
		// Vector addition is commutative. (v1 + v2 == v2 + v1) is always true.
		// Triangle law of vector addition states that when two vectors are represented as two sides of the
		// triangle with the order of magnitude and direction, then the third side of the triangle represents
		// the magnitude and direction of the resultant vector.
		// Example:
		// Vector3f vOne(1.0f, 1.0f, 1.0f);
		// Vector3f vTwo(2.0f, 2.0f, 2.0f);
		// Vector3f vThree = vOne + vTwo;
		Vector3f operator +(const Vector3f& vector) const;

		// Addition operator which adds two vectors together
		// Vector addition is commutative. (v1 + v2 == v2 + v1) is always true.
		// Triangle law of vector addition states that when two vectors are represented as two sides of the
		// triangle with the order of magnitude and direction, then the third side of the triangle represents
		// the magnitude and direction of the resultant vector.
		// Example:
		// Vector3f vOne(1.0f, 1.0f, 1.0f);
		// Vector3f vThree(2.0f, 2.0f, 2.0f);
		// vThree += vOne;
		Vector3f& operator +=(const Vector3f& vector);

		// Subtraction operator 
		// Vector subtraction is not commutative. (v1 - v2 == v2 - v1) is only true when v1 is same as v2.
		// Example:
		// Vector3f vOne(1.0f, 1.0f, 1.0f);
		// Vector3f vTwo(2.0f, 2.0f, 2.0f);
		// Vector3f vMinusOne = vOne - vTwo;
		Vector3f operator -(const Vector3f& vector) const;

		// Subtraction operator 
		// Vector subtraction is not commutative. (v1 - v2 == v2 - v1) is only true when v1 is same as v2.
		// Example:
		// Vector3f vMinusOne(1.0f, 1.0f, 1.0f);
		// Vector3f vTwo(2.0f, 2.0f, 2.0f);
		// vMinusOne -= vTwo;
		Vector3f& operator -=(const Vector3f& vector);

		// Multiplication by scalar
		// Adjusts the magnitude of a vector, keeping the direction the same (Unless the scalar is negative, in which case the direction is reversed)
		// Example:
		// Vector3f vTwo(2.0f, 2.0f, 2.0f);
		// Vector3f vOne = vTwo * 0.5f;
		const Vector3f operator *(const float scalar) const;

		// Multiplication by scalar
		// Adjusts the magnitude of a vector, keeping the direction the same (Unless the scalar is negative, in which case the direction is reversed) 
		// Example:
		// CVector3f vOne(2.0f, 2.0f, 2.0f);
		// vOne *= 0.5f; 
		void operator *=(const float scalar);

		// Check for equality
		bool operator ==(const Vector3f& vector) const;

		// Check for inequality
		bool operator !=(const Vector3f& vector) const;

		// Sets each element to the values given
		void set(float X, float Y, float Z);

		// Sets each element within the vector's set to zero.
		// A zero vector is a special case. It does not have a direction, and it's magnitude is zero. 
		void setZero(void);

		// Returns true if the vector is a zero vector.
		bool isZero(void) const;

		// Negates the vector.
		// A negated vector has the same magnitude as before, however direction is reversed. 
		void negate(void);

		// Computes and returns the vector's magnitude
		// A vector's magnitude is also know as "the length" or norm of the vector.
		// Uses sqrtf() so it's quite expensive.
		float getMagnitude(void) const;

		// Normalises this vector so that it becomes a unit vector (Has a magnitude of 1)
		// Uses sqrtf() so it's quite expensive.
		// If the vector has zero length(magnitude), then it is not modified.
		void normalise(void);

		// Computes the dot product (also known as inner product) between this vector and the one given.
		// Dot product is commutative (v1.getDot(v2) == v2.getDot(v1)) == always true.
		// If returned dot product is...
		// > 0		vectors are pointing roughly in same direction (upto 90 degrees apart)
		// = 0		vectors are perpendicular to one another (90 degrees)
		// < 0		vectors are pointing in opposite directions (+90 degrees to 180 degrees)	
		float getDot(const Vector3f& vector) const;

		// Computes the cross product between this vector and the one given.
		// The cross product is a vector which is perpendicular to the two vectors it was calculated from. 
		Vector3f getCross(const Vector3f& vector) const;

		// Computes the angle (in radians) between two UNIT VECTORS (Length of 1)
		// No checking of vectors are unit length here. 
		float getAngle(const Vector3f& vector) const;

		// Computes distance between this vector and the one given, treating each vector as a point in 3D space.
		float getDistance(const Vector3f& vector) const;

		// Computes distance squared between this vector and the one given, treating each vector as a point in 3D space.
		// This is faster than the getDistance() method, as no expensive square root is used.
		// This is useful if you have many vectors/points and need to find the closest out of them all.
		// After finding the closest, then you can squareroot the return value from this method or use getDistance() to obtain the true distance. 
		float getDistanceSquared(const Vector3f& vector) const;

		// Returns x,y and z as an array of floats
		// Example:
		// float array[3];
		// vector.getAsArray(array);
		void getAsArray(float* pArray) const;

		// Multiplies this vector by the given scalar
		void multiply(float scalar);
		float x;
		float y;
		float z;
	};
}