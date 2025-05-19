#pragma once

#include "RTVector3D.h"

namespace RTNormal3D {

    struct RTNormal3DImpl {

        using RTVec3D = RTVector3D::RTVec3DImpl;

        // Default constructor
        RTNormal3DImpl() = default;

        // Component constructor
        RTNormal3DImpl(float a, float b, float c);

        // Vector constructor
        explicit RTNormal3DImpl(RTVec3D const& v);

        /*
            Member functions
        */

        float LengthSquared() const;
        float Length() const;
        static RTNormal3DImpl Normalise(RTNormal3DImpl const& n);

        /*
            Operator overloads
        */

        // Using the [] operator to iterate over the member variables like an array.
        float& operator[] (int i);
        float const& operator[] (int i) const;

        // Assignment operator
        RTNormal3DImpl const& operator =(RTNormal3DImpl const& n);

        /*
            Addition and substraction
        */
        RTNormal3DImpl operator +(RTNormal3DImpl const& n) const;
        RTNormal3DImpl const& operator +=(RTNormal3DImpl const& n);
        RTNormal3DImpl operator -(RTNormal3DImpl const& n) const;
        RTNormal3DImpl const& operator -=(RTNormal3DImpl const& n);

        /*
            Scalar multiplication and division
        */
        RTNormal3DImpl operator *(float s) const;
        RTNormal3DImpl const& operator *=(float s);
        RTNormal3DImpl operator /(float s) const;
        RTNormal3DImpl const& operator /=(float s);

        // Member variables
        float x, y, z;
    };

    /*
        Operator overloads
    */
    bool operator ==(RTNormal3DImpl const& a, RTNormal3DImpl const& b);
    bool operator !=(RTNormal3DImpl const& a, RTNormal3DImpl const& b);
}
