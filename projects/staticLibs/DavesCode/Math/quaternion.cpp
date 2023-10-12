#include "quaternion.h"
#include "matrix.h"
#include <cstring>
#include <math.h>

namespace DC
{
	Quaternion::Quaternion()
	{
		setIdentity();
	}

	Quaternion::Quaternion(float X, float Y, float Z, float W)
	{
		q[0] = X;
		q[1] = Y;
		q[2] = Z;
		q[3] = W;
	}

	Quaternion::Quaternion(const Quaternion& quaternion)
	{
		*this = quaternion;
	}

	float& Quaternion::operator [](const int index)
	{
		return q[index];
	}

	const Quaternion Quaternion::operator *(const Quaternion& quat) const
	{
		/* Does the same as below? But too many sqrtf
		Vector3 v1(mfQuat[0], mfQuat[1], mfQuat[2]);
		Vector3 v2(quat.mfQuat[0], quat.mfQuat[1], quat.mfQuat[2]);
		Vector3 vFinal = v1 * quat.mfQuat[3] + v2 * mfQuat[3] + v1.getCross(v2);
		float fScalar = mfQuat[3] * quat.mfQuat[3] - v1.getDot(v2);
		return Quaternion(vFinal.x, vFinal.y, vFinal.z, fScalar);
		*/
		return Quaternion(q[3] * quat.q[0] + q[0] * quat.q[3] +
			q[1] * quat.q[2] - q[2] * quat.q[1],

			q[3] * quat.q[1] + q[1] * quat.q[3] +
			q[2] * quat.q[0] - q[0] * quat.q[2],

			q[3] * quat.q[2] + q[2] * quat.q[3] +
			q[0] * quat.q[1] - q[1] * quat.q[0],

			q[3] * quat.q[3] - q[0] * quat.q[0] -
			q[1] * quat.q[1] - q[2] * quat.q[2]);
	}

	const void Quaternion::operator *=(const Quaternion& quat)
	{
		Quaternion tmp(q[3] * quat.q[0] + q[0] * quat.q[3] +
			q[1] * quat.q[2] - q[2] * quat.q[1],

			q[3] * quat.q[1] + q[1] * quat.q[3] +
			q[2] * quat.q[0] - q[0] * quat.q[2],

			q[3] * quat.q[2] + q[2] * quat.q[3] +
			q[0] * quat.q[1] - q[1] * quat.q[0],

			q[3] * quat.q[3] - q[0] * quat.q[0] -
			q[1] * quat.q[1] - q[2] * quat.q[2]);
		*this = tmp;
	}

	const Quaternion Quaternion::operator +(const Quaternion& quat) const
	{
		return Quaternion(
			q[0] + quat.q[0],
			q[1] + quat.q[1],
			q[2] + quat.q[2],
			q[3] + quat.q[3]);
	}

	const Quaternion Quaternion::operator -(const Quaternion& quat) const
	{
		return Quaternion(
			q[0] - quat.q[0],
			q[1] - quat.q[1],
			q[2] - quat.q[2],
			q[3] - quat.q[3]);
	}

	const Quaternion Quaternion::operator *(const float fScalar) const
	{
		return Quaternion(
			q[0] * fScalar,
			q[1] * fScalar,
			q[2] * fScalar,
			q[3] * fScalar);
	}

	const Quaternion Quaternion::operator /(const float scalar) const
	{
		float fInvScl = 1 / scalar;
		return Quaternion(
			q[0] * fInvScl,
			q[1] * fInvScl,
			q[2] * fInvScl,
			q[3] * fInvScl);
	}

	Vector3f Quaternion::operator *(const Vector3f& vector) const
	{
		// nVidia SDK implementation (Taken from OGRE)
		Vector3f v2, v3;
		Vector3f vQ(q[0], q[1], q[2]);
		v2 = vQ.getCross(vector);
		v3 = vQ.getCross(v2);
		v2 *= (2.0f * q[3]);
		v3 *= 2.0f;
		return vector + v2 + v3;
	}

	const void Quaternion::operator =(const Quaternion& quaternion)
	{
		memcpy(q, quaternion.q, sizeof(float[4]));
	}

	const bool Quaternion::operator ==(const Quaternion& quat) const
	{
		return (q[0] == quat.q[0] && q[1] == quat.q[1] && q[2] == quat.q[2] && q[3] == quat.q[3]);
	}

	const bool Quaternion::operator != (const Quaternion& quaternion) const
	{
		return !(*this == quaternion);
	}

	void Quaternion::setIdentity(void)
	{
		q[0] = q[1] = q[2] = 0;
		q[3] = 1;
	}

	float Quaternion::getMagnitude(void) const
	{
		return sqrtf(	(q[0] * q[0]) +
						(q[1] * q[1]) +
						(q[2] * q[2]) +
						(q[3] * q[3]));
	}

	void Quaternion::normalise(void)
	{
		float fInvMag = 1.0f / getMagnitude();
		if (fInvMag != 1.0f)
		{
			q[0] *= fInvMag;
			q[1] *= fInvMag;
			q[2] *= fInvMag;
			q[3] *= fInvMag;
		}
	}

	Quaternion Quaternion::getConjugate(void) const
	{
		return Quaternion(-q[0], -q[1], -q[2], q[3]);
	}

	Quaternion Quaternion::getNegative(void) const
	{
		return Quaternion(-q[0], -q[1], -q[2], -q[3]);
	}

	void Quaternion::setFromAxisAngle(const Vector3f& axis, float angleRadians)
	{
		float sa = sinf(angleRadians * 0.5f);
		float ca = cosf(angleRadians * 0.5f);
		q[0] = axis.x * sa;
		q[1] = axis.y * sa;
		q[2] = axis.z * sa;
		q[3] = ca;
	}

	Vector3f Quaternion::getAxis(void) const
	{
		float sa = 1 / sqrtf(1 - (q[3] * q[3]));
		return Vector3f(q[0] * sa, q[1] * sa, q[2] * sa);
	}

	float Quaternion::getAngle(void) const
	{
		return (float)2 * acosf(q[3]);
	}

	void Quaternion::setFromEuler(float angleRadiansX, float angleRadiansY, float angleRadiansZ)
	{
		double dSY = sin(angleRadiansZ * 0.5f);
		double dSP = sin(angleRadiansY * 0.5f);
		double dSR = sin(angleRadiansX * 0.5f);
		double dCY = cos(angleRadiansZ * 0.5f);
		double dCP = cos(angleRadiansY * 0.5f);
		double dCR = cos(angleRadiansX * 0.5f);

		q[0] = float(dSR * dCP * dCY - dCR * dSP * dSY);
		q[1] = float(dCR * dSP * dCY + dSR * dCP * dSY);
		q[2] = float(dCR * dCP * dSY - dSR * dSP * dCY);
		q[3] = float(dCR * dCP * dCY + dSR * dSP * dSY);
		normalise();
	}

	Vector3f Quaternion::getEuler(void) const
	{
		float fSq[4]{};

		fSq[0] = q[0] * q[0];
		fSq[1] = q[1] * q[1];
		fSq[2] = q[2] * q[2];
		fSq[3] = q[3] * q[3];

		double m11 = fSq[3] + fSq[0] - fSq[1] - fSq[2];
		double m21 = 2 * (q[0] * q[1] + q[2] * q[3]);
		double m31 = 2 * (q[2] * q[0] - q[1] * q[3]);
		double m32 = 2 * (q[2] * q[1] + q[0] * q[3]);
		double m33 = fSq[3] - fSq[0] - fSq[1] + fSq[2];
		return Vector3f(float(atan2(m32, m33)), asinf(float(-m31)), float(atan2(m21, m11)));
	}

	void Quaternion::setFromMatrix(const Matrix& m)
	{
		float fTr = m.m[0] + m.m[5] + m.m[9] + 1.0f;
		// If the trace of the matrix is greater than 0, perform an instant calculation
		if (fTr > 0)
		{
			float fS = 0.5f / sqrtf(fTr);

			q[3] = 0.25f / fS;
			q[0] = (m.m[9] - m.m[6]) * fS;
			q[1] = (m.m[2] - m.m[8]) * fS;
			q[2] = (m.m[4] - m.m[1]) * fS;
		}
		// if the first diagonal element is the largest
		else if (m.m[0] > m.m[5] && m.m[0] > m.m[10])
		{
			float fS = float(1 / (sqrt(m.m[0] - m.m[5] - m.m[10] + 1.0f) * 2));
			q[0] = 0.5f * fS;
			q[1] = (m.m[1] + m.m[4]) * fS;
			q[2] = (m.m[2] + m.m[8]) * fS;
			q[3] = (m.m[6] + m.m[9]) * fS;
		}
		// if the second.....
		else if (m.m[5] > m.m[10])
		{
			float fS = float(1 / (sqrt(m.m[5] - m.m[0] - m.m[10] + 1.0f) * 2));
			q[0] = (m.m[1] + m.m[4]) * fS;
			q[1] = 0.5f * fS;
			q[2] = (m.m[6] + m.m[9]) * fS;
			q[3] = (m.m[2] + m.m[8]) * fS;
		}
		// if the last element is the largest
		else
		{
			float fS = float(1 / (sqrt(m.m[10] - m.m[0] - m.m[5] + 1.0f) * 2));
			q[0] = (m.m[2] + m.m[8]) * fS;
			q[1] = (m.m[6] + m.m[9]) * fS;
			q[2] = 0.5f * fS;
			q[3] = (m.m[1] + m.m[4]) * fS;
		}
	}

	void Quaternion::rotate(const Quaternion& quaternion)
	{
		Quaternion tmp(quaternion * (*this) * quaternion.getConjugate());
		*this = tmp;
	}

	Quaternion Quaternion::getSLERP(const Quaternion& quaternion, float interval) const
	{
		Quaternion q(quaternion);

		float fDot = q[0] * q.q[0] + q[1] * q.q[1] + q[2] * q.q[2] + q[3] * q.q[3];
		if (fDot < 0.0f)
		{
			q = q.getNegative();
			fDot = -fDot;
		}

		// Inaccurate, use lerp instead
		if (fDot < 1.00001f && fDot > 0.99999f)
		{
			return getLERP(q, interval);
		}

		// Calculate the angle between the quaternions 
		float fTheta = acosf(fDot);

		return ((*this * sinf(fTheta * (1 - interval)) + q * sinf(fTheta * interval)) / sinf(fTheta));
	}

	Quaternion Quaternion::getLERP(const Quaternion& quaternion, float interval) const
	{
		Quaternion ret(((quaternion - *this) * interval) + *this);
		ret.normalise();
		return ret;
	}
}