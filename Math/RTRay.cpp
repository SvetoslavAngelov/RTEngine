#include "RTRay.h"
#include <limits>

// Default constructor, which initialises the Ray with length of infinity.
RTRay::RTRay() :
    o{ 0.f, 0.f, 0.f }, d{ 0.f, 0.f, 0.f }, t{ 0.f }, length{ std::numeric_limits<float>::infinity() }
{
}

// Constructor taking an origin point and direction vector, with an optional length parameter. By default the length of the ray is set to infinity.
RTRay::RTRay(RTPoint3D const& origin, RTVec3D const& direction, float ray_length) :
    o{ origin }, d{ direction }, t{ 0.f }, length{ ray_length }
{
}

/*
    Member functions
    */

    // Returns a new intersection point along the ray at point t. 
RTRay::RTPoint3D RTRay::Position(float time) const
{
    return { o + d * time };
}