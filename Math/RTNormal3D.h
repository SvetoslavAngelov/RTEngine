#pragma once

#include "RTVector3D.h"

namespace RTNormal3D {

    struct RTNormal3DImpl {

        using RTVec3D = RTVector3D::RTVec3DImpl;

        // Default constructor
        RTNormal3DImpl() = default;

        // Component constructor
        RTNormal3DImpl(float a, float b, float c) :
            x{ a }, y{ b }, z{ c }
        {

        }

        // Vector constructor
        explicit RTNormal3DImpl(RTVec3D const& v) :
            x{ v.x }, y{ v.y }, z{ v.z }
        {

        }

        /*
            Member functions
        */

        float LengthSquared() const
        {
            return { x * x + y * y + z * z };
        }

        float Length() const
        {
            return (float)sqrt(LengthSquared());
        }

        static RTNormal3DImpl Normalise(RTNormal3DImpl const& n)
        {
            return n / n.Length();
        }

        /*
            Operator overloads
        */

        // Using the [] operator to iterate over the member variables like an array.
        float& operator[] (int i)
        {
            return ((&x)[i]);
        }

        float const& operator[] (int i) const
        {
            return ((&x)[i]);
        }

        // Assignment operator
        RTNormal3DImpl const& operator =(RTNormal3DImpl const& n)
        {
            x = n.x;
            y = n.y;
            z = n.z;
            return *this;
        }

        /*
            Addition and substraction
        */
        RTNormal3DImpl operator +(RTNormal3DImpl const& n) const
        {
            return RTNormal3DImpl{ x + n.x, y + n.y, z + n.z };
        }

        RTNormal3DImpl const& operator +=(RTNormal3DImpl const& n)
        {
            x += n.x;
            y += n.y;
            z += n.z;
            return *this;
        }

        RTNormal3DImpl operator -(RTNormal3DImpl const& n) const
        {
            return RTNormal3DImpl{ x - n.x, y - n.y, z - n.z };
        }

        RTNormal3DImpl const& operator -=(RTNormal3DImpl const& n)
        {
            x -= n.x;
            y -= n.y;
            z -= n.z;
            return *this;
        }

        /*
            Scalar multiplication and division
        */
        RTNormal3DImpl operator *(float s) const
        {
            return RTNormal3DImpl{ x * s, y * s, z * s };
        }

        RTNormal3DImpl const& operator *=(float s)
        {
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }

        RTNormal3DImpl operator /(float s) const
        {
            s = 1 / s;
            return RTNormal3DImpl{ x * s, y * s, z * s };
        }

        RTNormal3DImpl const& operator /=(float s)
        {
            s = 1 / s;
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }

        // Member variables
        float x, y, z;
    };

    /*
        Operator overloads
    */
    bool operator ==(RTNormal3DImpl const& a, RTNormal3DImpl const& b)
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    // !=
    bool operator !=(RTNormal3DImpl const& a, RTNormal3DImpl const& b)
    {
        return !(a == b);
    }
}
