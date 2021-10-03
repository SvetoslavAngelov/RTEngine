#pragma once
#include "RTVec3D.h"

struct RTMatrix3D {

	// Leaving the member variables uninitialised by default.
	RTMatrix3D() = default;

	// Constructor taking floating point numbers in row major.  
	RTMatrix3D(	float a00, float a01, float a02,
				float a10, float a11, float a12,
				float a20, float a21, float a22) 
	{
		n[0][0] = a00;
		n[0][1] = a01; 
		n[0][2] = a02;

		n[1][0] = a10;
		n[1][1] = a11;
		n[1][2] = a12;

		n[2][0] = a20;
		n[2][1] = a21;
		n[2][2] = a22;
	}

	// Constructor taking vectors
	RTMatrix3D(RTVec3D const& a, RTVec3D const& b, RTVec3D const& c)
	{
		n[0][0] = a.x;
		n[0][1] = a.y;
		n[0][2] = a.z;

		n[1][0] = b.x;
		n[1][1] = b.y;
		n[1][2] = b.z;

		n[2][0] = c.x;
		n[2][1] = c.y;
		n[2][2] = c.z;
	}

	RTMatrix3D const operator*(RTMatrix3D const& B)
	{
		return RTMatrix3D{	n[0][0] * B.n[0][0] + n[0][1] * B.n[1][0] + n[0][2] * B.n[2][0],
							n[0][0] * B.n[0][1] + n[0][1] * B.n[1][1] + n[0][2] * B.n[2][1],
							n[0][0] * B.n[0][2] + n[0][1] * B.n[1][2] + n[0][2] * B.n[2][2],
							n[1][0] * B.n[0][0] + n[1][1] * B.n[1][0] + n[1][2] * B.n[2][0],
							n[1][0] * B.n[0][1] + n[1][1] * B.n[1][1] + n[1][2] * B.n[2][1],
							n[1][0] * B.n[0][2] + n[1][1] * B.n[1][2] + n[1][2] * B.n[2][2],
							n[2][0] * B.n[0][0] + n[2][1] * B.n[1][0] + n[2][2] * B.n[2][0],
							n[2][0] * B.n[0][1] + n[2][1] * B.n[1][1] + n[2][2] * B.n[2][1],
							n[2][0] * B.n[0][2] + n[2][1] * B.n[1][2] + n[2][2] * B.n[2][2]
		};
	}

	RTVec3D const operator*(RTVec3D const& v)
	{
		return RTVec3D{ n[0][0] * v[0] + n[0][1] * v[1] + n[0][2] * v[2],
						n[1][0] * v[0] + n[1][1] * v[1] + n[1][2] * v[2],
						n[2][0] * v[0] + n[2][1] * v[1] + n[2][2] * v[2] };
	}

	// Helper function to extract a 3D vector from the matrix. Returns zero vector if index is larger than 2. 
	RTVec3D const operator[](int i)
	{
		if (i > 2) {
			return RTVec3D{ 0.f, 0.f, 0.f };
		}

		return { *reinterpret_cast<RTVec3D*>(n[i]) };
	}

	// Helper function to get an element from the matrix in row major order, i is the row and j the column.
	float const operator()(int i, int j)
	{
		return (n[i][j]);
	}

	// Using row major order, given a 3x3 matrix a[i][j], i is the row and j the column index. 
	float n[3][3];
};