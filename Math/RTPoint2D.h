#pragma once

#include "RTVector2D.h"

namespace RTPoint2D {

    struct RTPoint2DImpl
    {
        using RTVec2D = RTVector2D::RTVec2DImpl;

        // Default constructor
        RTPoint2DImpl() = default;

        // Takes two x, y coordinates
        RTPoint2DImpl(float a, float b) :
            x{ a }, y{ b }
        {

        }

        // Takes another Point2D object
        RTPoint2DImpl(RTPoint2DImpl const& p) :
            x{ p.x }, y{ p.y }
        {

        }

        // Takes a RTVec2D object
        explicit RTPoint2DImpl(RTVec2D const& p) :
            x{ p.x }, y{ p.y }
        {

        }

        /*
            Operator overloads.
        */

        // Using the [] operator to iterate over the member variables like an array.
        float& operator [](int i)
        {
            return ((&x)[i]);
        }

        float const& operator [](int i) const
        {
            return ((&x)[i]);
        }

        RTPoint2DImpl const& operator =(RTPoint2DImpl const& p)
        {
            x = p.x;
            y = p.y;
            return *this;
        }

        /*
            Addition and substraction
        */

        // Adding a vector to a point offsets the point in a given direction, resulting in a new point in space.
        RTPoint2DImpl operator +(RTVec2D const& v) const
        {
            return RTPoint2DImpl{ x + v.x, y + v.y };
        }

        // Substracting two points returns the vector between them. 
        RTVec2D operator -(RTPoint2DImpl const& p) const
        {
            return RTVec2D{ x - p.x, y - p.y };
        }

        // Adding a vector to a point offsets the point in a given direction, resulting in a new point in space. 
        RTPoint2DImpl const& operator +=(RTVec2D const& v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }

        // Substracting a point from a vector returns a new point with the vector offset applied. 
        RTPoint2DImpl const& operator -=(RTVec2D const& v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        /*
            Scalar multiplication and division
        */
        RTPoint2DImpl operator *(float s) const
        {
            return RTPoint2DImpl{ x * s, y * s };
        }

        RTPoint2DImpl operator /(float s) const
        {
            s = 1 / s;
            return RTPoint2DImpl{ x * s, y * s };
        }

        RTPoint2DImpl const& operator *=(float s)
        {
            x *= s;
            y *= s;
            return *this;
        }

        RTPoint2DImpl const& operator /=(float s)
        {
            s = 1 / s;
            x *= s;
            y *= s;
            return *this;
        }

        // Member variables
        float x, y;
    };

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

