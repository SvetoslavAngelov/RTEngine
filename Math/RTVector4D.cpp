#include "RTVector4D.h"
#include <math.h>

namespace RTVector4D {

    RTVec4DImpl::RTVec4DImpl(float a, float b, float c, float d) :
        x{ a }, y{ b }, z{ c }, w{ d }
    {
    }

    RTVec4DImpl::RTVec4DImpl(RTVec4DImpl const& v) :
        x{ v.x }, y{ v.y }, z{ v.z }, w{ v.w }
    {
    }

    bool RTVec4DImpl::isZeroVector() const
    {
        return (x == 0.f && y == 0.f && z == 0.f && w == 0.f);
    }

    float RTVec4DImpl::Magnitude() const
    {
        float s = (x * x) + (y * y) + (z * z) + (w * w);
        return (float)sqrt(s);
    }

    RTVec4DImpl RTVec4DImpl::GetNormal() const
    {
        return !isZeroVector() ? RTVec4DImpl{ *this / this->Magnitude() } : RTVec4DImpl{ 0.f, 0.f, 0.f, 0.f };
    }

    RTVec4DImpl RTVec4DImpl::GetUnsafeNormal() const
    {
        return RTVec4DImpl{ *this / this->Magnitude() };
    }

    float& RTVec4DImpl::operator[] (int i)
    {
        return ((&x)[i]);
    }

    float const& RTVec4DImpl::operator[] (int i) const
    {
        return ((&x)[i]);
    }

    RTVec4DImpl const& RTVec4DImpl::operator =(RTVec4DImpl const& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    RTVec4DImpl const& RTVec4DImpl::operator *=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
        return *this;
    }

    RTVec4DImpl const& RTVec4DImpl::operator /=(float s)
    {
        s = 1.f / s;
        x *= s;
        y *= s;
        z *= s;
        w *= s;
        return *this;
    }

    RTVec4DImpl RTVec4DImpl::operator *(float s) const
    {
        return { x * s, y * s, z * s, w * s };
    }

    RTVec4DImpl RTVec4DImpl::operator /(float s) const
    {
        s = 1.f / s;
        return { x * s, y * s, z * s, w * s };
    }

    RTVec4DImpl& RTVec4DImpl::operator +=(RTVec4DImpl const& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return (*this);
    }

    RTVec4DImpl& RTVec4DImpl::operator -=(RTVec4DImpl const& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return (*this);
    }

    RTVec4DImpl RTVec4DImpl::operator +(RTVec4DImpl const& v) const
    {
        return { x + v.x, y + v.y, z + v.z, w + v.w };
    }

    RTVec4DImpl RTVec4DImpl::operator -(RTVec4DImpl const& v) const
    {
        return { x - v.x, y - v.y, z - v.z, w - v.w };
    }
}