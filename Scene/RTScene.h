#pragma once

#include "../Math/RTMath.h"

struct SceneConstantBuffer {
	using Matrix4D = RTMatrix4D::RTMatrix4DImpl;
	using Vector4D = RTVector4D::RTVec4DImpl;

	Matrix4D projectionToWorld; 
	Vector4D cameraPosition; 
	Vector4D lightPosition; 
	Vector4D lightAmbientColour;
	Vector4D lightDiffuseColour;
};

struct ObjectConstantBuffer {
	using Vector4D = RTVector4D::RTVec4DImpl;

	Vector4D colour; 
};

struct Vertex {
	using Vector3D = RTVector3D::RTVec3DImpl;

	Vector3D position; 
	Vector3D normal;
};