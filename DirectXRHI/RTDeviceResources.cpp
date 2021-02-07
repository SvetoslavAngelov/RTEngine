#include "RTDeviceResources.h"
#include "HrException.h"
#include <algorithm>
#include <array>
#include "d3dx12.h"

RTDeviceResources::RTDeviceResources(DXGI_FORMAT backBufferFormat, UINT backBufferCount)	:
	backBufferIndex{ 0 },
	fenceValues{},
	rtvDescriptorHeap{ nullptr },
	rtvDescriptorSize{ 0 },
	screenViewport{ },
	scissorRect{ },
	backBufferFormat{ backBufferFormat },
	backBufferCount{ backBufferCount },
	d3dFeatureLevel{ D3D_FEATURE_LEVEL_11_0 }
{
	if (backBufferCount > MAX_BACK_BUFFER_COUNT)
	{
		throw std::out_of_range("ERROR: BackBufferCount too large");
	}
}

RTDeviceResources::~RTDeviceResources()
{
	// Ensure that the GPU is no longer referencing resources that are about to be destroyed.
	WaitForGpu();
}

void RTDeviceResources::InitialiseDXGIAdapter()
{
	bool isDebugDXGI = false; 

#if defined(_DEBUG)
	// Enable the debug layer.
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
	else
	{
		OutputDebugStringA("WARNING: Direct3D debug device is not available\n");
	}

	Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dxgiInfoQueue))))
	{
		isDebugDXGI = true; 

		ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgiFactory)));

		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
	}
#endif

	if (!isDebugDXGI)
	{
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));
	}

	isTearingSupported = CheckTearingSupport();
	
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory6; 

	HRESULT hr = dxgiFactory.As(&dxgiFactory6);

	if (FAILED(hr))
	{
		throw std::exception("DXGI 1.6 interface not supported.");
	}

	// Go through all available hardware adapters a and find the highest performance one. 
	for (UINT adapterID = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory6->EnumAdapterByGpuPreference(adapterID, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)); ++adapterID)
	{
		DXGI_ADAPTER_DESC1 adapterDesc; 
		ThrowIfFailed(adapter->GetDesc1(&adapterDesc));

		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Ignore the basic render driver adapter. 
			continue; 
		}

		// Try and create an adapter which supports Direct3D 12, if unsuccessful try to create a WARP12 adapter.
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
		{
			d3dAdapterDescription = adapterDesc.Description;

#ifdef _DEBUG
			wchar_t buffer[256] = {};
			swprintf_s(buffer, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterID, adapterDesc.VendorId, adapterDesc.DeviceId, adapterDesc.Description);
			OutputDebugStringW(buffer);
#endif
		}
		else if (SUCCEEDED(dxgiFactory6->EnumWarpAdapter(IID_PPV_ARGS(&adapter))))
		{
			OutputDebugStringA("Direct3D adapter - WARP12\n");
		}
		else
		{
			throw std::exception("WARP12 not available. Enable the 'Graphics Tools' optional feature");
		}
	}

	// If valid, assign the newly created adapter. 
	d3dAdapter = adapter.Detach();
}

void RTDeviceResources::CreateDeviceResources()
{
	// Create the DirectX 12 API device object. 
	ThrowIfFailed(D3D12CreateDevice(d3dAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice)));

#ifdef _DEBUG

	Microsoft::WRL::ComPtr<ID3D12InfoQueue> d3dInfoQueue;

	if (SUCCEEDED(d3dDevice.As(&d3dInfoQueue)))
	{
		d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

		D3D12_MESSAGE_ID hide[] =
		{
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
		};

		D3D12_INFO_QUEUE_FILTER filter = { };
		filter.DenyList.NumIDs = _countof(hide);
		filter.DenyList.pIDList = hide;
		d3dInfoQueue->AddStorageFilterEntries(&filter);
	}

#endif

	// Used to determine the maximum supported feature level for the chosen device. 
	static const D3D_FEATURE_LEVEL supportedFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1, 
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevels =
	{
		_countof(supportedFeatureLevels), supportedFeatureLevels, D3D_FEATURE_LEVEL_11_0
	};

	HRESULT hr = d3dDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevels, sizeof(featureLevels));

	if (SUCCEEDED(hr))
	{
		d3dFeatureLevel = featureLevels.MaxSupportedFeatureLevel;
	}

	// Create the command queue. 
	D3D12_COMMAND_QUEUE_DESC queueDesc = { };
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));

	// Create descriptor heaps for the render target views. 
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = { };
	rtvDescriptorHeapDesc.NumDescriptors = backBufferCount;
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV; 

	ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap)));

	rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Create a command allocator for each back buffer that will be rendered to. 
	for (UINT n = 0; n < backBufferCount; ++n)
	{
		ThrowIfFailed(d3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocators[n])
		));
	}

	// Create a command list for recording graphics commands. 
	ThrowIfFailed(d3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocators[0].Get(),
		nullptr,
		IID_PPV_ARGS(&commandList)
	));
	ThrowIfFailed(commandList->Close());

	// Create a fence for tracking GPU execution progress.
	ThrowIfFailed(d3dDevice->CreateFence(
		fenceValues[backBufferIndex],
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&fence)
	));

	fenceValues[backBufferIndex]++;

	fenceEvent.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));
	if (!fenceEvent.IsValid())
	{
		ThrowIfFailed(E_FAIL, L"CreateEvent failed.\n");
	}
}

void RTDeviceResources::SetWindow(HWND window, UINT width, UINT height)
{
	windowHandle = window;

	winOutputSize.left = winOutputSize.top = 0;
	winOutputSize.right = (long)width; 
	winOutputSize.bottom = (long)height;
}

void RTDeviceResources::CreateWindowSizeDependentResources()
{
	if (!windowHandle)
	{
		ThrowIfFailed(E_HANDLE, L"Call DeviceResources->SetWindow() first with a valid Win32 app window handle.\n");
	}
	
	// Set the render target size in pixels.
	long backBufferWidth = std::max<long>(winOutputSize.right - winOutputSize.left, 1);
	long backBufferHeight = std::max<long>(winOutputSize.bottom - winOutputSize.top, 1);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { };
	swapChainDesc.Width = (UINT)backBufferWidth;
	swapChainDesc.Height = (UINT)backBufferHeight;
	swapChainDesc.Format = backBufferFormat;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = backBufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags = isTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0; 

	// Set the level of multi sampling (anti-aliasing) quality. Set to default OFF. 
	swapChainDesc.SampleDesc.Count = 1; 
	swapChainDesc.SampleDesc.Quality = 0;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = { 0 };
	fsSwapChainDesc.Windowed = true; 

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChainTemp;

	ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
		commandQueue.Get(), 
		windowHandle,
		&swapChainDesc, 
		&fsSwapChainDesc,
		nullptr, 
		&swapChainTemp
	));

	ThrowIfFailed(swapChainTemp.As(&swapChain));

	// ALT + Enter key presses will be handled in main window message loop, rather than by DXGI call SetFullScreenState.
	if (isTearingSupported)
	{
		dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER);
	}

	// Obtain the back buffers for this window and create a render target view for each one of them. 
	for (UINT n = 0; n < backBufferCount; ++n)
	{
		ThrowIfFailed(swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n])));

		wchar_t name[25] = { };
		swprintf_s(name, L"Render target %u", n);
		renderTargets[n]->SetName(name);

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = { };
		rtvDesc.Format = backBufferFormat;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), n, rtvDescriptorSize);
		d3dDevice->CreateRenderTargetView(renderTargets[n].Get(), &rtvDesc, rtvDescriptor);
	}

	// Reset the index to the current back buffer.
	backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	// TODO Dept buffer set up. 

	// Set the 3D rendering viewport and scissor rectangle to target the entire window. 
	screenViewport.TopLeftX = screenViewport.TopLeftY = 0.f; 
	screenViewport.Width = (float)backBufferWidth;
	screenViewport.Height = (float)backBufferHeight;
	screenViewport.MinDepth = D3D12_MIN_DEPTH;
	screenViewport.MaxDepth = D3D12_MAX_DEPTH;

	scissorRect.left = scissorRect.top = 0; 
	scissorRect.right = backBufferWidth;
	scissorRect.bottom = backBufferHeight;
}

bool RTDeviceResources::CheckTearingSupport()
{
	bool allowTearing = false; 

	Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
	
	HRESULT hr = dxgiFactory.As(&factory5);
	{
		if (SUCCEEDED(hr))
		{
			hr = factory5->CheckFeatureSupport(
				DXGI_FEATURE_PRESENT_ALLOW_TEARING, 
				&allowTearing, 
				sizeof(allowTearing));
		}

		if (FAILED(hr) || !allowTearing)
		{
			// The interface must be run on an OS which supports tearing.
			ThrowIfFailed(FALSE, L"ERROR: DirectX interface must be run on an OS with tearing support.\n");
		}
	}

	return allowTearing == true;
}

void RTDeviceResources::ResizeWindowSizeDependentResources()
{
	// Wait until all GPU work is complete. 
	WaitForGpu(); 

	// Release the resources tied to the swap chain and update fence values. 
	for (UINT n = 0; n < backBufferCount; ++n)
	{
		renderTargets[n].Reset(); 
		fenceValues[n] = fenceValues[backBufferIndex];
	}

	// Determine the render target size in pixesl.
	long backBufferWidth = std::max<long>(winOutputSize.right - winOutputSize.left, 1);
	long backBufferHeight = std::max<long>(winOutputSize.bottom - winOutputSize.top, 1);

	if (swapChain)
	{
		// If the swap chain exists, resize it. 
		HRESULT hr = swapChain->ResizeBuffers(
			backBufferCount,
			backBufferWidth,
			backBufferHeight,
			backBufferFormat,
			isTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
#ifdef _DEBUG
			char buff[64] = {};
			sprintf_s(
				buff, 
				"Device lost on SwapChain->ResizeBuffers: Reason code 0x%08X\n",
				(hr == DXGI_ERROR_DEVICE_REMOVED) ? d3dDevice->GetDeviceRemovedReason() : hr);

			OutputDebugStringA(buff);
#endif
			// TODO 
			// HandleDeviceLost()

			return; 
		}
		else
		{
			ThrowIfFailed(hr);
		}
	}
}

void RTDeviceResources::Prepare(D3D12_RESOURCE_STATES beforeState)
{
	// Reset command list and allocator. 
	ThrowIfFailed(commandAllocators[backBufferIndex]->Reset());
	ThrowIfFailed(commandList->Reset(commandAllocators[backBufferIndex].Get(), nullptr));

	if (beforeState != D3D12_RESOURCE_STATE_RENDER_TARGET)
	{
		// Transition the render target into the correct state to allow for drawing into it. 
		D3D12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			renderTargets[backBufferIndex].Get(), 
			beforeState,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		commandList->ResourceBarrier(1, &resourceBarrier);
	}
}

void RTDeviceResources::Present(D3D12_RESOURCE_STATES beforeState)
{
	if (beforeState != D3D12_RESOURCE_STATE_PRESENT)
	{
		// Transition the render target to the state that allows it to be presented to the display. 
		D3D12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			renderTargets[backBufferIndex].Get(),
			beforeState,
			D3D12_RESOURCE_STATE_PRESENT
		);

		commandList->ResourceBarrier(1, &resourceBarrier);
	}

	ExecuteCommandList(); 

	HRESULT hr;

	if (isTearingSupported)
	{
		// Recommended to always use tearing if supported when using a sync interval of 0.
		// Note that this will fail if in true fullscreen mode. 
		hr = swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
	}
	else
	{
		hr = swapChain->Present(1, 0);
	}

	// Check if the deveice was reset or lost. If it was, reinitialise the renderer. 
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
#ifdef _DEBUG
		char buff[64] = {};
		sprintf_s(
			buff,
			"Device lost on Present: Reason code 0x%08X\n",
			(hr == ERROR_DEVICE_REMOVED) ? d3dDevice->GetDeviceRemovedReason() : hr);

		OutputDebugStringA(buff);
#endif
		// TODO 
		// HandleDeviceLost()
	}
	else
	{
		ThrowIfFailed(hr);

		MoveToNextFrame();
	}
}

void RTDeviceResources::ExecuteCommandList()
{
	ThrowIfFailed(commandList->Close());
	ID3D12CommandList* commandLists[] = { commandList.Get() };

	// Due to the limited number of command lists, the conversion from size_t to UINT is acceptable.
	commandQueue->ExecuteCommandLists((UINT)std::size(commandLists), commandLists);
}

void RTDeviceResources::WaitForGpu() noexcept
{
	if (commandQueue && fence && fenceEvent.IsValid())
	{
		// Schedule a Signal command in the GPU queue.
		UINT64 value = fenceValues[backBufferIndex];

		if (SUCCEEDED(commandQueue->Signal(fence.Get(), value)))
		{
			// Wait until the Signal has been processed. 
			if (SUCCEEDED(fence->SetEventOnCompletion(value, fenceEvent.Get())))
			{
				WaitForSingleObjectEx(fenceEvent.Get(), INFINITE, false);

				// Increment the fence value for the current frame. 
				fenceValues[backBufferIndex]++;
			}
		}
	}
}

void RTDeviceResources::MoveToNextFrame()
{
	// Schedule a Signal command in the queue. 
	UINT64 const currentFenceValue = fenceValues[backBufferIndex];
	ThrowIfFailed(commandQueue->Signal(fence.Get(), currentFenceValue));

	// Update the back buffer index. 
	backBufferIndex = swapChain->GetCurrentBackBufferIndex(); 

	// If the next frame is not ready to be rendered yet, wait until it is. 
	if (fence->GetCompletedValue() < fenceValues[backBufferIndex])
	{
		ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[backBufferIndex], fenceEvent.Get()));
		WaitForSingleObjectEx(fenceEvent.Get(), INFINITE, false);
	}

	// Set the fence value for the next frame. 
	fenceValues[backBufferIndex] = currentFenceValue + 1;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE RTDeviceResources::GetRenderTargetView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		backBufferIndex,
		rtvDescriptorSize
	);
}