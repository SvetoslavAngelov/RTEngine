#include "Common.hlsl"

[shader("miss")]
void Miss(inout HitInfo payload : SV_RayPayload)
{
    // Create a sky gradient for rays that miss all geometry
    float4 backgroundColor = float4(0.5f, 0.7f, 1.0f, 1.0f);
    
    // Get the ray direction from the WorldRayDirection system value
    float3 rayDir = WorldRayDirection();
    
    // Adjust color based on ray direction (darker at the bottom, lighter toward the top)
    // Map the Y direction from [-1,1] to [0,1]
    float t = saturate((rayDir.y + 1.0) * 0.5);
    backgroundColor.rgb = lerp(float3(0.5f, 0.5f, 0.8f), float3(0.8f, 0.9f, 1.0f), t);
    
    payload.colorAndDistance = float4(backgroundColor.rgb, -1.0f);
}