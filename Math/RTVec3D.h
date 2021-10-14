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

	// Returns the dot product between two 3D vectors. 
	static float DotProduct(RTVec3D const& a, RTVec3D const& b)
	{
		return { a.x * b.x + a.y * b.y + a.z * b.z };
	}

	// Retruns the cross product between two 3D vectors. 
	static RTVec3D CrossProduct(RTVec3D const& a, RTVec3D const& b)
	{
		return {
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}

	// Returns a new 3D vector which is a projection of vector a onto vector b.
	static RTVec3D Projection(RTVec3D const& a, RTVec3D const& b)
	{
		return { b * (DotProduct(a,b) / DotProduct(b,b)) };
	}

	// Returns a new 3D vector which is a rejection of vector a from b and is perpendicular to b. 
	static RTVec3D Rejection(RTVec3D const& a, RTVec3D const& b)
	{
		return { (a - b) * (DotProduct(a,b) / DotProduct(b,b)) };
	}

	// Returns the magnitude, or length, of the vector.
	float Magnitude() const
	{
		float s = (x * x) + (y * y) + (z * z);
		return (float)sqrt( s );
	}

	// Returns a normalised copy of the vector, if it's safe to do so, otherwise returns a zero vector.
	RTVec3D GetNormal() const
	{ 
		return !isZeroVector() ? RTVec3D{ *this / this->Magnitude() } : RTVec3D{ 0.f, 0.f, 0.f };
	}

	// Returns a normalised copy of the vector, but doesn't check for zero length. 
	RTVec3D GetUnsafeNormal() const
	{
		return RTVec3D{ *this / this->Magnitude() };
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

	RTVec3D const& operator =(RTVec3D const& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	RTVec3D const& operator *=(float s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	RTVec3D const& operator /=(float s)
	{
		s = 1.f / s;
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	RTVec3D operator *(float s) const
	{
		return { x * s, y * s, z * s };
	}

	RTVec3D operator /(float s) const
	{
		s = 1.f / s;
		return { x * s, y * s, z * s };
	}

	RTVec3D& operator +=(RTVec3D const& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return (*this);
	}

	RTVec3D& operator -=(RTVec3D const& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return (*this);
	}

	RTVec3D operator +(RTVec3D const& v) const
	{
		return { x + v.x, y + v.y, z + v.z };
	}

	RTVec3D operator -(RTVec3D const& v) const
	{
		return { x - v.x, y - v.y, z - v.z };
	}

	friend bool operator ==(RTVec3D const& a, RTVec3D const& b)
	{
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}

	friend bool operator !=(RTVec3D const& a, RTVec3D const& b)
	{
		return !(a == b);
	}

	// Member variables;
	float x, y, z;
};




