#include "RTMatrix3D.h"

namespace RTMatrix3D {

    // The default consutrctor sets the matrix to its identity matrix. 
    RTMatrix3DImpl::RTMatrix3DImpl()
    {
        n[0][0] = n[1][1] = n[2][2] = 1.f;

        n[0][1] = n[0][2] =
            n[1][0] = n[1][2] =
            n[2][0] = n[2][1] = 0.f;
    }

    // Constructor taking floating point numbers in row major.  
    RTMatrix3DImpl::RTMatrix3DImpl(float a00, float a01, float a02,
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
    RTMatrix3DImpl::RTMatrix3DImpl(RTVec3D const& a, RTVec3D const& b, RTVec3D const& c)
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

    RTMatrix3DImpl::RTVec3D const RTMatrix3DImpl::operator*(RTVec3D const& v)
    {
        return RTVec3D{ n[0][0] * v[0] + n[0][1] * v[1] + n[0][2] * v[2],
                        n[1][0] * v[0] + n[1][1] * v[1] + n[1][2] * v[2],
                        n[2][0] * v[0] + n[2][1] * v[1] + n[2][2] * v[2] };
    }

    // Helper function to extract a 3D vector from the matrix.
    RTMatrix3DImpl::RTVec3D const RTMatrix3DImpl::operator[](int i) const
    {
        return RTVec3D{ n[i][0], n[i][1], n[i][2] };
    }

    // Helper function to get an element from the matrix in row major order, i is the row and j the column.
    float const RTMatrix3DImpl::operator()(int i, int j) const
    {
        return (n[i][j]);
    }

    /*
        Operator overloads
    */
    RTMatrix3DImpl const operator*(RTMatrix3DImpl const& m1, RTMatrix3DImpl const& m2)
    {
        RTMatrix3DImpl res;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                res.n[i][j] = m1.n[i][0] * m2.n[0][j] + m1.n[i][1] * m2.n[1][j] +
                m1.n[i][2] * m2.n[2][j];
        return res;
    }
}