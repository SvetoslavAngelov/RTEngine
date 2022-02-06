#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include <string>

/*
	Base interface class between Direct3D 12 and the application.
	Using Microsoft's DXSample.h and DXSample.cpp as reference.
	Source: https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12Raytracing/
*/

struct Rect {
	long left;
	long top;
	long right; 
	long bottom;
};

class RTDXInterface {

	using UINT = unsigned int;
	using WCHAR = wchar_t;

public: 
	// Default constructor
	RTDXInterface(UINT viewportWidth, UINT viewportHeight, std::wstring windowName);
	virtual ~RTDXInterface();

	virtual void OnInit(); 
	virtual void OnUpdate(); 
	virtual void OnRender(); 

	// Raytracing pipeline.
	void CreateDeviceDependentResources();

	void CreateRaytracingInterfaces(); 
	void CreateGlobalRootSignature();
	void CreateLocalRootSignature();
	void CreateRaytracingPipelineStateObject();
	void CreateDescriptorHeap();
	void BuildGeometry();
	void BuildAccelerationStructures();
	void BuildShaderTables();
	void CreateRaytracingOutputResource();

	// Accessors. 
	UINT GetViewportWidth() const { return width; }
	UINT GetViewportHeight() const { return height; }
	std::wstring GetWindowName() const { return windowTitle; }

protected: 
	// Viewport dimensions. 
	UINT												width; 
	UINT												height; 
	float												aspectRatio;

	// Window bounds. 
	Rect												windowBounds;
		
	// D3D device resources.
	std::unique_ptr<class RTDeviceResources>			deviceResources;
	Microsoft::WRL::ComPtr<ID3D12Device5>				raytracingDevice;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>	raytracingCommanList;
	Microsoft::WRL::ComPtr<ID3D12StateObject>			raytracingStateObject;

	// Root signatures
	Microsoft::WRL::ComPtr<ID3D12RootSignature>			raytracingGlobalRootSignature;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>			raytracingLocalRootSignature;

private:
	std::wstring										windowTitle;
};

