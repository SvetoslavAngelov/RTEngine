# RTEngine

RTEngine is a DirectX 12 raytracing engine that uses a custom Math library for geometry representation and calculations. The engine implements a modern physically-based rendering pipeline using hardware-accelerated raytracing capabilities provided by DirectX Raytracing (DXR).

## Project Structure

The project is organized into several key components:

### Math Library

Located in the `/Math` directory, this is a custom implementation of 3D mathematics primitives and operations:

- Vector classes (2D, 3D, 4D)
- Point classes (2D, 3D)
- Matrix classes (3D, 4D)
- Normal vectors
- Ray definition and operations

The Math library is designed to be independent of any rendering API, allowing for clean separation of concerns between mathematics and rendering code.

### DirectX RHI (Rendering Hardware Interface)

Located in the `/DirectXRHI` directory, this component handles all DirectX 12 specific code:

- `RTDXInterface`: Main interface between the application and DirectX 12
- `RTDeviceResources`: Manages DirectX device resources and rendering state
- `RTWinApp`: Windows application handling
- `RTHelper`: Utility functions for DirectX 12 raytracing

### Shaders

Located in the `/Shaders` directory, these are HLSL shaders required for DirectX Raytracing:

- `RayGen.hlsl`: Generates primary rays for each pixel
- `Hit.hlsl`: Handles ray-triangle intersection and shading calculation
- `Miss.hlsl`: Handles rays that don't hit any geometry
- `Common.hlsl`: Shared definitions and structures

Compiled versions of these shaders are stored in the `/Shaders/CompiledShaders` directory.

### Scene Management

Located in the `/Scene` directory:

- `RTScene.h`: Defines scene data structures and constant buffers for raytracing

## Math Library to DirectX Pipeline Integration

One of the key features of RTEngine is the seamless integration between the custom Math library and the DirectX 12 raytracing pipeline. Here's how they connect:

1. **Scene Definition**: The `RTScene.h` file defines data structures that use the Math library's types:
   - `SceneConstantBuffer` uses `RTMatrix4D` and `RTVector4D` for transformation matrices and scene properties
   - `Vertex` structures use `RTVector3D` for position and normal data

2. **Geometry Building**: In `RTDXInterface::BuildGeometry()`, custom Math library types are used to define vertex positions and normals, which are then uploaded to GPU memory for raytracing.

3. **Constant Buffers**: Scene data such as camera position, light information, and transformation matrices use Math library types, which are then copied to GPU-accessible constant buffers.

4. **Acceleration Structures**: The DirectX raytracing pipeline uses Bottom Level Acceleration Structures (BLAS) and Top Level Acceleration Structures (TLAS) to accelerate ray-geometry intersection tests. These structures reference vertex and index buffers that are populated with geometry expressed using the Math library.

5. **Shader Interaction**: The HLSL shaders are designed to work with the memory layout of the Math library types when they're uploaded to the GPU.

## Building and Compiling Shaders

### Project Build

The project uses Visual Studio's built-in build system with the provided `.sln` and `.vcxproj` files.

### Shader Compilation

The raytracing shaders need to be compiled with the DirectX Shader Compiler (DXC) using the lib_6_3 profile for use with DirectX Raytracing. After modifying any shader, they need to be recompiled before building the project.

Example compilation command:
```
dxc -T lib_6_3 -Fo Shaders/CompiledShaders/RayGen.hlsl.h RayGen.hlsl
```

## Architecture Details

### Raytracing Pipeline Setup

The raytracing pipeline is set up in the following stages:

1. **Device Setup**: Creates and initializes DirectX 12 device and command lists
2. **Pipeline Creation**: Defines the raytracing pipeline state object, including shader entries, hit groups, and global root signature
3. **Resource Creation**: Creates necessary resources such as acceleration structures, constant buffers, and output textures
4. **Geometry Setup**: Loads scene geometry using the custom Math library and creates GPU buffers
5. **Acceleration Structure Building**: Creates BLAS and TLAS for efficient ray-triangle intersection tests
6. **Shader Table Setup**: Creates shader tables that link shader code with scene resources
7. **Raytracing Execution**: Dispatches rays for rendering and copies results to the output

### Scene Constants and Objects

The engine uses two main constant buffer structures:

- `SceneConstantBuffer`: Contains global scene information like camera position, light data, and projection matrices
- `ObjectConstantBuffer`: Contains per-object data like color information

### Rendering Loop

The main rendering loop is implemented in `RTDXInterface::OnRender()`, which:

1. Prepares device resources
2. Performs ray tracing via `DoRayTracing()`
3. Copies the raytracing output to the render target
4. Presents the rendered frame to the screen

## Future Directions

Potential areas for extension and improvement:

1. Support for more complex geometry and scene descriptions
2. Material system for physically-based rendering
3. Dynamic scene updates and animation
4. Post-processing effects
5. Performance optimizations for acceleration structure builds
6. Multi-sample anti-aliasing (MSAA) support
7. Denoising for improved raytracing quality

## License

See the LICENSE file for details.