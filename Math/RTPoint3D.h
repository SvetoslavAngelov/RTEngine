#pragma once

#include "RTPoint2D.h"
#include "RTVec2D.h"
#include "RTVec3D.h"
#include <algorithm>

struct RTPoint3D
{
    // Default constructor
    RTPoint3D() = default;

    // Takes two x, y coordinates
    RTPoint3D(float a, float b, float c) :
        x{ a }, y{ b }, z{ c }
    {

    }

    // Takes another Point2D object
    RTPoint3D(RTPoint3D const& p) :
        x{ p.x }, y{ p.y }, z{ p.z }
    {

    }

    // Takes a RTVec2D object, z component is initialised to 0
    explicit RTPoint3D(RTVec2D const& v) :
        x{ v.x }, y{ v.y }, z{ 0.f }
    {

    }

    // Takes a RTPoint2D object, z component is initialised to 0
    explicit RTPoint3D(RTPoint2D const& p) :
        x{ p.x }, y{ p.y }, z{ 0.f }
    {

    }

    // Takes a RTVec3D object
    explicit RTPoint3D(RTVec3D const& v) :
        x{ v.x }, y{ v.y }, z{ v.z }
    {

    }

    /*
        Operator overloads
    */ 

    float& operator [](int i)
    {
        return ((&x)[i]);
    }

    float const& operator [](int i) const
    {
        return ((&x)[i]);
    }

    // Assignemt operator
    RTPoint3D const& operator =(RTPoint3D const& p)
    {
        x = p.x;
        y = p.y;
        z = p.z;
        return *this;
    }

    /*
        Addition and substraction.
    */

    // Adding a vector to a point offsets the point in a given direction, resulting in a new point in space. 
    RTPoint3D operator +(RTVec3D const& v) const
    {
        return RTPoint3D{ x + v.x, y + v.y, z + v.z };
    }

    // Substracting two points returns the vector between them. 
    RTVec3D operator -(RTPoint3D const& p) const
    {
        return RTVec3D{ x - p.x, y - p.y, z - p.z };
    }

    // Adding a vector to a point offsets the point in a given direction, resulting in a new point in space. 
    RTPoint3D const& operator +=(RTVec3D const& p)
    {
        x += p.x;
        y += p.y;
        z += p.z;
        return *this;
    }

    // Substracting a point from a vector returns a new point with the vector offset applied. 
    RTPoint3D const& operator -=(RTVec3D const& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    /*
        Scalar multiplication and division
    */

    RTPoint3D operator *(float s) const
    {
        return RTPoint3D{ x * s, y * s, z * s };
    }

    RTPoint3D operator /(float s) const
    {
        s = 1 / s;
        return RTPoint3D{ x * s, y * s, z * s };
    }

    RTPoint3D const& operator *=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    RTPoint3D const& operator /=(float s)
    {
        s = 1 / s;
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    /*
        Comparison operators
    */

    friend bool operator ==(RTPoint3D const& a, RTPoint3D const& b)
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    friend bool operator !=(RTPoint3D const& a, RTPoint3D const& b)
    {
        return !(a == b);
    }

    // Member variables
    float x, y, z;
};