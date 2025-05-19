#include "RTNormal3D.h"
#include <math.h>

namespace RTNormal3D {

    // Component constructor
    RTNormal3DImpl::RTNormal3DImpl(float a, float b, float c) :
        x{ a }, y{ b }, z{ c }
    {
    }

    // Vector constructor
    RTNormal3DImpl::RTNormal3DImpl(RTVec3D const& v) :
        x{ v.x }, y{ v.y }, z{ v.z }
    {
    }

    /*
        Member functions
    */

    float RTNormal3DImpl::LengthSquared() const
    {
        return { x * x + y * y + z * z };
    }

    float RTNormal3DImpl::Length() const
    {
        return (float)sqrt(LengthSquared());
    }

    RTNormal3DImpl RTNormal3DImpl::Normalise(RTNormal3DImpl const& n)
    {
        return n / n.Length();
    }

    /*
        Operator overloads
    */

    // Using the [] operator to iterate over the member variables like an array.
    float& RTNormal3DImpl::operator[] (int i)
    {
        return ((&x)[i]);
    }

    float const& RTNormal3DImpl::operator[] (int i) const
    {
        return ((&x)[i]);
    }

    // Assignment operator
    RTNormal3DImpl const& RTNormal3DImpl::operator =(RTNormal3DImpl const& n)
    {
        x = n.x;
        y = n.y;
        z = n.z;
        return *this;
    }

    /*
        Addition and substraction
    */
    RTNormal3DImpl RTNormal3DImpl::operator +(RTNormal3DImpl const& n) const
    {
        return RTNormal3DImpl{ x + n.x, y + n.y, z + n.z };
    }

    RTNormal3DImpl const& RTNormal3DImpl::operator +=(RTNormal3DImpl const& n)
    {
        x += n.x;
        y += n.y;
        z += n.z;
        return *this;
    }

    RTNormal3DImpl RTNormal3DImpl::operator -(RTNormal3DImpl const& n) const
    {
        return RTNormal3DImpl{ x - n.x, y - n.y, z - n.z };
    }

    RTNormal3DImpl const& RTNormal3DImpl::operator -=(RTNormal3DImpl const& n)
    {
        x -= n.x;
        y -= n.y;
        z -= n.z;
        return *this;
    }

    /*
        Scalar multiplication and division
    */
    RTNormal3DImpl RTNormal3DImpl::operator *(float s) const
    {
        return RTNormal3DImpl{ x * s, y * s, z * s };
    }

    RTNormal3DImpl const& RTNormal3DImpl::operator *=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    RTNormal3DImpl RTNormal3DImpl::operator /(float s) const
    {
        s = 1 / s;
        return RTNormal3DImpl{ x * s, y * s, z * s };
    }

    RTNormal3DImpl const& RTNormal3DImpl::operator /=(float s)
    {
        s = 1 / s;
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

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