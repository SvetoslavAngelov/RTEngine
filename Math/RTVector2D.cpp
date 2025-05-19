#include "RTVector2D.h"

namespace RTVector2D {

    RTVec2DImpl::RTVec2DImpl(float a, float b) :
        x{ a }, y{ b }
    {
    }

    RTVec2DImpl::RTVec2DImpl(RTVec2DImpl const& v) :
        x{ v.x }, y{ v.y }
    {
    }

    bool RTVec2DImpl::isZeroVector() const
    {
        return (x == 0.f && y == 0.f);
    }

    float& RTVec2DImpl::operator[] (int i)
    {
        return ((&x)[i]);
    }

    float const& RTVec2DImpl::operator[] (int i) const
    {
        return ((&x)[i]);
    }

    RTVec2DImpl const& RTVec2DImpl::operator =(RTVec2DImpl const& v)
    {
        x = v.x;
        y = v.y;
        return *this;
    }

    RTVec2DImpl& RTVec2DImpl::operator +=(RTVec2DImpl const& v)
    {
        x += v.x;
        y += v.y;
        return (*this);
    }

    RTVec2DImpl& RTVec2DImpl::operator -=(RTVec2DImpl const& v)
    {
        x -= v.x;
        y -= v.y;
        return (*this);
    }

    RTVec2DImpl RTVec2DImpl::operator +(RTVec2DImpl const& v) const
    {
        return { x + v.x, y + v.y };
    }

    RTVec2DImpl RTVec2DImpl::operator -(RTVec2DImpl const& v) const
    {
        return { x - v.x, y - v.y };
    }

    RTVec2DImpl const& RTVec2DImpl::operator *=(float s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    RTVec2DImpl const& RTVec2DImpl::operator /=(float s)
    {
        s = 1.f / s;
        x *= s;
        y *= s;
        return *this;
    }

    RTVec2DImpl RTVec2DImpl::operator *(float s) const
    {
        return { x * s, y * s };
    }

    RTVec2DImpl RTVec2DImpl::operator /(float s) const
    {
        s = 1.f / s;
        return { x * s, y * s };
    }

    /*
        Operator overloads
    */

    bool operator ==(RTVec2DImpl const& a, RTVec2DImpl const& b)
    {
        return a.x == b.x && a.y == b.y;
    }

    bool operator !=(RTVec2DImpl const& a, RTVec2DImpl const& b)
    {
        return !(a == b);
    }
}