#include "RTDXInterface.h"
#include "RTDeviceResources.h"
#include "RTWinApp.h"
#include "HrException.h"
#include "../Shaders/CompiledShaders/RayGen.hlsl.h"
#include "../Shaders/CompiledShaders/Hit.hlsl.h"
#include "../Shaders/CompiledShaders/Miss.hlsl.h"

const wchar_t* RTDXInterface::c_rayGenShaderName = L"RayGen";
const wchar_t* RTDXInterface::c_closestHitShaderName = L"ClosestHit";
const wchar_t* RTDXInterface::c_missShaderName = L"Miss";
const wchar_t* RTDXInterface::c_hitGroupName = L"HitGroup";


RTDXInterface::RTDXInterface(UINT viewportWidth, UINT viewportHeight, std::wstring windowName) :
	width { viewportWidth },
	height { viewportHeight },
	aspectRatio { static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight) },
	windowBounds { 0, 0, static_cast<long>(viewportWidth), static_cast<long>(viewportHeight) },
	windowTitle { windowName }
{
	// Initialize timer
	timer.SetFixedTimeStep(true);
	timer.SetTargetElapsedSeconds(1.0 / 60.0); // Target 60 FPS
}

void RTDXInterface::OnInit()
{
	deviceResources = std::make_unique<RTDeviceResources>();

	if (deviceResources)
	{
		deviceResources->SetWindow(RTWinApp::GetHwnd(), width, height);
		deviceResources->InitialiseDXGIAdapter();
		deviceResources->CreateDeviceResources(); 
		deviceResources->CreateWindowSizeDependentResources();
	
		// Initialise the scene
		{
			// Set up object colour
			rtObject.colour = RTVector4D(1.f, 1.f, 1.f, 1.f);

			// Set up scene constants
			for (int i = 0; i < frameCount; i++)
			{
				// Set identity matrix
				rtScene[i].projectionToWorld = RTMatrix4D::RTMatrix4DImpl();
				
				// Set camera position
				rtScene[i].cameraPosition = RTVector4D(0.0f, 0.0f, -2.0f, 1.0f);
				
				// Set light position
				rtScene[i].lightPosition = RTVector4D(0.0f, 1.0f, -2.0f, 1.0f);
				
				// Set light colors
				rtScene[i].lightAmbientColour = RTVector4D(0.2f, 0.2f, 0.2f, 1.0f);
				rtScene[i].lightDiffuseColour = RTVector4D(0.8f, 0.8f, 0.8f, 1.0f);
			}
		}

		// Build the raytracing pipeline.
		CreateDeviceDependentResources();
	}
}

void RTDXInterface::CreateDeviceDependentResources()
{
	// Build the raytracing pipeline.

	// Query the instanciated device object and assign the raytracing device and command list, if ray tracing is supported.
	CreateRaytracingInterfaces(); 

	// Create the global root signature used by the shaders. 
	CreateGlobalRootSignature(); 

	// Create a raytracing pipeline state object which defines the binding of shaders, state and resources to be used during raytracing. 
	CreateRaytracingPipelineStateObject();

	// Allocate GPU memory for the descriptor resources. 
	CreateDescriptorHeap(); 

	// Build the geometry. 
	BuildGeometry();

	// Build raytracing acceleration structures from the generated geometry. 
	BuildAccelerationStructures(); 

	// Build shader tables, which define shaders an their local root arguments. 
	BuildShaderTables();

	// Create an output texture to store the raytracing result to. 
	CreateRaytracingOutputResource();
}

void RTDXInterface::CreateRaytracingInterfaces()
{
	Microsoft::WRL::ComPtr<ID3D12Device> device = deviceResources->GetD3DDevice();
	ID3D12GraphicsCommandList* commandList = deviceResources->GetCommandList();

	ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&raytracingDevice)), L"Couldn't get a DirectX Raytracing interface for the device.\n");
	ThrowIfFailed(commandList->QueryInterface(IID_PPV_ARGS(&raytracingCommanList)), L"Couldn't get DirectX Raytracing interface for the command list.\n");
}

void RTDXInterface::CreateGlobalRootSignature()
{
	// Global root parameters, which are shared between all shaders.
	//	- Unordered Access View (UAV) descriptor for the ray tracing output, stored as texture.
	//  - Constant Buffer View (CBC) desriptor for the scene transform data (MVP matrix). 
	//  - Shader Resource View (SRV) descritors for the
	//		- Acceleration structure
	//		- Vertex buffer
	//		- Index buffer

	CD3DX12_ROOT_PARAMETER rootParameters[GlobalRootSignatureParams::Count];

	// Output view slot
	// The ray tracing result stored as a UAV resource is bound to :register (u0, space0)
	rootParameters[GlobalRootSignatureParams::OutputViewSlot].InitAsUnorderedAccessView(0);

	// Constant buffer slot
	// The constant buffer stored as CBV resource is bound to :register (b0, space0)
	rootParameters[GlobalRootSignatureParams::ConstantBufferSlot].InitAsConstantBufferView(0);

	// Acceleration structure slot
	// The acceleration structure stored as SRV resource is bound to :register (t0, space0)
	rootParameters[GlobalRootSignatureParams::AccelerationStructureSlot].InitAsShaderResourceView(0);

	// Vertex & Index buffer slot
	// The SRVs are bound to :register (t1, space0) and :register (t2, space0)
	CD3DX12_DESCRIPTOR_RANGE VertexBufferDescriptor;
	VertexBufferDescriptor.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);
	rootParameters[GlobalRootSignatureParams::VertexBufferSlot].InitAsDescriptorTable(1, &VertexBufferDescriptor);

	// Create the global root signature with the list of descriptor tables as parameters. 
	CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(GlobalRootSignatureParams::Count, rootParameters);

	// Serialise and create the global root signature
	Microsoft::WRL::ComPtr<ID3D12Device> device = deviceResources->GetD3DDevice();
	Microsoft::WRL::ComPtr<ID3DBlob> serialisedRootSignature; 
	Microsoft::WRL::ComPtr<ID3DBlob> errorMessage; 

	ThrowIfFailed(D3D12SerializeRootSignature(&globalRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serialisedRootSignature, &errorMessage),
		errorMessage ? static_cast<wchar_t*>(errorMessage->GetBufferPointer()) : nullptr);

	ThrowIfFailed(device->CreateRootSignature(1, serialisedRootSignature->GetBufferPointer(), serialisedRootSignature->GetBufferSize(), 
		IID_PPV_ARGS(&raytracingGlobalRootSignature)));
}

void RTDXInterface::CreateRaytracingPipelineStateObject()
{
	// A state object represents a variable amount of configuration state, 
	// including shaders, that an application manages as a single unit and 
	// which is given to a driver atomically to process (e.g. compile/optimize)
    // however it sees fit. A state object is created via CreateStateObject() on a D3D12 device.

	// The Pipeline State Object here is built using six subobjects:
	// - DXIL library
    // - Triangle hit group
    // - Shader config
    // - Global root signature
    // - Pipeline config

	CD3DX12_STATE_OBJECT_DESC raytracingPipeline{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

	// DXIL libraries 
	D3D12_SHADER_BYTECODE rayGenDXIL = CD3DX12_SHADER_BYTECODE((void*)g_pRayGen, ARRAYSIZE(g_pRayGen));
	D3D12_SHADER_BYTECODE closestHitDXIL = CD3DX12_SHADER_BYTECODE((void*)g_pHit, ARRAYSIZE(g_pHit));
	D3D12_SHADER_BYTECODE missDXIL = CD3DX12_SHADER_BYTECODE((void*)g_pMiss, ARRAYSIZE(g_pMiss));

	// Raygen library
	auto rayGenLib = raytracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
	rayGenLib->SetDXILLibrary(&rayGenDXIL);
	rayGenLib->DefineExport(c_rayGenShaderName);

	// Closest hit library
	auto closestHitLib = raytracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
	closestHitLib->SetDXILLibrary(&closestHitDXIL);
	closestHitLib->DefineExport(c_closestHitShaderName);

	// Miss library 
	auto missLib = raytracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
	missLib->SetDXILLibrary(&missDXIL);
	missLib->DefineExport(c_missShaderName);

	// Triangle hit group 
	auto hitGroup = raytracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
	hitGroup->SetClosestHitShaderImport(c_closestHitShaderName);
	hitGroup->SetHitGroupExport(c_hitGroupName);
	hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

	// Shader config
	auto shaderConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	using Vec2D = RTVector2D::RTVec2DImpl;
	using Vec4D = RTVector4D;
	
	UINT rayPayloadSize = sizeof(Vec4D);
	UINT attributeSize = sizeof(Vec2D);
	shaderConfig->Config(rayPayloadSize, attributeSize);

	// Global root signature
	auto globalRootSignature = raytracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	globalRootSignature->SetRootSignature(raytracingGlobalRootSignature.Get());

	// Pipeline config
	auto pipelineConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	
	// Use primary rays only
	UINT maxRecursionDepth = 1; 
	pipelineConfig->Config(maxRecursionDepth);

	// Create the state object 
	ThrowIfFailed(raytracingDevice->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&raytracingStateObject)), L"Couldn't create a DirectX Raytracing state object.\n");
}

void RTDXInterface::CreateDescriptorHeap()
{
	auto device = deviceResources->GetD3DDevice();

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};

	// Allocate a heap for 3 descriptors: 
	// 2 - vertex and index buffer SRVs
	// 1 - raytracing output texture UAV
	descriptorHeapDesc.NumDescriptors = 3; 
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NodeMask = 0;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	
	descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void RTDXInterface::BuildGeometry()
{
	auto device = deviceResources->GetD3DDevice();
	auto commandList = deviceResources->GetCommandList();

	// Simple triangle
	// TODO pass geometry to function 
	constexpr int numVertices = 3;
	
	Vertex triangleVertices[numVertices] = {
		// Position                                Normal
		{ RTVector3D::RTVec3DImpl(0.0f, 0.5f, 0.0f), RTVector3D::RTVec3DImpl(0.0f, 0.0f, -1.0f) },
		{ RTVector3D::RTVec3DImpl(0.5f, -0.5f, 0.0f), RTVector3D::RTVec3DImpl(0.0f, 0.0f, -1.0f) },
		{ RTVector3D::RTVec3DImpl(-0.5f, -0.5f, 0.0f), RTVector3D::RTVec3DImpl(0.0f, 0.0f, -1.0f) }
	};

	// Define indices for the triangle
	std::vector<UINT> triangleIndices = { 0, 1, 2 };
	UINT numIndices = static_cast<UINT>(triangleIndices.size());

	// Create vertex buffer
	const UINT vertexBufferSize = numVertices * sizeof(Vertex);

	// Upload the vertex buffer to the GPU
	{
		CD3DX12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		ThrowIfFailed(device->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertexBuffer.resource)),
			L"Failed to create vertex buffer resource");

		// Copy the triangle data to the vertex buffer
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(vertexBuffer.resource->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)),
			L"Failed to map vertex buffer");
		memcpy(pVertexDataBegin, triangleVertices, vertexBufferSize);
		vertexBuffer.resource->Unmap(0, nullptr);

		// Create the SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.NumElements = numVertices;
		srvDesc.Buffer.StructureByteStride = sizeof(Vertex);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		// Allocate descriptor handles
		vertexBuffer.cpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), descriptorsAllocated, descriptorSize);
		vertexBuffer.gpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(descriptorHeap->GetGPUDescriptorHandleForHeapStart(), descriptorsAllocated, descriptorSize);
		descriptorsAllocated++;

		device->CreateShaderResourceView(vertexBuffer.resource.Get(), &srvDesc, vertexBuffer.cpuDescriptorHandle);
	}

	// Create index buffer
	const UINT indexBufferSize = numIndices * sizeof(UINT);

	// Upload the index buffer to the GPU
	{
		CD3DX12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
		ThrowIfFailed(device->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&indexBuffer.resource)),
			L"Failed to create index buffer resource");

		// Copy the triangle data to the index buffer
		UINT8* pIndexDataBegin;
		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(indexBuffer.resource->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin)),
			L"Failed to map index buffer");
		memcpy(pIndexDataBegin, triangleIndices.data(), indexBufferSize);
		indexBuffer.resource->Unmap(0, nullptr);

		// Create the SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.NumElements = numIndices;
		srvDesc.Buffer.StructureByteStride = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
		srvDesc.Buffer.FirstElement = 0;

		// Allocate descriptor handles
		indexBuffer.cpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), descriptorsAllocated, descriptorSize);
		indexBuffer.gpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(descriptorHeap->GetGPUDescriptorHandleForHeapStart(), descriptorsAllocated, descriptorSize);
		descriptorsAllocated++;

		device->CreateShaderResourceView(indexBuffer.resource.Get(), &srvDesc, indexBuffer.cpuDescriptorHandle);
	}
}

void RTDXInterface::BuildAccelerationStructures()
{
	auto device = deviceResources->GetD3DDevice();
	auto commandList = deviceResources->GetCommandList();

	// 1. Create the Bottom Level Acceleration Structure (BLAS)
	// This stores the triangle mesh data
	D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Triangles.IndexBuffer = indexBuffer.resource->GetGPUVirtualAddress();
	geometryDesc.Triangles.IndexCount = 3; // Single triangle
	geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
	geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	geometryDesc.Triangles.VertexCount = 3;
	geometryDesc.Triangles.VertexBuffer.StartAddress = vertexBuffer.resource->GetGPUVirtualAddress();
	geometryDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);
	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

	// Get required sizes for acceleration structure buffers
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottomLevelInputs = {};
	bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	bottomLevelInputs.NumDescs = 1;
	bottomLevelInputs.pGeometryDescs = &geometryDesc;
	bottomLevelInputs.Flags = buildFlags;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
	raytracingDevice->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);

	// Create BLAS scratch buffer
	CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC scratchBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(
		bottomLevelPrebuildInfo.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
	);
	
	ThrowIfFailed(device->CreateCommittedResource(
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&scratchBufferDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&bottomLevelAccelerationStructureScratch)),
		L"Failed to create BLAS scratch buffer");

	// Create BLAS buffer
	CD3DX12_RESOURCE_DESC blasBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(
		bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
	);

	ThrowIfFailed(device->CreateCommittedResource(
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&blasBufferDesc,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		nullptr,
		IID_PPV_ARGS(&bottomLevelAccelerationStructure)),
		L"Failed to create BLAS buffer");

	// 2. Create the Top Level Acceleration Structure (TLAS)
	// This references the BLAS and defines its transformation in the scene
	
	// Create instance descriptor
	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
	// Identity matrix for transformation
	instanceDesc.Transform[0][0] = 1.0f;
	instanceDesc.Transform[1][1] = 1.0f;
	instanceDesc.Transform[2][2] = 1.0f;
	instanceDesc.InstanceID = 0;
	instanceDesc.InstanceMask = 1;
	instanceDesc.InstanceContributionToHitGroupIndex = 0;
	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	// Refer to the bottom level acceleration structure
	instanceDesc.AccelerationStructure = bottomLevelAccelerationStructure->GetGPUVirtualAddress();

	// Create instance descriptors buffer
	CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC instanceDescsBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(D3D12_RAYTRACING_INSTANCE_DESC));

	ThrowIfFailed(device->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&instanceDescsBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&instanceDescs)),
		L"Failed to create instance descriptors buffer");

	// Copy instance descriptor to the buffer
	D3D12_RAYTRACING_INSTANCE_DESC* mappedData;
	CD3DX12_RANGE readRange(0, 0);
	ThrowIfFailed(instanceDescs->Map(0, &readRange, reinterpret_cast<void**>(&mappedData)),
		L"Failed to map instance descriptors buffer");
	memcpy(mappedData, &instanceDesc, sizeof(D3D12_RAYTRACING_INSTANCE_DESC));
	instanceDescs->Unmap(0, nullptr);

	// Get required sizes for the top level acceleration structure buffers
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS topLevelInputs = {};
	topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	topLevelInputs.NumDescs = 1;
	topLevelInputs.InstanceDescs = instanceDescs->GetGPUVirtualAddress();
	topLevelInputs.Flags = buildFlags;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
	raytracingDevice->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);

	// Create TLAS scratch buffer
	CD3DX12_RESOURCE_DESC topLevelScratchBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(
		topLevelPrebuildInfo.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
	);

	ThrowIfFailed(device->CreateCommittedResource(
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&topLevelScratchBufferDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&topLevelAccelerationStructureScratch)),
		L"Failed to create TLAS scratch buffer");

	// Create TLAS buffer
	CD3DX12_RESOURCE_DESC topLevelBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(
		topLevelPrebuildInfo.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
	);

	ThrowIfFailed(device->CreateCommittedResource(
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&topLevelBufferDesc,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		nullptr,
		IID_PPV_ARGS(&topLevelAccelerationStructure)),
		L"Failed to create TLAS buffer");

	// 3. Build the acceleration structures
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
	bottomLevelBuildDesc.Inputs = bottomLevelInputs;
	bottomLevelBuildDesc.ScratchAccelerationStructureData = bottomLevelAccelerationStructureScratch->GetGPUVirtualAddress();
	bottomLevelBuildDesc.DestAccelerationStructureData = bottomLevelAccelerationStructure->GetGPUVirtualAddress();

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
	topLevelBuildDesc.Inputs = topLevelInputs;
	topLevelBuildDesc.ScratchAccelerationStructureData = topLevelAccelerationStructureScratch->GetGPUVirtualAddress();
	topLevelBuildDesc.DestAccelerationStructureData = topLevelAccelerationStructure->GetGPUVirtualAddress();

	// Build both acceleration structures
	raytracingCommanList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);

	// Wait for BLAS build to complete before TLAS build
	CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(bottomLevelAccelerationStructure.Get());
	raytracingCommanList->ResourceBarrier(1, &uavBarrier);

	raytracingCommanList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);
}

void RTDXInterface::BuildShaderTables()
{
	auto device = deviceResources->GetD3DDevice();

	void* rayGenShaderIdentifier = nullptr;
	void* missShaderIdentifier = nullptr; 
	void* hitGroupShaderIdentifier = nullptr; 

	auto GetShaderIdentifiers = [&](auto* stateObjectProperties)
		{
			rayGenShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_rayGenShaderName);
			missShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_missShaderName);
			hitGroupShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_hitGroupName);
		};

	// Get shader identifiers
	UINT shaderIdentifierSize;
	{
		Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
		ThrowIfFailed(raytracingStateObject.As(&stateObjectProperties));
		GetShaderIdentifiers(stateObjectProperties.Get());
		shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	}
	
	// Ray gen shader table 
	{
		UINT numShaderRecords = 1; 
		UINT shaderRecordSize = shaderIdentifierSize;
		ShaderTable rayGenShaderTable(device, numShaderRecords, shaderRecordSize, L"RayGenShaderTable");
		rayGenShaderTable.push_back(ShaderRecord(rayGenShaderIdentifier, shaderIdentifierSize));
		m_rayGenShaderTable = rayGenShaderTable.GetResource();
	}

	// Miss shader table 
	{
		UINT numShaderRecords = 1; 
		UINT shaderRecordSize = shaderIdentifierSize;
		ShaderTable missShaderTable(device, numShaderRecords, shaderRecordSize, L"MissShaderTable");
		missShaderTable.push_back(ShaderRecord(missShaderIdentifier, shaderIdentifierSize));
		m_missShaderTable = missShaderTable.GetResource();
	}

	// Hit group shader table 
	{
		UINT numShaderRecords = 1;
		UINT shaderRecordSize = shaderIdentifierSize;
		ShaderTable hitGroupShaderTable(device, numShaderRecords, shaderRecordSize, L"HitGroupShaderTable");
		hitGroupShaderTable.push_back(ShaderRecord(hitGroupShaderIdentifier, shaderIdentifierSize));
		m_hitGroupShaderTable = hitGroupShaderTable.GetResource();
	}
}

void RTDXInterface::CreateRaytracingOutputResource()
{
	auto device = deviceResources->GetD3DDevice();
	auto commandList = deviceResources->GetCommandList();

	// Create the output resource
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.DepthOrArraySize = 1;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	resDesc.Width = width;
	resDesc.Height = height;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;

	CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

	ThrowIfFailed(device->CreateCommittedResource(
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&raytracingOutput)),
		L"Failed to create raytracing output resource");

	// Create the UAV for raytracing output
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

	// Allocate descriptor handles
	raytracingOutputResourceUAVDescriptorHeapIndex = descriptorsAllocated++;
	CD3DX12_CPU_DESCRIPTOR_HANDLE uavDescriptorHandle(
		descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		raytracingOutputResourceUAVDescriptorHeapIndex,
		descriptorSize);
	raytracingOutputResourceUAVGpuDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		descriptorHeap->GetGPUDescriptorHandleForHeapStart(),
		raytracingOutputResourceUAVDescriptorHeapIndex,
		descriptorSize);

	// Create the UAV
	device->CreateUnorderedAccessView(raytracingOutput.Get(), nullptr, &uavDesc, uavDescriptorHandle);
}

void RTDXInterface::OnUpdate()
{
	// Update timer
	timer.Tick([&]()
	{
		// Calculate time-based values
		float elapsedTime = static_cast<float>(timer.GetElapsedSeconds());
		float totalTime = static_cast<float>(timer.GetTotalSeconds());
		
		// --- Update scene for animation ---
		
		// Get the current frame index
		auto frameIndex = deviceResources->GetCurrentFrameIndex();
		
		// 1. Animate the light position in a circular pattern
		float lightRadius = 3.0f;
		float lightSpeed = 0.5f;  // Revolutions per second
		float lightAngle = totalTime * lightSpeed * 2.0f * 3.14159f;
		
		rtScene[frameIndex].lightPosition = RTVector4D(
			lightRadius * sinf(lightAngle),
			1.0f + 0.5f * sinf(totalTime * 2.0f),  // Add some vertical movement
			lightRadius * cosf(lightAngle),
			1.0f
		);
		
		// 2. Smoothly change light colors over time
		// Create a subtle pulsing effect for the ambient light
		float pulseAmount = 0.1f;
		float pulseSpeed = 2.0f;
		float pulseValue = 0.2f + pulseAmount * sinf(totalTime * pulseSpeed);
		
		rtScene[frameIndex].lightAmbientColour = RTVector4D(
			pulseValue,
			pulseValue,
			pulseValue * 1.2f,  // Slightly blue tint
			1.0f
		);
		
		// 3. Gradually shift diffuse light color for visual interest
		float colorCycleSpeed = 0.3f;
		rtScene[frameIndex].lightDiffuseColour = RTVector4D(
			0.7f + 0.3f * sinf(totalTime * colorCycleSpeed),
			0.7f + 0.3f * sinf(totalTime * colorCycleSpeed + 2.0f),
			0.7f + 0.3f * sinf(totalTime * colorCycleSpeed + 4.0f),
			1.0f
		);
		
		// 4. Optionally update camera position for simple camera movement
		// Example: Small orbital movement
		if (false) { // Disabled by default - enable when needed
			float cameraRadius = 2.5f;
			float cameraSpeed = 0.1f;  // Slower than light
			float cameraAngle = totalTime * cameraSpeed * 2.0f * 3.14159f;
			
			rtScene[frameIndex].cameraPosition = RTVector4D(
				cameraRadius * sinf(cameraAngle) * 0.5f,
				0.5f * sinf(totalTime * 0.5f),  // Gentle up/down
				-2.0f + cameraRadius * cosf(cameraAngle) * 0.5f,
				1.0f
			);
			
			// Update view matrix accordingly
			// This would require more complex matrix calculation
			// based on camera position and target
		}
		
		// 5. Update object color for visual effect
		// Cycle through different hues
		float colorSpeed = 0.2f;
		rtObject.colour = RTVector4D(
			0.5f + 0.5f * sinf(totalTime * colorSpeed),
			0.5f + 0.5f * sinf(totalTime * colorSpeed + 2.0f),
			0.5f + 0.5f * sinf(totalTime * colorSpeed + 4.0f),
			1.0f
		);
		
		// For debug/performance tracking
		// You could output frame rate periodically
		static int fpsCounter = 0;
		fpsCounter++;
		if (fpsCounter >= 60) {
			OutputDebugStringW((L"FPS: " + std::to_wstring(timer.GetFramesPerSecond()) + L"\n").c_str());
			fpsCounter = 0;
		}
	});
}

void RTDXInterface::DoRayTracing()
{
	auto device = deviceResources->GetD3DDevice();
	auto commandList = deviceResources->GetCommandList();
	auto frameIndex = deviceResources->GetCurrentFrameIndex();

	auto DispatchRays = [&](auto* commandList, auto* stateObject, auto* dispatchDesc)
		{
			// Each shader table has only one shader record, the stride is the same
			dispatchDesc->RayGenerationShaderRecord.StartAddress = m_rayGenShaderTable->GetGPUVirtualAddress();
			dispatchDesc->RayGenerationShaderRecord.SizeInBytes = m_rayGenShaderTable->GetDesc().Width;

			dispatchDesc->MissShaderTable.StartAddress = m_missShaderTable->GetGPUVirtualAddress();
			dispatchDesc->MissShaderTable.SizeInBytes = m_missShaderTable->GetDesc().Width;
			dispatchDesc->MissShaderTable.StrideInBytes = dispatchDesc->MissShaderTable.SizeInBytes;

			dispatchDesc->HitGroupTable.StartAddress = m_hitGroupShaderTable->GetGPUVirtualAddress();
			dispatchDesc->HitGroupTable.SizeInBytes = m_hitGroupShaderTable->GetDesc().Width;
			dispatchDesc->HitGroupTable.StrideInBytes = dispatchDesc->HitGroupTable.SizeInBytes;

			dispatchDesc->Width = width;
			dispatchDesc->Height = height;
			dispatchDesc->Depth = 1;

			commandList->SetPipelineState1(stateObject);
			commandList->DispatchRays(dispatchDesc);
		};

	auto SetCommonPipelineState = [&](auto* descriptorSetCommandList)
		{
			descriptorSetCommandList->SetDescriptorHeaps(1, descriptorHeap.GetAddressOf());

			// Set raytracing output UAV
			descriptorSetCommandList->SetComputeRootDescriptorTable(
				GlobalRootSignatureParams::OutputViewSlot, 
				raytracingOutputResourceUAVGpuDescriptor);

			// Set index and vertex buffer descriptor tables
			descriptorSetCommandList->SetComputeRootDescriptorTable(
				GlobalRootSignatureParams::VertexBufferSlot,
				vertexBuffer.gpuDescriptorHandle);
		};

	commandList->SetComputeRootSignature(raytracingGlobalRootSignature.Get());

	// Copy the updated scene constant buffer to GPU
	{
		// Create a constant buffer for the scene data
		CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		
		// Calculate size needed for the constant buffer (must be aligned to 256 bytes)
		UINT constantBufferSize = (sizeof(SceneConstantBuffer) + 255) & ~255;
		
		// Create the constant buffer resource
		Microsoft::WRL::ComPtr<ID3D12Resource> sceneConstantBuffer;
		CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);
		
		ThrowIfFailed(device->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&constantBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&sceneConstantBuffer)),
			L"Failed to create scene constant buffer");
		
		// Copy the scene data to the constant buffer
		UINT8* pSceneDataBegin;
		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(sceneConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pSceneDataBegin)),
			L"Failed to map scene constant buffer");
		memcpy(pSceneDataBegin, &rtScene[frameIndex], sizeof(SceneConstantBuffer));
		sceneConstantBuffer->Unmap(0, nullptr);
		
		// Set the constant buffer
		commandList->SetComputeRootConstantBufferView(
			GlobalRootSignatureParams::ConstantBufferSlot,
			sceneConstantBuffer->GetGPUVirtualAddress());
	}

	// Bind the heaps, acceleration structure and dispatch rays
	D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
	SetCommonPipelineState(commandList);
	
	// Set the acceleration structure
	commandList->SetComputeRootShaderResourceView(
		GlobalRootSignatureParams::AccelerationStructureSlot, 
		topLevelAccelerationStructure->GetGPUVirtualAddress());
	
	// Dispatch rays
	DispatchRays(raytracingCommanList.Get(), raytracingStateObject.Get(), &dispatchDesc);
}

void RTDXInterface::OnRender()
{
	deviceResources->Prepare(); 
	
	auto commandList = deviceResources->GetCommandList();
	auto renderTarget = deviceResources->GetRenderTarget();

	// Perform raytracing
	DoRayTracing();
	
	// Copy raytracing output to the render target
	D3D12_RESOURCE_BARRIER preCopyBarriers[2];
	preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);
	preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(raytracingOutput.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	commandList->ResourceBarrier(ARRAYSIZE(preCopyBarriers), preCopyBarriers);

	commandList->CopyResource(renderTarget, raytracingOutput.Get());

	D3D12_RESOURCE_BARRIER postCopyBarriers[2];
	postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);
	postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(raytracingOutput.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	commandList->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);
	
	deviceResources->Present(D3D12_RESOURCE_STATE_PRESENT);
}

RTDXInterface::~RTDXInterface()
{

}
