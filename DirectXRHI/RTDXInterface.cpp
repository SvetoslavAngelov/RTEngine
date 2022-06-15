#include "RTDXInterface.h"
#include "RTDeviceResources.h"
#include "RTWinApp.h"
#include "HrException.h"

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
		// TODO

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

	// DXIL library 
	// Shaders cannot be passed as subobjects into the raytracing pipeline. 
	// To add shaders into a state object, they must be put in a containing 
	// DXIL library, which is then passe into the pipeline state object. 
	
	CD3DX12_DXIL_LIBRARY_SUBOBJECT* dxShaderLib = raytracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
	//D3D12_SHADER_BYTECODE dxilLib = CD3DX12_SHADER_BYTECODE((void*)g_pRaytracing)

}

void RTDXInterface::CreateDescriptorHeap()
{

}

void RTDXInterface::BuildGeometry()
{

}

void RTDXInterface::BuildAccelerationStructures()
{

}

void RTDXInterface::BuildShaderTables()
{

}

void RTDXInterface::CreateRaytracingOutputResource()
{

}

void RTDXInterface::OnUpdate()
{
	// TODO Tick().
}

void RTDXInterface::OnRender()
{
	deviceResources->Prepare(); 

	// Populate command list.
	float const clearColour[] = { 0.f, 0.4f, 0.4f, 1.0f };
	auto commandList = deviceResources->GetCommandList();
	auto rtvHandle = deviceResources->GetRenderTargetView(); 
	commandList->ClearRenderTargetView(rtvHandle, clearColour, 0, nullptr);

	deviceResources->Present();
}

RTDXInterface::~RTDXInterface()
{

}
