#include "RTPoint2D.h"

namespace RTPoint2D {

    // Takes two x, y coordinates
    RTPoint2DImpl::RTPoint2DImpl(float a, float b) :
        x{ a }, y{ b }
    {
    }

    // Takes another Point2D object
    RTPoint2DImpl::RTPoint2DImpl(RTPoint2DImpl const& p) :
        x{ p.x }, y{ p.y }
    {
    }

    // Takes a RTVec2D object
    RTPoint2DImpl::RTPoint2DImpl(RTVec2D const& p) :
        x{ p.x }, y{ p.y }
    {
    }

    // Using the [] operator to iterate over the member variables like an array.
    float& RTPoint2DImpl::operator [](int i)
    {
        return ((&x)[i]);
    }

    float const& RTPoint2DImpl::operator [](int i) const
    {
        return ((&x)[i]);
    }

    RTPoint2DImpl const& RTPoint2DImpl::operator =(RTPoint2DImpl const& p)
    {
        x = p.x;
        y = p.y;
        return *this;
    }

    // Adding a vector to a point offsets the point in a given direction, resulting in a new point in space.
    RTPoint2DImpl RTPoint2DImpl::operator +(RTVec2D const& v) const
    {
        return RTPoint2DImpl{ x + v.x, y + v.y };
    }

    // Substracting two points returns the vector between them. 
    RTPoint2DImpl::RTVec2D RTPoint2DImpl::operator -(RTPoint2DImpl const& p) const
    {
        return RTVec2D{ x - p.x, y - p.y };
    }

    // Adding a vector to a point offsets the point in a given direction, resulting in a new point in space. 
    RTPoint2DImpl const& RTPoint2DImpl::operator +=(RTVec2D const& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    // Substracting a point from a vector returns a new point with the vector offset applied. 
    RTPoint2DImpl const& RTPoint2DImpl::operator -=(RTVec2D const& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    /*
        Scalar multiplication and division
    */
    RTPoint2DImpl RTPoint2DImpl::operator *(float s) const
    {
        return RTPoint2DImpl{ x * s, y * s };
    }

    RTPoint2DImpl RTPoint2DImpl::operator /(float s) const
    {
        s = 1 / s;
        return RTPoint2DImpl{ x * s, y * s };
    }

    RTPoint2DImpl const& RTPoint2DImpl::operator *=(float s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    RTPoint2DImpl const& RTPoint2DImpl::operator /=(float s)
    {
        s = 1 / s;
        x *= s;
        y *= s;
        return *this;
    }

    /*
        Operator overloads
    */
    bool operator ==(RTPoint2DImpl const& a, RTPoint2DImpl const& b)
    {
        return a.x == b.x && a.y == b.y;
    }

    bool operator !=(RTPoint2DImpl const& a, RTPoint2DImpl const& b)
    {
        return !(a == b);
    }
}