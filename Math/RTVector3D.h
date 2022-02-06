#pragma once

#include <math.h>

namespace RTVector3D {
	
	struct RTVec3DImpl {

		// Leaving the member variables uninitialised by default.
		RTVec3DImpl() = default;

		RTVec3DImpl(float a, float b, float c) :
			x{ a }, y{ b }, z{ c }
		{

		};

		RTVec3DImpl(RTVec3DImpl const& v) :
			x{ v.x }, y{ v.y }, z{ v.z }
		{

		};

		// Checks if the value of each vector component is exactly 0. 
		bool isZeroVector() const
		{
			return (x == 0.f && y == 0.f && z == 0.f);
		}

		// Returns the magnitude, or length, of the vector.
		float Magnitude() const
		{
			float s = (x * x) + (y * y) + (z * z);
			return (float)sqrt(s);
		}

		// Returns a normalised copy of the vector, if it's safe to do so, otherwise returns a zero vector.
		RTVec3DImpl GetNormal() const
		{
			return !isZeroVector() ? RTVec3DImpl{ *this / this->Magnitude() } : RTVec3DImpl{ 0.f, 0.f, 0.f };
		}

		// Returns a normalised copy of the vector, but doesn't check for zero length. 
		RTVec3DImpl GetUnsafeNormal() const
		{
			return RTVec3DImpl{ *this / this->Magnitude() };
		}

		// Operator overloads

		// Using the [] operator to iterate over the member variables like an array.
		float& operator[] (int i)
		{
			return ((&x)[i]);
		}

		float const& operator[] (int i) const
		{
			return ((&x)[i]);
		}

		RTVec3DImpl const& operator =(RTVec3DImpl const& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}

		RTVec3DImpl const& operator *=(float s)
		{
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}

		RTVec3DImpl const& operator /=(float s)
		{
			s = 1.f / s;
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}

		RTVec3DImpl operator *(float s) const
		{
			return { x * s, y * s, z * s };
		}

		RTVec3DImpl operator /(float s) const
		{
			s = 1.f / s;
			return { x * s, y * s, z * s };
		}

		RTVec3DImpl& operator +=(RTVec3DImpl const& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			return (*this);
		}

		RTVec3DImpl& operator -=(RTVec3DImpl const& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return (*this);
		}

		RTVec3DImpl operator +(RTVec3DImpl const& v) const
		{
			return { x + v.x, y + v.y, z + v.z };
		}

		RTVec3DImpl operator -(RTVec3DImpl const& v) const
		{
			return { x - v.x, y - v.y, z - v.z };
		}

		// Member variables;
		float x, y, z;
	};

	/*
	Vector operations
	*/

	// Returns the dot product between two 3D vectors. 
	float DotProduct(RTVec3DImpl const& a, RTVec3DImpl const& b)
	{
		return { a.x * b.x + a.y * b.y + a.z * b.z };
	}

	// Retruns the cross product between two 3D vectors. 
	RTVec3DImpl CrossProduct(RTVec3DImpl const& a, RTVec3DImpl const& b)
	{
		return {
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}

	// Returns a new 3D vector which is a projection of vector a onto vector b.
	RTVec3DImpl Projection(RTVec3DImpl const& a, RTVec3DImpl const& b)
	{
		return { b * (DotProduct(a,b) / DotProduct(b,b)) };
	}

	// Returns a new 3D vector which is a rejection of vector a from b and is perpendicular to b. 
	RTVec3DImpl Rejection(RTVec3DImpl const& a, RTVec3DImpl const& b)
	{
		return { (a - b) * (DotProduct(a,b) / DotProduct(b,b)) };
	}

	/*
		Operator overloads
	*/
	bool operator ==(RTVec3DImpl const& a, RTVec3DImpl const& b)
	{
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}

	bool operator !=(RTVec3DImpl const& a, RTVec3DImpl const& b)
	{
		return !(a == b);
	}
}






