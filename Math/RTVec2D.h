#pragma once

struct RTVec2D {

	// Leaving the member variables uninitialised by default.
	RTVec2D() = default;

	RTVec2D(float a, float b) :
		x{ a }, y{ b }
	{

	};

	RTVec2D(RTVec2D const& v) :
		x{ v.x }, y{ v.y }
	{

	};

	// Checks if the value of each vector component is exactly 0. 
	bool isZeroVector() const
	{
		return (x == 0.f && y == 0.f);
	}

	/*
		Operator overloads
	*/

	// Using the [] operator to iterate over the member variables like an array.
	float& operator[] (int i)
	{
		return ((&x)[i]);
	}

	float const& operator[] (int i) const
	{
		return ((&x)[i]);
	}

	// Assignment oprator.
	RTVec2D const& operator =(RTVec2D const& v)
	{
		x = v.x; 
		y = v.y;
		return *this;
	}

	/*
		Addition and substraction
	*/

	RTVec2D& operator +=(RTVec2D const& v)
	{
		x += v.x;
		y += v.y;
		return (*this);
	}

	RTVec2D& operator -=(RTVec2D const& v)
	{
		x -= v.x;
		y -= v.y;
		return (*this);
	}

	RTVec2D operator +(RTVec2D const& v) const
	{
		return { x + v.x, y + v.y };
	}

	RTVec2D operator -(RTVec2D const& v) const
	{
		return { x - v.x, y - v.y };
	}

	/*
		Scalar multiplication and division
	*/

	RTVec2D const& operator *=(float s)
	{
		x *= s;
		y *= s;
		return *this;
	}

	RTVec2D const& operator /=(float s)
	{
		s = 1.f / s;
		x *= s;
		y *= s;
		return *this;
	}

	RTVec2D operator *(float s) const
	{
		return { x * s, y * s };
	}

	RTVec2D operator /(float s) const
	{
		s = 1.f / s;
		return { x * s, y * s };
	}

	/*
		Comparison operators.
	*/

	friend bool operator ==(RTVec2D const& a, RTVec2D const& b)
	{
		return a.x == b.x && a.y == b.y;
	}

	friend bool operator !=(RTVec2D const& a, RTVec2D const& b)
	{
		return !(a == b);
	}

	// Member variables;
	float x, y;
};