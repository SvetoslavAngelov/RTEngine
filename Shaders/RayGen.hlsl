#include "Common.hlsl"

// Raytracing output texture, accessed as a UAV
RWTexture2D<float4> gOutput : register(u0);

// Scene constant buffer
cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projectionToWorld;
    float4 cameraPosition;
    float4 lightPosition;
    float4 lightAmbientColor;
    float4 lightDiffuseColor;
};

// Raytracing acceleration structure, accessed as a SRV
RaytracingAccelerationStructure SceneBVH : register(t0);

// Vertex buffer
StructuredBuffer<float3> Vertices : register(t1);
// Index buffer
ByteAddressBuffer Indices : register(t2);

[shader("raygeneration")] 
void RayGen() {
    // Initialize the ray payload
    HitInfo payload;
    payload.colorAndDistance = float4(0.0, 0.0, 0.0, 0.0);

    // Get the location within the dispatched 2D grid of work items
    uint2 launchIndex = DispatchRaysIndex();
    uint2 launchDim = DispatchRaysDimensions();

    // Generate primary ray from the camera
    float2 pixelCenter = launchIndex + float2(0.5, 0.5);
    float2 screenPos = (pixelCenter / float2(launchDim)) * 2.0 - 1.0;
    
    // Invert Y for DirectX coordinate system
    screenPos.y = -screenPos.y;

    // Define ray
    float3 origin = cameraPosition.xyz;
    float3 direction = normalize(float3(screenPos.x, screenPos.y, 1.0));

    // Trace the ray
    RayDesc ray;
    ray.Origin = origin;
    ray.Direction = direction;
    ray.TMin = 0.001;
    ray.TMax = 10000.0;

    // Trace the ray
    TraceRay(
        SceneBVH,         // Acceleration structure
        RAY_FLAG_NONE,    // Ray flags
        0xFF,             // Instance inclusion mask
        0,                // Ray contribution to hit group index
        0,                // Multiplier for geometry contribution to hit group index
        0,                // Miss shader index
        ray,              // Ray description
        payload           // Ray payload
    );

    // Write the raytracing result to the output texture
    gOutput[launchIndex] = float4(payload.colorAndDistance.rgb, 1.0);
}
