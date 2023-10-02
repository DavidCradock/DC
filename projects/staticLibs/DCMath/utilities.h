#pragma once
#include <cmath>
#include <math.h>
#include <limits>
#include <stdlib.h>

namespace DC
{
	// Due to floating point errors, we use this to determine whether
	// two floating points are roughly equal
	const float kfEpsilon = 0.0000001f;
	inline bool areFloatsEqual(const float& A, const float& B)
	{
		return (fabsf(A - B) < kfEpsilon);
	}

	// Due to floating point errors with doubles, we use this to determine whether
	// two doubles points are roughly equal
	const double kdEpsilon = 0.000000000000001f;
	inline bool areDoublesEqual(const double& A, const double& B)
	{
		return (abs(A - B) < kdEpsilon);
	}

	// Interpolate between two given values, given a position value between 0.0f and 1.0f
	inline float interpolate(float valueA, float valueB, float position)
	{
		float diff = valueA - valueB;
		diff *= -1.0f;
		return valueA + (diff * position);
	}

	// Interpolate between two given values, given a position value between 0.0 and 1.0
	inline double interpolate(double valueA, double valueB, double position)
	{
		double diff = valueA - valueB;
		diff *= -1.0;
		return valueA + (diff * position);
	}

	// Sigmoid function
	// Given a value, takes that and scales it so that the values lie upon an S curve.
	// response curve specified the shape of the curve.
	// Higher values, flatten the curve, lower ones tighten it.
	// A default value of 1 is usually used.
	// Do not set it to zero, this'll create a divide by zero error.
	inline float sigmoid(float value, float response = 1.0f)
	{
		// The sigmoid function looks like this...
		// 1 / 1 + (e to the power of(-value / response));
		// Where e is a math constant, which is roughly 2.7183
		// We use exp() to compute the exponential value of (-value / response)
		return (1.0f / (1.0f + exp(-value / response)));
	}

	// Sigmoid function
	// Given a value, takes that and scales it so that the values lie upon an S curve.
	// response curve specified the shape of the curve.
	// Higher values, flatten the curve, lower ones tighten it.
	// A default value of 1 is usually used.
	// Do not set it to zero, this'll create a divide by zero error.
	inline double sigmoid(double value, double response = 1)
	{
		// The sigmoid function looks like this...
		// 1 / 1 + (e to the power of(-value / response));
		// Where e is a math constant, which is roughly 2.7183
		// We use exp() to compute the exponential value of (-value / response)
		return (1 / (1 + exp(-value / response)));
	}

	// Sigmoid function
	// Given a value, takes that and scales it so that the values lie upon an S curve.
	// This does not have the ability to set the shape of the S curve, but it is 
	// slightly faster as there is no divide.
	inline float sigmoid(float value)
	{
		return 1.0f / (1.0f + exp(-value));
	}

	// Sigmoid function
	// Given a value, takes that and scales it so that the values lie upon an S curve.
	// This does not have the ability to set the shape of the S curve, but it is 
	// slightly faster as there is no divide.
	inline double sigmoid(double value)
	{
		return 1 / (1 + exp(-value));
	}


	const float kfPi = 3.14159265f;				// Pi constant
	const float kf2Pi = kfPi * 2.0f;			// 2 * Pi (Radians in a circle)
	const float kfPiOver2 = kfPi / 2.0f;		// Pi divided by 2 (90 degrees in radians)
	const float kf1OverPi = 1.0f / kfPi;
	const float kf1Over2Pi = 1.0f / kf2Pi;
	const float kfPiOver180 = kfPi / 180.0f;
	const float kf180OverPi = 180.0f / kfPi;

	const double kdPi = 3.141592653589793;		// Pi constant
	const double kd2Pi = kdPi * 2.0;			// 2 * Pi (Radians in a circle)
	const double kdPiOver2 = kdPi / 2.0;		// Pi divided by 2 (90 degrees in radians)
	const double kd1OverPi = 1.0 / kdPi;
	const double kd1Over2Pi = 1.0 / kd2Pi;
	const double kdPiOver180 = kdPi / 180.0;
	const double kd180OverPi = 180.0 / kdPi;

	// Maximum values for basic data types
	const double  kMaxDouble = (std::numeric_limits<double>::max)();
	const double  kMinDouble = (std::numeric_limits<double>::min)();
	const float   kMaxFloat = (std::numeric_limits<float>::max)();
	const float   kMinFloat = (std::numeric_limits<float>::min)();
	const int     kMaxInt = (std::numeric_limits<int>::max)();

	// Comparing a variable when it becomes invalid always returns false.
	// Nice easy way to check if I've done something stupid with a zero and a division somewhere
	template <typename Type>
	inline bool isNAN(Type type)
	{
		return type != type;
	}

	// Converts degrees to radians
	inline float deg2rad(float angleDegrees) { return angleDegrees * kfPiOver180; }

	// Converts radians to degrees
	inline float rad2deg(float angleRadians) { return angleRadians * kf180OverPi; }

	// Converts degrees to radians
	inline double deg2rad(double angleDegrees) { return angleDegrees * kdPiOver180; }

	// Converts radians to degrees
	inline double rad2deg(double angleRadians) { return angleRadians * kd180OverPi; }

	// Clamps given value within the range of -1 to +1 then performs standard acos function
	inline float acosClamped(float f)
	{
		if (f < -1.0f)
			return kfPi;
		if (f > 1.0f)
			return 0.0f;
		return acosf(f);
	}

	// Clamps given value within the range of -1 to +1 then performs standard acos function
	inline double acosClamped(double d)
	{
		if (d < -1.0)
			return kdPi;
		if (d > 1.0)
			return 0.0;
		return acos(d);
	}

	// Computes both sin and cos of a scalar (Sometime faster to calculate both at same time)
	inline void sincos(float& outSin, float& outCos, float scalar)
	{
		outSin = sinf(scalar);
		outCos = cosf(scalar);
	}

	// Computes both sin and cos of a scalar (Sometime faster to calculate both at same time)
	inline void sincos(double& outSin, double& outCos, double scalar)
	{
		outSin = sin(scalar);
		outCos = cos(scalar);
	}

	// Clamps a value within given range
	template <class type>
	inline void clamp(type& valueToClamp, type valueA, type valueB)
	{
		type min;
		type max;
		if (valueA < valueB)
		{
			min = valueA;
			max = valueB;
		}
		else
		{
			min = valueB;
			max = valueA;
		}
		if (valueToClamp < min)
			valueToClamp = min;
		else if (valueToClamp > max)
			valueToClamp = max;
	}

	// Sets given variable to absolute value
	template <class type>
	inline void absolute(type& value)
	{
		if (value < 0)
			value *= -1;
	}

	// Computes the difference
	template <class type>
	inline type difference(type valueA, type valueB)
	{
		type result = valueA - valueB;
		absolute(result);
		return result;
	}

	// Returns a random integer between x and y
	inline int randInt(int x, int y) { return rand() % (y - x + 1) + x; }

	// Returns a random float between zero and 1
	inline float randFloat() { return float(rand()) / float(RAND_MAX + 1.0f); }

	// Returns a random double between zero and 1
	inline double randDouble() { return double(rand()) / double(RAND_MAX + 1.0); }

	// Returns a random bool
	inline bool randBool()
	{
		if (randInt(0, 1)) return true;
		return false;
	}

	// Returns a random float in the range -1 < n < 1
	inline float randomClampedFloat() { return randFloat() - randFloat(); }

	// Returns a random double in the range -1 < n < 1
	inline double randomClampedDouble() { return randDouble() - randDouble(); }

	// Returns a random float within the specified range
	inline float randf(float min, float max)
	{
		float fZeroToOne = (float)rand() / float(RAND_MAX + 1.0);
		return min + (max - min) * fZeroToOne;
	}

	// Returns a random double within the specified range
	inline double randd(double min, double max)
	{
		double zeroToOne = (double)rand() / double(RAND_MAX + 1.0);
		return min + (max - min) * zeroToOne;
	}
}