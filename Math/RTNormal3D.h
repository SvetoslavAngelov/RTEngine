#pragma once

#include "RTVec3D.h"

struct RTNormal3D {

    // Default constructor
    RTNormal3D() = default;

    // Component constructor
    RTNormal3D(float a, float b, float c) :
        x{ a }, y{ b }, z{ c }
    {

    }

    // Vector constructor
    explicit RTNormal3D(RTVec3D const& v) :
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
        return std::sqrt(LengthSquared());
    }

    static RTNormal3D Normalise(RTNormal3D const& n)
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
    RTNormal3D const& operator =(RTNormal3D const& n)
    {
        x = n.x;
        y = n.y;
        z = n.z;
        return *this;
    }

    /*
        Addition and substraction
    */
    RTNormal3D operator +(RTNormal3D const& n) const
    {
        return RTNormal3D{ x + n.x, y + n.y, z + n.z };
    }

    RTNormal3D const& operator +=(RTNormal3D const& n)
    {
        x += n.x;
        y += n.y;
        z += n.z;
        return *this;
    }

    RTNormal3D operator -(RTNormal3D const& n) const
    {
        return RTNormal3D{ x - n.x, y - n.y, z - n.z };
    }

    RTNormal3D const& operator -=(RTNormal3D const& n)
    {
        x -= n.x;
        y -= n.y;
        z -= n.z;
        return *this;
    }

    /*
        Scalar multiplication and division
    */
    RTNormal3D operator *(float s) const
    {
        return RTNormal3D{ x * s, y * s, z * s };
    }

    RTNormal3D const& operator *=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    RTNormal3D operator /(float s) const
    {
        s = 1 / s;
        return RTNormal3D{ x * s, y * s, z * s };
    }

    RTNormal3D const& operator /=(float s)
    {
        s = 1 / s;
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    /*
        Comparison operator
    */
    friend bool operator ==(RTNormal3D const& a, RTNormal3D const& b)
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    // !=
    friend bool operator !=(RTNormal3D const& a, RTNormal3D const& b)
    {
        return !(a == b);
    }

    // Member variables
    float x, y, z;
};