#pragma once
#include "RTVec2D.h"

struct RTPoint2D
{
    // Default constructor
    RTPoint2D() = default;

    // Takes two x, y coordinates
    RTPoint2D(float a, float b) :
        x{ a }, y{ b }
    {

    }

    // Takes another Point2D object
    explicit RTPoint2D(RTPoint2D const& p) :
        x{ p.x }, y{ p.y }
    {

    }

    // Takes a RTVec2D object
    explicit RTPoint2D(RTVec2D const& p) :
        x{ p.x }, y{ p.y }
    {

    }
   
    // Operator overloads
    
    float& operator [](int i)
    {
        return ((&x)[i]);
    }

    RTPoint2D const& operator =(RTPoint2D const& p)
    {
        x = p.x;
        y = p.y;
        return *this;
    }

    // Adding a vector to a point offsets the point in a given direction, resulting in a new point in space.
    RTPoint2D const& operator +(RTVec2D const& v) const
    {
        return RTPoint2D{ x + v.x, y + v.y };
    }

    // Substracting two points returns the vector between them. 
    RTVec2D const& operator -(RTPoint2D const& p) const
    {
        return RTVec2D{ x - p.x, y - p.y };
    }

    // Adding a vector to a point offsets the point in a given direction, resulting in a new point in space. 
    RTPoint2D const& operator +=(RTVec2D const& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    // Substracting a point from a vector returns a new point with the vector offset applied. 
    RTPoint2D const& operator -=(RTVec2D const& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    RTPoint2D const& operator *(float s) const
    {
        return RTPoint2D{ x * s, y * s };
    }

    RTPoint2D const& operator /(float s) const
    {
        s = 1 / s; 
        return RTPoint2D{ x * s, y * s };
    }

    RTPoint2D const& operator *=(float s)
    {
        x *= s; 
        y *= s; 
        return *this;
    }

    RTPoint2D const& operator /=(float s)
    {
        s = 1 / s; 
        x *= s; 
        y *= s;
        return *this; 
    }

    friend bool operator ==(RTPoint2D const& a, RTPoint2D const& b)
    {
        return a.x == b.x && a.y == b.y;
    }

    friend bool operator !=(RTPoint2D const& a, RTPoint2D const& b)
    {
        return !(a == b);
    }

     // Member variables
     float x, y;
};
