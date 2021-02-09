#pragma once
#include <math.h>

struct RTVec3D {

	// Leaving the member variables uninitialised by default.
	RTVec3D() = default;

	RTVec3D(float a, float b, float c) :
	x { a }, y { b }, z{ c }
	{

	};

	RTVec3D(RTVec3D const& v) :
		x{ v.x }, y{ v.y }, z{ v.z }
	{
	
	};

	// Checks if the value of each vector component is exactly 0. 
	bool isZeroVector() const
	{
		return (x == 0.f && y == 0.f && z == 0.f);
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

	// Scalar multiplication. 
	RTVec3D const& operator *=(float s)
	{
		x *= s; 
		y *= s;
		z *= s;
		return *this;
	}

	// Scalar division.
	RTVec3D const& operator /=(float s)
	{
		s = 1.f / s; 
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	// Returns the magnitude, or length, of the vector.
	float Magnitude() const
	{
		return sqrt( (x * x) + (y * y) + (z * z) );
	}

	// Returns a normalised copy of the vector, if it's safe to do so, otherwise returns a zero vector.
	RTVec3D GetNormal() const
	{ 
		return !isZeroVector() ? RTVec3D{ /**/ } : RTVec3D{ 0.f, 0.f, 0.f };
	}

	// Returns a normalised copy of the vector, but doesn't check for zero length. 
	RTVec3D GetUnsafeNormal() const
	{

	}

	// Member variables;
	float x, y, z;
};

inline RTVec3D operator *(RTVec3D const& v, float s)
{
	return { v.x * s, v.y * s, v.z * s };
}

inline RTVec3D operator /(RTVec3D const& v, float s)
{
	s = 1.f / s;
	return { v.x * s, v.y * s, v.z * s };
}

inline RTVec3D operator -(RTVec3D const& v)
{
	return { -v.x, -v.y, -v.z };
}





