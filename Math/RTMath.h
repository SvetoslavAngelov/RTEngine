#pragma once

#include "RTVec3D.h"
#include "RTPoint3D.h"	

/*
	Vector operations
*/

// Returns the dot product between two 3D vectors. 
float DotProduct(RTVec3D const& a, RTVec3D const& b)
{
	return { a.x * b.x + a.y * b.y + a.z * b.z };
}

// Retruns the cross product between two 3D vectors. 
RTVec3D CrossProduct(RTVec3D const& a, RTVec3D const& b)
{
	return {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

// Returns a new 3D vector which is a projection of vector a onto vector b.
RTVec3D Projection(RTVec3D const& a, RTVec3D const& b)
{
	return { b * (DotProduct(a,b) / DotProduct(b,b)) };
}

// Returns a new 3D vector which is a rejection of vector a from b and is perpendicular to b. 
RTVec3D Rejection(RTVec3D const& a, RTVec3D const& b)
{
	return { (a - b) * (DotProduct(a,b) / DotProduct(b,b)) };
}

/*
	Point operations
*/

// Returns the distance between two points. 
float Distance(RTPoint3D const& a, RTPoint3D const& b)
{
	return (a - b).Magnitude();
}

// Returns a new point with the component min values between two points.
RTPoint3D Min(RTPoint3D const& a, RTPoint3D const& b)
{
	return RTPoint3D{
		std::min<float>(a.x, b.x),
		std::min<float>(a.y, b.y),
		std::min<float>(a.z, b.z)
	};
}

// Returns a new point with the component max values between two points. 
RTPoint3D Max(RTPoint3D const& a, RTPoint3D const& b)
{
	return RTPoint3D{
		std::max<float>(a.x, b.x),
		std::max<float>(a.y, b.y),
		std::max<float>(a.z, b.z)
	};
}

