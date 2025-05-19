#pragma once

#include <math.h>

namespace RTVector3D {
	
	struct RTVec3DImpl {

		// Leaving the member variables uninitialised by default.
		RTVec3DImpl() = default;

		RTVec3DImpl(float a, float b, float c);
		RTVec3DImpl(RTVec3DImpl const& v);

		// Checks if the value of each vector component is exactly 0. 
		bool isZeroVector() const;

		// Returns the magnitude, or length, of the vector.
		float Magnitude() const;

		// Returns a normalised copy of the vector, if it's safe to do so, otherwise returns a zero vector.
		RTVec3DImpl GetNormal() const;

		// Returns a normalised copy of the vector, but doesn't check for zero length. 
		RTVec3DImpl GetUnsafeNormal() const;

		// Operator overloads

		// Using the [] operator to iterate over the member variables like an array.
		float& operator[] (int i);
		float const& operator[] (int i) const;
		RTVec3DImpl const& operator =(RTVec3DImpl const& v);
		RTVec3DImpl const& operator *=(float s);
		RTVec3DImpl const& operator /=(float s);
		RTVec3DImpl operator *(float s) const;
		RTVec3DImpl operator /(float s) const;
		RTVec3DImpl& operator +=(RTVec3DImpl const& v);
		RTVec3DImpl& operator -=(RTVec3DImpl const& v);
		RTVec3DImpl operator +(RTVec3DImpl const& v) const;
		RTVec3DImpl operator -(RTVec3DImpl const& v) const;

		// Member variables;
		float x, y, z;
	};

	/*
	Vector operations
	*/

	// Returns the dot product between two 3D vectors. 
	float DotProduct(RTVec3DImpl const& a, RTVec3DImpl const& b);

	// Retruns the cross product between two 3D vectors. 
	RTVec3DImpl CrossProduct(RTVec3DImpl const& a, RTVec3DImpl const& b);

	// Returns a new 3D vector which is a projection of vector a onto vector b.
	RTVec3DImpl Projection(RTVec3DImpl const& a, RTVec3DImpl const& b);

	// Returns a new 3D vector which is a rejection of vector a from b and is perpendicular to b. 
	RTVec3DImpl Rejection(RTVec3DImpl const& a, RTVec3DImpl const& b);

	/*
		Operator overloads
	*/
	bool operator ==(RTVec3DImpl const& a, RTVec3DImpl const& b);
	bool operator !=(RTVec3DImpl const& a, RTVec3DImpl const& b);
}






