#pragma once

namespace RTVector2D {

	/*
		Vector implementation
	*/

	struct RTVec2DImpl {

		// Leaving the member variables uninitialised by default.
		RTVec2DImpl() = default;

		RTVec2DImpl(float a, float b);
		RTVec2DImpl(RTVec2DImpl const& v);

		// Checks if the value of each vector component is exactly 0. 
		bool isZeroVector() const;

		/*
			Operator overloads
		*/

		// Using the [] operator to iterate over the member variables like an array.
		float& operator[] (int i);
		float const& operator[] (int i) const;

		// Assignment oprator.
		RTVec2DImpl const& operator =(RTVec2DImpl const& v);

		/*
			Addition and substraction
		*/

		RTVec2DImpl& operator +=(RTVec2DImpl const& v);
		RTVec2DImpl& operator -=(RTVec2DImpl const& v);
		RTVec2DImpl operator +(RTVec2DImpl const& v) const;
		RTVec2DImpl operator -(RTVec2DImpl const& v) const;

		/*
			Scalar multiplication and division
		*/

		RTVec2DImpl const& operator *=(float s);
		RTVec2DImpl const& operator /=(float s);
		RTVec2DImpl operator *(float s) const;
		RTVec2DImpl operator /(float s) const;

		// Member variables;
		float x, y;
	};

	/*
		Operator overloads
	*/

	bool operator ==(RTVec2DImpl const& a, RTVec2DImpl const& b);
	bool operator !=(RTVec2DImpl const& a, RTVec2DImpl const& b);
}
