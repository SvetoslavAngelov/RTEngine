#pragma once

#include "RTVector3D.h"
#include "RTPoint3D.h"
#include <limits>

struct RTRay {

    using RTVec3D = RTVector3D::RTVec3DImpl;
    using RTPoint3D = RTPoint3D::RTPoint3DImpl;

    // Default constructor, which initialises the Ray with length of infinity.
    RTRay();

    // Constructor taking an origin point and direction vector, with an optional length parameter. By default the length of the ray is set to infinity.
    RTRay(RTPoint3D const& origin, RTVec3D const& direction, float ray_length = std::numeric_limits<float>::infinity());

    /*
        Member functions
     */

     // Returns a new intersection point along the ray at point t. 
    RTPoint3D Position(float time) const;

    /*
        Member variables
     */

     // Ray origin
    RTPoint3D o;

    // Ray direction
    RTVec3D d;

    // Time value, used to define the point intersection of the ray at time t.
    // R(t) = o + (t)d, given that 0 < t < infinity or std::numeric_limit<float>.
    float t;

    // The maximum length of the ray.
    // Used to determine the last possible intersection point.
    float length;
};