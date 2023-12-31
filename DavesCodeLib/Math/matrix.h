﻿#pragma once
#include "vector3f.h"
#include "quaternion.h"

namespace DC
{
	// Here's the matrix class used for applying transformations to vectors in 3D space
	// and projecting points to 2D.
	// This is a column-major matrix, compatible with OpenGL.
	// So the elements of the matrix are stored like this..
	// [ 0  4  8 12 ]
	// [ 1  5  9 13 ]
	// [ 2  6 10 14 ]
	// [ 3  7 11 15 ]
	// 
	// A matrix is a rectangular array of numbers layed out in rows and columns, like this...
	// [1 0 0 0]
	// [0 1 0 0]
	// [0 0 1 0]
	// [0 0 0 1]
	// 
	// The above is a special case of a matrix, known as an identity matrix where if multiplying
	// a point/vector by it, will produce no translation, rotation or scale.
	//
	// A translation in a matrix for x, y and z are stored in these values...
	// [1 0 0 x]
	// [0 1 0 y]
	// [0 0 1 z]
	// [0 0 0 1]
	//
	// The scaling values are along the diagonal(scaling of x, y and z)...
	// [x 0 0 0]
	// [0 y 0 0]
	// [0 0 z 0]
	// [0 0 0 1]
	// 
	// The rotation part is more complicated since it uses cosine and sine functions and it
	// changes depending on the axis...
	//		X - Axis Rotation			Y - Axis Rotation			Z - Axis Rotation
	// [1	0			0		0]	[cos(θ)		0	-sin(θ)	0]	[cos(θ)		sin(θ)  0   0]
	// [0	cos(θ)		sin(θ)	0]	[0			1   0		0]	[-sin(θ)	cos(θ)  0   0]
	// [0	-sin(θ)		cos(θ)  0]	[sin(θ)		0   cos(θ)	0]	[0			0       1   0]
	// [0	0			0		1]	[0			0   0		1]	[0			0       0   1]
	//
	// The rotation matrices can be combined with the scale and translation matrix to create the final matrix.
	//
	// Here's what a poor vertex has to go through to get to the screen...
	// Vertex object coordinates x model matrix = world coordinates
	// World coordinates x view matrix = eye coordinates
	// Eye coordinates x projection matrix = clipping space coordinates
	// Clipping space coordinates x perspective division = normalized device coordinates
	// Normalized device coordinates x viewport transformation = screen coordinates
	//
	// Vertex object coordinates are the vertex positions usually created in 3D modelling apps such as Blender.
	// Model matrix is what translates, rotates and scales a vertex into the correct world position.
	// View matrix is the camera position and created by the inverse of the camera position. We use the lookat()
	// method to create a view matrix.
	// Projection matrix is created using setProjectionOrtho() and setProjectionPerspective() depending
	// upon how we wish the vertices to be transformed.
	// The other stuff is performed in the vertex/fragment shaders.
	// 
	// Remember that the order of matrix multiplication is important and we go from right to left, like this in a shader...
	// gl_Position = projectionMatrix * viewMatrix * modelMatrix * in_Position;
	// 
	class Matrix
	{
	public:
		friend class Quaternion;

		// Default constructor, sets to identity matrix
		Matrix();

		// Sets this matrix to the one on the right
		const void operator = (const Matrix& matrix);

		// Multiply this matrix by another and return the result
		const Matrix operator * (const Matrix& matrix) const;

		// Multiply this matrix by another
		const void operator *= (const Matrix& matrix);

		// Compares whether this matrix and the one to the right of the == operator are equal.
		// This is an exact compare, no teeny epsilon value.
		bool operator==(const Matrix& matrix) const;

		// Compares whether this matrix and the one to the right of the != operator are equal.
		// This is an exact compare, no teeny epsilon value.
		bool operator!=(const Matrix& matrix) const;

		// Sets matrix to identity matrix
		void setIdentity(void);

		// Fills the matrix with zeros
		void setZero(void);

		// Sets this matrix from the given array of floats
		void set(const float src[16]);

		// Sets the matrix from the one given
		void set(const Matrix& src);

		// Sets the matrix from the one given
//		void set(const glm::mat4& matrix);

		// Returns a matrix containing the contents of this matrix
		Matrix get(void) const;

		// Returns a glm matrix constructed from this one
//		glm::mat4 getGLM(void) const;

		// Set the matrix to hold a translation transformation
		void setTranslation(float X, float Y, float Z);

		// Set the matrix to hold a translation transformation
		void setTranslation(const Vector3f& translation);

		// Set the matrix to hold a scale transformation
		void setScale(float X, float Y, float Z);

		// Set the matrix to hold a scale transformation
		void setScale(const Vector3f& scale);

		// Sets the matrix to hold a rotation transformation
		// The rotation is specified from the give amount of rotation in degrees around the given axis.
		// Working with degrees is slightly slower than working in radians as when using degrees, deg2rad() is called.
		void setFromAxisAngleDegrees(const Vector3f& axis, float angleDegrees);

		// Sets the matrix to hold a rotation transformation
		// The rotation is specified from the give amount of rotation in radians around the given axis.
		// Working with radians is slightly faster than working in degress as when using degrees, deg2rad() is called.
		void setFromAxisAngleRadians(const Vector3f& axis, float angleRadians);

		// Sets the matrix to hold a rotation transformation from a quaternion
		void setFromQuaternion(const Quaternion& quaternion);

		// Sets the given vector to hold this matrix's right vector/axis
		// This is used to obtain the orientation of the current state of the matrix
		void getRightVector(Vector3f& vector) const;

		// Sets the given vector to hold this matrix's up vector/axis
		// This is used to obtain the orientation of the current state of the matrix
		void getUpVector(Vector3f& vector) const;

		// Sets the given vector to hold this matrix's forward vector/axis
		// This is used to obtain the orientation of the current state of the matrix
		void getForwardVector(Vector3f& vector) const;

		// Returns a matrix which is the transpose of this matrix
		// The transpose of a matrix is obtained by flipping the matrix over its diagonal.
		// Which means the rows become columns and the columns become rows.
		// The transpose of a matrix has some useful properties such as reversing the order
		// of multiplication, preserving the trace and determinant, changing the orientation
		// of vectors and converting matrixs between column major and row major.
		Matrix transpose(void);

		// Returns a matrix which is the inverse of this matrix
		// The inverse of a matrix is a matrix that, when multiplied by the original matrix, gives the identity matrix.
		Matrix inverse(void);

		// Multiplies this matrix by the one given and returns the resulting matrix
		Matrix multiply(const Matrix& matrix);

		// Multiplies the given vector by this matrix and returns the resulting vector
		Vector3f multiply(const Vector3f& vector);

		// Multiplies each value in the matrix by the given scalar and returns the resulting matrix
		Matrix multiply(const float scalar);

		// Sets the matrix to represent a perspective projection matrix
		// Aspect ration is width / height of whatever we're rendering to
		void setProjectionPerspective(
			float fieldOfViewInDegrees = 55.0f,
			float nearClippingPlaneDistance = 1.0f,
			float farClippingPlaneDistance = 1000.0f,
			float aspectRatio = 1.3f);

		// Sets the matrix to represent an orthographic prjection matrix
		// right and bottom are width / height of whatever we're rendering to
		void setProjectionOrthographic(
			float left = 0.0f,
			float right = 1.0f,
			float top = 0.0f,
			float bottom = 1.0f,
			float near = -1.0f,
			float far = 1.0f);

		// Sets the matrix to represent a view matrix based upon glLookat
		// vUp should be normalised
		void setViewLookat(const Vector3f& eyePosition, const Vector3f& targetPosition, const Vector3f &up = Vector3f(0.0f, 1.0f, 0.0f));

		// Returns a pointer to the float array containing the matrix
		const float* getFloat(void) const;

		// Returns the current translation stored in the matrix as a Vector3f
		Vector3f getTranslation(void) const;
	private:
		float m[16];	// Values for the matrix
	};
}