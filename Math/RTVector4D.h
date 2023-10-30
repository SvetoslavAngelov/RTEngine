#pragma once

struct RTVector4D {

	// Leaving the member variables uninitialised by default.
	RTVector4D() = default;

	RTVector4D(float a, float b, float c, float d) :
		x{ a }, y{ b }, z{ c }, w{ d }
	{

	};

	RTVector4D(RTVector4D const& v) :
		x{ v.x }, y{ v.y }, z{ v.z }, w{ v.w }
	{

	};

	// Using the [] operator to iterate over the member variables like an array.
	float& operator[] (int i)
	{
		return ((&x)[i]);
	}

	float const& operator[] (int i) const
	{
		return ((&x)[i]);
	}

	RTVector4D const& operator =(RTVector4D const& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	// Member variables
	float x, y, z, w; 
};