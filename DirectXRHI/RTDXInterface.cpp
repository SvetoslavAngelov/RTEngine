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
	
		//	TODO
		//  Build the raytracing pipeline.
	}
}

void RTDXInterface::CreateDeviceDependentResources()
{
	// Build the raytracing pipeline.

	// Create the raytracing device and command list. 
	CreateRaytracingInterfaces(); 

	// Creat global root signatures for the shaders. 
	CreateGlobalRootSignature(); 

	// Creat global root signatures for the shaders. 
	CreateLocalRootSignature();

	//	Create a raytracing pipeline state object which defines the binding of shaders, state and resources to be used during raytracing. 
	CreateRaytracingPipelineStateObject();

	// Create a heap for the descriptor resources. 
	CreateDescriptorHeap(); 

	// Buil the geometry. 
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
	CD3DX12_DESCRIPTOR_RANGE UAVDescriptor;
	UAVDescriptor.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
	
	CD3DX12_ROOT_PARAMETER rootParameters[2];

	// Output view slot 
	rootParameters[0].InitAsDescriptorTable(1, &UAVDescriptor);

	// Acceleration structure slot
	rootParameters[1].InitAsShaderResourceView(0);

	CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(2, rootParameters);

	Microsoft::WRL::ComPtr<ID3D12Device> device = deviceResources->GetD3DDevice();
	Microsoft::WRL::ComPtr<ID3DBlob> serialisedRootSignature; 
	Microsoft::WRL::ComPtr<ID3DBlob> errorMessage; 

	ThrowIfFailed(D3D12SerializeRootSignature(&globalRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serialisedRootSignature, &errorMessage)),
		errorMessage ? static_cast<wchar_t*>(errorMessage->GetBufferPointer()) : nullptr);

	ThrowIfFailed(device->CreateRootSignature(1, serialisedRootSignature->GetBufferPointer(), serialisedRootSignature->GetBufferSize(), 
		IID_PPV_ARGS(&raytracingGlobalRootSignature)));
}

void RTDXInterface::CreateLocalRootSignature()
{

}


void RTDXInterface::CreateRaytracingPipelineStateObject()
{

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
