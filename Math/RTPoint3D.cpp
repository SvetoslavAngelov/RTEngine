#include "RTPoint3D.h"
#include <algorithm>

namespace RTPoint3D {

    // Takes x, y, z coordinates
    RTPoint3DImpl::RTPoint3DImpl(float a, float b, float c) :
        x{ a }, y{ b }, z{ c }
    {
    }

    // Takes another Point3D object
    RTPoint3DImpl::RTPoint3DImpl(RTPoint3DImpl const& p) :
        x{ p.x }, y{ p.y }, z{ p.z }
    {
    }

    // Takes a RTVec2D object, z component is initialised to 0
    RTPoint3DImpl::RTPoint3DImpl(RTVec2D const& v) :
        x{ v.x }, y{ v.y }, z{ 0.f }
    {
    }

    // Takes a RTPoint2D object, z component is initialised to 0
    RTPoint3DImpl::RTPoint3DImpl(RTPoint2D const& p) :
        x{ p.x }, y{ p.y }, z{ 0.f }
    {
    }

    // Takes a RTVec3D object
    RTPoint3DImpl::RTPoint3DImpl(RTVec3D const& v) :
        x{ v.x }, y{ v.y }, z{ v.z }
    {
    }

    /*
        Operator overloads
    */

    float& RTPoint3DImpl::operator [](int i)
    {
        return ((&x)[i]);
    }

    float const& RTPoint3DImpl::operator [](int i) const
    {
        return ((&x)[i]);
    }

    // Assignment operator
    RTPoint3DImpl const& RTPoint3DImpl::operator =(RTPoint3DImpl const& p)
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
    RTPoint3DImpl RTPoint3DImpl::operator +(RTVec3D const& v) const
    {
        return RTPoint3DImpl{ x + v.x, y + v.y, z + v.z };
    }

    // Substracting two points returns the vector between them. 
    RTPoint3DImpl::RTVec3D RTPoint3DImpl::operator -(RTPoint3DImpl const& p) const
    {
        return RTVec3D{ x - p.x, y - p.y, z - p.z };
    }

    // Adding a vector to a point offsets the point in a given direction, resulting in a new point in space. 
    RTPoint3DImpl const& RTPoint3DImpl::operator +=(RTVec3D const& p)
    {
        x += p.x;
        y += p.y;
        z += p.z;
        return *this;
    }

    // Substracting a point from a vector returns a new point with the vector offset applied. 
    RTPoint3DImpl const& RTPoint3DImpl::operator -=(RTVec3D const& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    /*
        Scalar multiplication and division
    */

    RTPoint3DImpl RTPoint3DImpl::operator *(float s) const
    {
        return RTPoint3DImpl{ x * s, y * s, z * s };
    }

    RTPoint3DImpl RTPoint3DImpl::operator /(float s) const
    {
        s = 1 / s;
        return RTPoint3DImpl{ x * s, y * s, z * s };
    }

    RTPoint3DImpl const& RTPoint3DImpl::operator *=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    RTPoint3DImpl const& RTPoint3DImpl::operator /=(float s)
    {
        s = 1 / s;
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    /*
        Point operations
    */

    // Returns the distance between two points. 
    float Distance(RTPoint3DImpl const& a, RTPoint3DImpl const& b)
    {
        return (a - b).Magnitude();
    }

    // Returns a new point with the component min values between two points.
    RTPoint3DImpl Min(RTPoint3DImpl const& a, RTPoint3DImpl const& b)
    {
        return RTPoint3DImpl{
            std::min<float>(a.x, b.x),
            std::min<float>(a.y, b.y),
            std::min<float>(a.z, b.z)
        };
    }

    // Returns a new point with the component max values between two points. 
    RTPoint3DImpl Max(RTPoint3DImpl const& a, RTPoint3DImpl const& b)
    {
        return RTPoint3DImpl{
            std::max<float>(a.x, b.x),
            std::max<float>(a.y, b.y),
            std::max<float>(a.z, b.z)
        };
    }

    /*
        Operator overloads
    */

    bool operator ==(RTPoint3DImpl const& a, RTPoint3DImpl const& b)
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    bool operator !=(RTPoint3DImpl const& a, RTPoint3DImpl const& b)
    {
        return !(a == b);
    }
}