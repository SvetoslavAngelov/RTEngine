#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>

/*
	Wrapper for managing memory resources on a DirectX12 compatible GPU. 
	Using Microsoft's DeviceResources.h and DeviceResources.cpp as reference. 
	Source: https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12Raytracing/
*/

class RTDeviceResources {

	// The number of swap chain back buffers.  
	UINT static const MAX_BACK_BUFFER_COUNT = 3;

public: 
	// Default constructor.
	RTDeviceResources(
		DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT,
		UINT backBufferCount = 3,
		D3D_FEATURE_LEVEL minFeatureLevel = D3D_FEATURE_LEVEL_11_0);

	~RTDeviceResources();

	// Configure DXGI factory and retreive an adapter.
	void InitialiseDXGIAdapter();

	// Configures the Direct3D device. 
	void CreateDeviceResources();

	// Set window size and handle. 
	void SetWindow(HWND window, UINT width, UINT height);

	// Create window size dependent resources, including back buffer and swap chain size.
	void CreateWindowSizeDependentResources();

	// Update the window size dependent resources, when the window has been resized.
	void ResizeWindowSizeDependentResources();

	// Prepare the command list and render target for rendering. 
	void Prepare(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_PRESENT);

	// Present the contents of the swap chain. 
	void Present(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Send the command list to the GPU for processing.
	void ExecuteCommandList();

	// Wait for any pending work on the GPU to complete. 
	void WaitForGpu() noexcept;

	// Prepare the render the next frame. 
	void MoveToNextFrame();

	// Check for tearing support. 
	bool CheckTearingSupport(); 

	// Private member accessors. 
	
	ID3D12GraphicsCommandList*				GetCommandList() const { return commandList.Get(); }
	ID3D12Resource*							GetRenderTarget() const { return renderTargets[backBufferIndex].Get(); }
	Microsoft::WRL::ComPtr<ID3D12Device>	GetD3DDevice() const { return d3dDevice; }

	struct CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const; 

private: 
	// Direct3D objects. 
	Microsoft::WRL::ComPtr<IDXGIAdapter1>				d3dAdapter;
	Microsoft::WRL::ComPtr<ID3D12Device>				d3dDevice;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>			commandQueue;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	commandList;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		commandAllocators[MAX_BACK_BUFFER_COUNT];
	UINT												backBufferIndex;

	// Swap chain objects. 
	Microsoft::WRL::ComPtr<IDXGIFactory4>				dxgiFactory; 
	Microsoft::WRL::ComPtr<IDXGISwapChain3>				swapChain; 
	Microsoft::WRL::ComPtr<ID3D12Resource>				renderTargets[MAX_BACK_BUFFER_COUNT]; 
	Microsoft::WRL::ComPtr<ID3D12Resource>				depthStencil;

	// Fence objects for CPU<->GPU synchronisation. 
	Microsoft::WRL::ComPtr<ID3D12Fence>					fence;
	Microsoft::WRL::Wrappers::Event						fenceEvent;
	UINT64												fenceValues[MAX_BACK_BUFFER_COUNT];

	// Direct3D rendering objects. 
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		rtvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		dsvDescriptorHeap;
	UINT												rtvDescriptorSize;
	D3D12_VIEWPORT										screenViewport; 
	D3D12_RECT											scissorRect;

	// Direct3D properties. 
	DXGI_FORMAT											backBufferFormat;
	DXGI_FORMAT											depthBufferFormat;
	UINT												backBufferCount; 

	// Cached device properties 
	std::wstring										d3dAdapterDescription; 
	D3D_FEATURE_LEVEL									d3dFeatureLevel;
	RECT												winOutputSize;
	HWND												windowHandle;
	bool												isTearingSupported;
};
