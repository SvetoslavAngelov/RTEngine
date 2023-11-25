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
	aspectRatio { 0.f },
	windowBounds { 0, 0, 0, 0 },
	windowTitle { windowName }
{

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

			// Set up scene 
			

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
	// TODO 
}

void RTDXInterface::BuildAccelerationStructures()
{
	// TODO 
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
		missShaderTable.push_back(ShaderRecord(rayGenShaderIdentifier, shaderIdentifierSize));
		m_missShaderTable = missShaderTable.GetResource();
	}

	// Hit group shader table 
	// TODO 
}

void RTDXInterface::CreateRaytracingOutputResource()
{
	auto commandList = deviceResources->GetCommandList();
	auto renderTarget = deviceResources->GetRenderTarget();

	D3D12_RESOURCE_BARRIER preCopyBarriers[2];
	preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);
	preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(raytracingOutput.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	commandList->ResourceBarrier(ARRAYSIZE(preCopyBarriers), preCopyBarriers);

	commandList->CopyResource(renderTarget, raytracingOutput.Get());

	D3D12_RESOURCE_BARRIER postCopyBarriers[2];
	postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);
	postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(raytracingOutput.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	commandList->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);
}

void RTDXInterface::OnUpdate()
{
	// TODO Tick().
}

void RTDXInterface::DoRayTracing()
{
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

			dispatchDesc->Width = width;
			dispatchDesc->Height = height;
			dispatchDesc->Depth = 1;

			commandList->SetPipelineState1(stateObject);
			commandList->DispatchRays(dispatchDesc);
		};

	auto SetCommonPipelineState = [&](auto* descriptorSetCommandList)
		{
			descriptorSetCommandList->SetDescriptorHeaps(1, descriptorHeap.GetAddressOf());

			// Set index and vertex buffer descriptor tables
			// TODO
		};

	commandList->SetComputeRootSignature(raytracingGlobalRootSignature.Get());

	// Copy the updated scene constant buffer to GPU
	// TODO 

	// Bind the heaps, acceleration structure and dispatch rays
	D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
	SetCommonPipelineState(commandList);
	// commandList->SetComputeRootShaderResourceView(GlobalRootSignatureParams::AccelerationStructureSlot, topLevelAccelerationStructure->GetGPUVirtualAddress());
	DispatchRays(raytracingCommanList.Get(), raytracingStateObject.Get(), &dispatchDesc);
}

void RTDXInterface::OnRender()
{
	deviceResources->Prepare(); 

	// Populate command list.
	/*float const clearColour[] = {0.f, 0.4f, 0.4f, 1.0f};
	auto commandList = deviceResources->GetCommandList();
	auto rtvHandle = deviceResources->GetRenderTargetView(); 
	commandList->ClearRenderTargetView(rtvHandle, clearColour, 0, nullptr);*/

	DoRayTracing();
	CreateRaytracingOutputResource();
	
	deviceResources->Present(D3D12_RESOURCE_STATE_PRESENT);
}

RTDXInterface::~RTDXInterface()
{

}
