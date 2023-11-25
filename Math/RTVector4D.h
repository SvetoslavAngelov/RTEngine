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

	// Checks if the value of each vector component is exactly 0. 
	bool isZeroVector() const
	{
		return (x == 0.f && y == 0.f && z == 0.f && w == 0.f);
	}

	// Returns the magnitude, or length, of the vector.
	float Magnitude() const
	{
		float s = (x * x) + (y * y) + (z * z) + (w * w);
		return (float)sqrt(s);
	}

	// Returns a normalised copy of the vector, if it's safe to do so, otherwise returns a zero vector.
	RTVector4D GetNormal() const
	{
		return !isZeroVector() ? RTVector4D{ *this / this->Magnitude() } : RTVector4D{ 0.f, 0.f, 0.f, 0.f };
	}

	// Returns a normalised copy of the vector, but doesn't check for zero length. 
	RTVector4D GetUnsafeNormal() const
	{
		return RTVector4D{ *this / this->Magnitude() };
	}

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

	RTVector4D const& operator *=(float s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}

	RTVector4D const& operator /=(float s)
	{
		s = 1.f / s;
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}

	RTVector4D operator *(float s) const
	{
		return { x * s, y * s, z * s, w * s };
	}

	RTVector4D operator /(float s) const
	{
		s = 1.f / s;
		return { x * s, y * s, z * s, w * s };
	}

	RTVector4D& operator +=(RTVector4D const& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return (*this);
	}

	RTVector4D& operator -=(RTVector4D const& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return (*this);
	}

	RTVector4D operator +(RTVector4D const& v) const
	{
		return { x + v.x, y + v.y, z + v.z, w + v.w };
	}

	RTVector4D operator -(RTVector4D const& v) const
	{
		return { x - v.x, y - v.y, z - v.z, w - v.w };
	}

	// Member variables
	float x, y, z, w; 
};