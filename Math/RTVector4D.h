#pragma once

struct RTVector4D {

	// Leaving the member variables uninitialised by default.
	RTVector4D() = default;

	RTVector4D(float a, float b, float c, float d);
	RTVector4D(RTVector4D const& v);

	// Checks if the value of each vector component is exactly 0. 
	bool isZeroVector() const;

	// Returns the magnitude, or length, of the vector.
	float Magnitude() const;

	// Returns a normalised copy of the vector, if it's safe to do so, otherwise returns a zero vector.
	RTVector4D GetNormal() const;

	// Returns a normalised copy of the vector, but doesn't check for zero length. 
	RTVector4D GetUnsafeNormal() const;

	// Using the [] operator to iterate over the member variables like an array.
	float& operator[] (int i);
	float const& operator[] (int i) const;

	RTVector4D const& operator =(RTVector4D const& v);
	RTVector4D const& operator *=(float s);
	RTVector4D const& operator /=(float s);
	RTVector4D operator *(float s) const;
	RTVector4D operator /(float s) const;
	RTVector4D& operator +=(RTVector4D const& v);
	RTVector4D& operator -=(RTVector4D const& v);
	RTVector4D operator +(RTVector4D const& v) const;
	RTVector4D operator -(RTVector4D const& v) const;

	// Member variables
	float x, y, z, w; 
};