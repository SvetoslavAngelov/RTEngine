#pragma once
#include "RTVector3D.h"

namespace RTMatrix4D {

	struct RTMatrix4DImpl {

		using RTVec3D = RTVector3D::RTVec3DImpl;

		// The default constructor sets the matrix to its identity matrix.
		RTMatrix4DImpl();

		// Constructor taking floating point numbers in row major.  
		RTMatrix4DImpl(float a00, float a01, float a02, float a03,
			float a10, float a11, float a12, float a13,
			float a20, float a21, float a22, float a23,
			float a30, float a31, float a32, float a33);

		/*
			Member functions.
		*/

		RTMatrix4DImpl Inverse(RTMatrix4DImpl const& m) const;

		// Helper function to extract a 3D vector from the matrix. The fourth element contains the weight, so is omitted. 
		RTVec3D const operator[](int i) const;

		// Helper function to get an element from the matrix in row major order, i is the row and j the column.
		float const operator()(int i, int j) const;

		// Using row major order, given a 4x4 matrix a[i][j], i is the row and j the column index. 
		float n[4][4];
	};

	/*
		Operator overloads.
	*/

	RTMatrix4DImpl operator *(RTMatrix4DImpl const& m1, RTMatrix4DImpl const& m2);
	bool operator ==(RTMatrix4DImpl const& m1, RTMatrix4DImpl const& m2);
	bool operator !=(RTMatrix4DImpl const& m1, RTMatrix4DImpl const& m2);
}
