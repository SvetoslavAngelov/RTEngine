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
    explicit RTPoint3D(RTPoint3D const& p) :
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

    // Returns the distance between two points. 
    static float Distance(RTPoint3D const& a, RTPoint3D const& b)
    {
        return (a - b).Magnitude();
    }

    // Returns a new point with the component min values between two points.
    static RTPoint3D Min(RTPoint3D const& a, RTPoint3D const& b)
    {
        return RTPoint3D{
            std::min<float>(a.x, b.x),
            std::min<float>(a.y, b.y),
            std::min<float>(a.z, b.z)
        };
    }

    // Returns a new point with the component max values between two points. 
    static RTPoint3D Max(RTPoint3D const& a, RTPoint3D const& b)
    {
        return RTPoint3D{
            std::max<float>(a.x, b.x),
            std::max<float>(a.y, b.y),
            std::max<float>(a.z, b.z)
        };
    }

    // Operator overloads

    float& operator [](int i)
    {
        return ((&x)[i]);
    }

    RTPoint3D const& operator =(RTPoint3D const& p)
    {
        x = p.x;
        y = p.y;
        z = p.z;
        return *this;
    }

    // Adding a vector to a point offsets the point in a given direction, resulting in a new point in space. 
    RTPoint3D const& operator +(RTVec3D const& v) const
    {
        return RTPoint3D{ x + v.x, y + v.y, z + v.z };
    }

    // Substracting two points returns the vector between them. 
    RTVec3D const& operator -(RTPoint3D const& p) const
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

    RTPoint3D const& operator -(RTVec3D const& v) const
    {
        return RTPoint3D{ x - v.x, y - v.y, z - v.z };
    }

    // Substracting a point from a vector returns a new point with the vector offset applied. 
    RTPoint3D const& operator -=(RTVec3D const& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    RTPoint3D const& operator *(float s) const
    {
        return RTPoint3D{ x * s, y * s, z * s };
    }

    RTPoint3D const& operator /(float s) const
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

    friend bool operator ==(RTPoint3D const& a, RTPoint3D const& b)
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    friend bool operator !=(RTPoint3D const& a, RTPoint3D const& b)
    {
        return !(a == b);
    }

    friend RTPoint3D operator +(RTPoint3D const& p, RTVec3D const& v)
    {
        return RTPoint3D{ p.x + v.x, p.y + v.y, p.z + v.z };
    }

    friend RTVec3D operator -(RTPoint3D const& a, RTPoint3D const& b)
    {
        return RTVec3D{ a.x - b.x, a.y - b.y, a.z - b.z };
    }

    // Member variables
    float x, y, z;
};