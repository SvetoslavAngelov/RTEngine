

// Input parameters
//RaytracingAccelerationStructure Scene : register (t0, space0);
RWTexture2D<float4> RaytracingOutput : register(u0, space0);
ByteAddressBuffer IndexStream : register (t1, space0);
StructuredBuffer<Vertex> VertexStream : register (t2, space0);

// Output
