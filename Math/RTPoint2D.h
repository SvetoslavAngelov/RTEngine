#pragma once

#include "RTVector2D.h"

namespace RTPoint2D {

    struct RTPoint2DImpl
    {
        using RTVec2D = RTVector2D::RTVec2DImpl;

        // Default constructor
        RTPoint2DImpl() = default;

        // Takes two x, y coordinates
        RTPoint2DImpl(float a, float b);

        // Takes another Point2D object
        RTPoint2DImpl(RTPoint2DImpl const& p);

        // Takes a RTVec2D object
        explicit RTPoint2DImpl(RTVec2D const& p);

        /*
            Operator overloads.
        */

        // Using the [] operator to iterate over the member variables like an array.
        float& operator [](int i);
        float const& operator [](int i) const;
        RTPoint2DImpl const& operator =(RTPoint2DImpl const& p);

        /*
            Addition and substraction
        */

        // Adding a vector to a point offsets the point in a given direction, resulting in a new point in space.
        RTPoint2DImpl operator +(RTVec2D const& v) const;

        // Substracting two points returns the vector between them. 
        RTVec2D operator -(RTPoint2DImpl const& p) const;

        // Adding a vector to a point offsets the point in a given direction, resulting in a new point in space. 
        RTPoint2DImpl const& operator +=(RTVec2D const& v);

        // Substracting a point from a vector returns a new point with the vector offset applied. 
        RTPoint2DImpl const& operator -=(RTVec2D const& v);

        /*
            Scalar multiplication and division
        */
        RTPoint2DImpl operator *(float s) const;
        RTPoint2DImpl operator /(float s) const;
        RTPoint2DImpl const& operator *=(float s);
        RTPoint2DImpl const& operator /=(float s);

        // Member variables
        float x, y;
    };

    /*
        Operator overloads
    */
    bool operator ==(RTPoint2DImpl const& a, RTPoint2DImpl const& b);
    bool operator !=(RTPoint2DImpl const& a, RTPoint2DImpl const& b);
}

