#include "RTVector4D.h"
#include <math.h>

RTVector4D::RTVector4D(float a, float b, float c, float d) :
    x{ a }, y{ b }, z{ c }, w{ d }
{
}

RTVector4D::RTVector4D(RTVector4D const& v) :
    x{ v.x }, y{ v.y }, z{ v.z }, w{ v.w }
{
}

bool RTVector4D::isZeroVector() const
{
    return (x == 0.f && y == 0.f && z == 0.f && w == 0.f);
}

float RTVector4D::Magnitude() const
{
    float s = (x * x) + (y * y) + (z * z) + (w * w);
    return (float)sqrt(s);
}

RTVector4D RTVector4D::GetNormal() const
{
    return !isZeroVector() ? RTVector4D{ *this / this->Magnitude() } : RTVector4D{ 0.f, 0.f, 0.f, 0.f };
}

RTVector4D RTVector4D::GetUnsafeNormal() const
{
    return RTVector4D{ *this / this->Magnitude() };
}

float& RTVector4D::operator[] (int i)
{
    return ((&x)[i]);
}

float const& RTVector4D::operator[] (int i) const
{
    return ((&x)[i]);
}

RTVector4D const& RTVector4D::operator =(RTVector4D const& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    return *this;
}

RTVector4D const& RTVector4D::operator *=(float s)
{
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
}

RTVector4D const& RTVector4D::operator /=(float s)
{
    s = 1.f / s;
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
}

RTVector4D RTVector4D::operator *(float s) const
{
    return { x * s, y * s, z * s, w * s };
}

RTVector4D RTVector4D::operator /(float s) const
{
    s = 1.f / s;
    return { x * s, y * s, z * s, w * s };
}

RTVector4D& RTVector4D::operator +=(RTVector4D const& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return (*this);
}

RTVector4D& RTVector4D::operator -=(RTVector4D const& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return (*this);
}

RTVector4D RTVector4D::operator +(RTVector4D const& v) const
{
    return { x + v.x, y + v.y, z + v.z, w + v.w };
}

RTVector4D RTVector4D::operator -(RTVector4D const& v) const
{
    return { x - v.x, y - v.y, z - v.z, w - v.w };
}