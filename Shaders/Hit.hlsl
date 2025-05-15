#include "Common.hlsl"

// Scene constant buffer
cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projectionToWorld;
    float4 cameraPosition;
    float4 lightPosition;
    float4 lightAmbientColor;
    float4 lightDiffuseColor;
};

// Object constant buffer
cbuffer ObjectConstantBuffer : register(b1)
{
    float4 objectColor;
};

// Vertex buffer
struct Vertex
{
    float3 position;
    float3 normal;
};
StructuredBuffer<Vertex> Vertices : register(t1);

// Index buffer
ByteAddressBuffer Indices : register(t2);

[shader("closesthit")] 
void ClosestHit(inout HitInfo payload, Attributes attrib) 
{
    // Get the hit triangle
    uint indexSizeInBytes = 4;
    uint indicesPerTriangle = 3;
    uint triangleIndexStride = indicesPerTriangle * indexSizeInBytes;
    uint baseIndex = PrimitiveIndex() * triangleIndexStride;

    // Load indices for the hit triangle
    uint3 indices = uint3(
        Indices.Load(baseIndex),
        Indices.Load(baseIndex + indexSizeInBytes),
        Indices.Load(baseIndex + 2 * indexSizeInBytes)
    );

    // Get the vertices for the hit triangle
    Vertex v0 = Vertices[indices.x];
    Vertex v1 = Vertices[indices.y];
    Vertex v2 = Vertices[indices.z];

    // Interpolate the normal using the barycentric coordinates
    float3 barycentrics = float3(1.0 - attrib.bary.x - attrib.bary.y, attrib.bary.x, attrib.bary.y);
    float3 normal = normalize(v0.normal * barycentrics.x + v1.normal * barycentrics.y + v2.normal * barycentrics.z);

    // Calculate the position of the hit point
    float3 hitPosition = v0.position * barycentrics.x + v1.position * barycentrics.y + v2.position * barycentrics.z;
    
    // Calculate the direction to the light
    float3 lightDir = normalize(lightPosition.xyz - hitPosition);
    
    // Calculate the lambertian diffuse term
    float diffuseFactor = max(dot(normal, lightDir), 0.0);
    
    // Final color calculation = ambient + diffuse
    float3 finalColor = lightAmbientColor.rgb + diffuseFactor * lightDiffuseColor.rgb * objectColor.rgb;
    
    // Output color and distance
    payload.colorAndDistance = float4(finalColor, RayTCurrent());
}
