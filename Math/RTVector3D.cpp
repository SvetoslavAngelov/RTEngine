#include "RTVector3D.h"
#include <math.h>

namespace RTVector3D {

    RTVec3DImpl::RTVec3DImpl(float a, float b, float c) :
        x{ a }, y{ b }, z{ c }
    {
    }

    RTVec3DImpl::RTVec3DImpl(RTVec3DImpl const& v) :
        x{ v.x }, y{ v.y }, z{ v.z }
    {
    }

    bool RTVec3DImpl::isZeroVector() const
    {
        return (x == 0.f && y == 0.f && z == 0.f);
    }

    float RTVec3DImpl::Magnitude() const
    {
        float s = (x * x) + (y * y) + (z * z);
        return (float)sqrt(s);
    }

    RTVec3DImpl RTVec3DImpl::GetNormal() const
    {
        return !isZeroVector() ? RTVec3DImpl{ *this / this->Magnitude() } : RTVec3DImpl{ 0.f, 0.f, 0.f };
    }

    RTVec3DImpl RTVec3DImpl::GetUnsafeNormal() const
    {
        return RTVec3DImpl{ *this / this->Magnitude() };
    }

    float& RTVec3DImpl::operator[] (int i)
    {
        return ((&x)[i]);
    }

    float const& RTVec3DImpl::operator[] (int i) const
    {
        return ((&x)[i]);
    }

    RTVec3DImpl const& RTVec3DImpl::operator =(RTVec3DImpl const& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    RTVec3DImpl const& RTVec3DImpl::operator *=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    RTVec3DImpl const& RTVec3DImpl::operator /=(float s)
    {
        s = 1.f / s;
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    RTVec3DImpl RTVec3DImpl::operator *(float s) const
    {
        return { x * s, y * s, z * s };
    }

    RTVec3DImpl RTVec3DImpl::operator /(float s) const
    {
        s = 1.f / s;
        return { x * s, y * s, z * s };
    }

    RTVec3DImpl& RTVec3DImpl::operator +=(RTVec3DImpl const& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return (*this);
    }

    RTVec3DImpl& RTVec3DImpl::operator -=(RTVec3DImpl const& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return (*this);
    }

    RTVec3DImpl RTVec3DImpl::operator +(RTVec3DImpl const& v) const
    {
        return { x + v.x, y + v.y, z + v.z };
    }

    RTVec3DImpl RTVec3DImpl::operator -(RTVec3DImpl const& v) const
    {
        return { x - v.x, y - v.y, z - v.z };
    }

    /*
    Vector operations
    */

    // Returns the dot product between two 3D vectors. 
    float DotProduct(RTVec3DImpl const& a, RTVec3DImpl const& b)
    {
        return { a.x * b.x + a.y * b.y + a.z * b.z };
    }

    // Retruns the cross product between two 3D vectors. 
    RTVec3DImpl CrossProduct(RTVec3DImpl const& a, RTVec3DImpl const& b)
    {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    // Returns a new 3D vector which is a projection of vector a onto vector b.
    RTVec3DImpl Projection(RTVec3DImpl const& a, RTVec3DImpl const& b)
    {
        return { b * (DotProduct(a,b) / DotProduct(b,b)) };
    }

    // Returns a new 3D vector which is a rejection of vector a from b and is perpendicular to b. 
    RTVec3DImpl Rejection(RTVec3DImpl const& a, RTVec3DImpl const& b)
    {
        return { (a - b) * (DotProduct(a,b) / DotProduct(b,b)) };
    }

    /*
        Operator overloads
    */
    bool operator ==(RTVec3DImpl const& a, RTVec3DImpl const& b)
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    bool operator !=(RTVec3DImpl const& a, RTVec3DImpl const& b)
    {
        return !(a == b);
    }
}