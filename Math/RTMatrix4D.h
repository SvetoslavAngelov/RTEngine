#pragma once
#include <algorithm>
#include "RTMath.h"

struct RTMatrix4D {

	// The default constructor sets the matrix to its identity matrix.
	RTMatrix4D()
	{
		n[0][0] = n[1][1] = n[2][2] = n[3][3] = 1.f;

		n[0][1] = n[0][2] = n[0][3] = 
			n[1][0] = n[1][2] = n[1][3] = 
				n[2][0] = n[2][1] = n[2][3] = 
					n[3][0] = n[3][1] = n[3][2] = 0.f;
	}

	// Constructor from individual floats.
	// Constructor taking floating point numbers in row major.  
	RTMatrix4D(float a00, float a01, float a02, float a03,
		float a10, float a11, float a12, float a13,
		float a20, float a21, float a22, float a23,
		float a30, float a31, float a32, float a33)
	{
		n[0][0] = a00;
		n[0][1] = a01;
		n[0][2] = a02;
		n[0][3] = a03;

		n[1][0] = a10;
		n[1][1] = a11;
		n[1][2] = a12;
		n[1][3] = a13;

		n[2][0] = a20;
		n[2][1] = a21;
		n[2][2] = a22;
		n[2][3] = a23;

		n[3][0] = a30;
		n[3][1] = a31;
		n[3][2] = a32;
		n[3][3] = a33;
	}

	/*
		Member functions. 	
	*/

	RTMatrix4D Inverse(RTMatrix4D const& m) const
	{
		RTVec3D a = m[0];
		RTVec3D b = m[1];
		RTVec3D c = m[2];
		RTVec3D d = m[3];

		float x = m(3, 0);
		float y = m(3, 1);
		float z = m(3, 2);
		float w = m(3, 3);

		RTVec3D s = CrossProduct(a, b);
		RTVec3D t = CrossProduct(c, d);
		RTVec3D u = a * y - b * x;
		RTVec3D v = c * w - d * z;

		float invDet = 1.f / (DotProduct(s, v) + DotProduct(t, u));
		s *= invDet;
		t *= invDet; 
		u *= invDet;
		v *= invDet; 

		RTVec3D r0 = CrossProduct(b, v) + t * y; 
		RTVec3D r1 = CrossProduct(v, a) + t * x;
		RTVec3D	r2 = CrossProduct(d, u) + s * w; 
		RTVec3D r3 = CrossProduct(u, c) - s * z;

		return RTMatrix4D{ r0.x, r0.y, r0.z, -DotProduct(b, t),
							r1.x, r1.y, r1.z, DotProduct(a, t),
							r2.x, r2.y, r2.z, -DotProduct(d, s),
							r3.x, r3.y, r3.z, DotProduct(c, s) };
	}

	/*
		Operator overloads.
	*/

	friend RTMatrix4D operator *(RTMatrix4D const& m1, RTMatrix4D const& m2) 
	{
		RTMatrix4D res;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				res.n[i][j] = m1.n[i][0] * m2.n[0][j] + m1.n[i][1] * m2.n[1][j] +
				m1.n[i][2] * m2.n[2][j] + m1.n[i][3] * m2.n[3][j];
		return res;
	}

	friend bool operator ==(RTMatrix4D const& m1, RTMatrix4D const& m2)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; i < 4; ++j)
			{
				if (m1[i][j] != m2[i][j])
				{
					return false;
				}
					
			}
		}
		return true;
	}

	friend bool operator !=(RTMatrix4D const& m1, RTMatrix4D const& m2)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; i < 4; ++j)
			{
				if (m1[i][j] != m2[i][j])
				{
					return true;
				}

			}
		}
		return false;
	}

	// Helper function to extract a 3D vector from the matrix. The fourth element contains the weight, so is omitted. 
	RTVec3D const operator[](int i) const
	{
		return RTVec3D{ n[i][0], n[i][1], n[i][2] };
	}

	// Helper function to get an element from the matrix in row major order, i is the row and j the column.
	float const operator()(int i, int j) const
	{
		return (n[i][j]);
	}

	// Using row major order, given a 4x4 matrix a[i][j], i is the row and j the column index. 
	float n[4][4];
};