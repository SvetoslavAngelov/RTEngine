#pragma once

#include "RTVector3D.h"

namespace RTMatrix3D {

	struct RTMatrix3DImpl {

		using RTVec3D = RTVector3D::RTVec3DImpl;

		// The default consutrctor sets the matrix to its identity matrix. 
		RTMatrix3DImpl();

		// Constructor taking floating point numbers in row major.  
		RTMatrix3DImpl(float a00, float a01, float a02,
			float a10, float a11, float a12,
			float a20, float a21, float a22);

		// Constructor taking vectors
		RTMatrix3DImpl(RTVec3D const& a, RTVec3D const& b, RTVec3D const& c);

		RTVec3D const operator*(RTVec3D const& v);

		// Helper function to extract a 3D vector from the matrix.
		RTVec3D const operator[](int i) const;

		// Helper function to get an element from the matrix in row major order, i is the row and j the column.
		float const operator()(int i, int j) const;

		// Using row major order, given a 3x3 matrix a[i][j], i is the row and j the column index. 
		float n[3][3];
	};

	/*
		Operator overloads
	*/
	RTMatrix3DImpl const operator*(RTMatrix3DImpl const& m1, RTMatrix3DImpl const& m2);
}
