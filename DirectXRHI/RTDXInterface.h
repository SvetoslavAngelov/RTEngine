#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include <string>
#include "RTHelper.h"
#include "../Scene/RTScene.h"

/*
	Base interface class between Direct3D 12 and the application.
	Using Microsoft's DXSample.h and DXSample.cpp as reference.
	Source: https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12Raytracing/
*/

enum GlobalRootSignatureParams {
	OutputViewSlot = 0, 
	ConstantBufferSlot,
	AccelerationStructureSlot,
	VertexBufferSlot,
	Count
};

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
	void CreateRaytracingPipelineStateObject();
	void CreateDescriptorHeap();
	void BuildGeometry();
	void BuildAccelerationStructures();
	void BuildShaderTables();
	void CreateRaytracingOutputResource();
	void DoRayTracing();

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

private:

	// Frame count, must match the size of the back buffer
	static const UINT									frameCount = 3;

	std::wstring										windowTitle;

	// Shader tables 
	static const wchar_t*								c_rayGenShaderName; 
	static const wchar_t*								c_closestHitShaderName; 
	static const wchar_t*								c_missShaderName;
	static const wchar_t*								c_hitGroupName;

	Microsoft::WRL::ComPtr<ID3D12Resource>				m_rayGenShaderTable; 
	Microsoft::WRL::ComPtr<ID3D12Resource>				m_missShaderTable; 
	Microsoft::WRL::ComPtr<ID3D12Resource>				m_hitGroupShaderTable;

	// Descriptors 
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		descriptorHeap;
	UINT												descriptorsAllocated;
	UINT												descriptorSize;

	// Raytracing output 
	Microsoft::WRL::ComPtr<ID3D12Resource>				raytracingOutput;
	D3D12_GPU_DESCRIPTOR_HANDLE							raytracingOutputResourceUAVGpuDescriptor;
	UINT												raytracingOutputResourceUAVDescriptorHeapIndex;

	// Raytracing scene 
	SceneConstantBuffer									rtScene[frameCount]; 
	ObjectConstantBuffer								rtObject; 

	// Geometry reference
	D3DBuffer											indexBuffer;
	D3DBuffer											vertexBuffer;
};

