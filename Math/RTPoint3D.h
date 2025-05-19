#pragma once

#include "RTPoint2D.h"
#include "RTVector2D.h"
#include "RTVector3D.h"
#include <algorithm>

namespace RTPoint3D {

    struct RTPoint3DImpl
    {
        using RTVec2D = RTVector2D::RTVec2DImpl;
        using RTVec3D = RTVector3D::RTVec3DImpl;
        using RTPoint2D = RTPoint2D::RTPoint2DImpl;

        // Default constructor
        RTPoint3DImpl() = default;

        // Takes two x, y coordinates
        RTPoint3DImpl(float a, float b, float c);

        // Takes another Point2D object
        RTPoint3DImpl(RTPoint3DImpl const& p);

        // Takes a RTVec2D object, z component is initialised to 0
        explicit RTPoint3DImpl(RTVec2D const& v);

        // Takes a RTPoint2D object, z component is initialised to 0
        explicit RTPoint3DImpl(RTPoint2D const& p);

        // Takes a RTVec3D object
        explicit RTPoint3DImpl(RTVec3D const& v);

        /*
            Operator overloads
        */

        float& operator [](int i);
        float const& operator [](int i) const;

        // Assignemt operator
        RTPoint3DImpl const& operator =(RTPoint3DImpl const& p);

        /*
            Addition and substraction.
        */

        // Adding a vector to a point offsets the point in a given direction, resulting in a new point in space. 
        RTPoint3DImpl operator +(RTVec3D const& v) const;

        // Substracting two points returns the vector between them. 
        RTVec3D operator -(RTPoint3DImpl const& p) const;

        // Adding a vector to a point offsets the point in a given direction, resulting in a new point in space. 
        RTPoint3DImpl const& operator +=(RTVec3D const& p);

        // Substracting a point from a vector returns a new point with the vector offset applied. 
        RTPoint3DImpl const& operator -=(RTVec3D const& v);

        /*
            Scalar multiplication and division
        */

        RTPoint3DImpl operator *(float s) const;
        RTPoint3DImpl operator /(float s) const;
        RTPoint3DImpl const& operator *=(float s);
        RTPoint3DImpl const& operator /=(float s);

        // Member variables
        float x, y, z;
    };

    /*
        Point operations
    */

    // Returns the distance between two points. 
    float Distance(RTPoint3DImpl const& a, RTPoint3DImpl const& b);

    // Returns a new point with the component min values between two points.
    RTPoint3DImpl Min(RTPoint3DImpl const& a, RTPoint3DImpl const& b);

    // Returns a new point with the component max values between two points. 
    RTPoint3DImpl Max(RTPoint3DImpl const& a, RTPoint3DImpl const& b);

    /*
        Operator overloads
    */

    bool operator ==(RTPoint3DImpl const& a, RTPoint3DImpl const& b);
    bool operator !=(RTPoint3DImpl const& a, RTPoint3DImpl const& b);
}
