#pragma once

namespace RTVector2D {

	/*
		Vector implementation
	*/

	struct RTVec2DImpl {

		// Leaving the member variables uninitialised by default.
		RTVec2DImpl() = default;

		RTVec2DImpl(float a, float b) :
			x{ a }, y{ b }
		{

		};

		RTVec2DImpl(RTVec2DImpl const& v) :
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
		RTVec2DImpl const& operator =(RTVec2DImpl const& v)
		{
			x = v.x;
			y = v.y;
			return *this;
		}

		/*
			Addition and substraction
		*/

		RTVec2DImpl& operator +=(RTVec2DImpl const& v)
		{
			x += v.x;
			y += v.y;
			return (*this);
		}

		RTVec2DImpl& operator -=(RTVec2DImpl const& v)
		{
			x -= v.x;
			y -= v.y;
			return (*this);
		}

		RTVec2DImpl operator +(RTVec2DImpl const& v) const
		{
			return { x + v.x, y + v.y };
		}

		RTVec2DImpl operator -(RTVec2DImpl const& v) const
		{
			return { x - v.x, y - v.y };
		}

		/*
			Scalar multiplication and division
		*/

		RTVec2DImpl const& operator *=(float s)
		{
			x *= s;
			y *= s;
			return *this;
		}

		RTVec2DImpl const& operator /=(float s)
		{
			s = 1.f / s;
			x *= s;
			y *= s;
			return *this;
		}

		RTVec2DImpl operator *(float s) const
		{
			return { x * s, y * s };
		}

		RTVec2DImpl operator /(float s) const
		{
			s = 1.f / s;
			return { x * s, y * s };
		}

		// Member variables;
		float x, y;
	};

	/*
		Operator overloads
	*/

	bool operator ==(RTVec2DImpl const& a, RTVec2DImpl const& b)
	{
		return a.x == b.x && a.y == b.y;
	}

	bool operator !=(RTVec2DImpl const& a, RTVec2DImpl const& b)
	{
		return !(a == b);
	}
}
