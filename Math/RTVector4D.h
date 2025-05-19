#pragma once

namespace RTVector4D {

	struct RTVec4DImpl {

		// Leaving the member variables uninitialised by default.
		RTVec4DImpl() = default;

		RTVec4DImpl(float a, float b, float c, float d);
		RTVec4DImpl(RTVec4DImpl const& v);

		// Checks if the value of each vector component is exactly 0. 
		bool isZeroVector() const;

		// Returns the magnitude, or length, of the vector.
		float Magnitude() const;

		// Returns a normalised copy of the vector, if it's safe to do so, otherwise returns a zero vector.
		RTVec4DImpl GetNormal() const;

		// Returns a normalised copy of the vector, but doesn't check for zero length. 
		RTVec4DImpl GetUnsafeNormal() const;

		// Using the [] operator to iterate over the member variables like an array.
		float& operator[] (int i);
		float const& operator[] (int i) const;

		RTVec4DImpl const& operator =(RTVec4DImpl const& v);
		RTVec4DImpl const& operator *=(float s);
		RTVec4DImpl const& operator /=(float s);
		RTVec4DImpl operator *(float s) const;
		RTVec4DImpl operator /(float s) const;
		RTVec4DImpl& operator +=(RTVec4DImpl const& v);
		RTVec4DImpl& operator -=(RTVec4DImpl const& v);
		RTVec4DImpl operator +(RTVec4DImpl const& v) const;
		RTVec4DImpl operator -(RTVec4DImpl const& v) const;

		// Member variables
		float x, y, z, w; 
	};

	// Convenience typedef for backward compatibility
	typedef RTVec4DImpl RTVector4D;
}