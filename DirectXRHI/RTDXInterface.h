#pragma once

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

	// Accessors. 
	UINT GetViewportWidth() const { return width; }
	UINT GetViewportHeight() const { return height; }
	std::wstring GetWindowName() const { return windowTitle; }

protected: 
	// Viewport dimensions. 
	UINT										width; 
	UINT										height; 
	float										aspectRatio;

	// Window bounds. 
	Rect										windowBounds;

	// D3D device resources.
	std::unique_ptr<class RTDeviceResources>	deviceResources;

private:
	std::wstring								windowTitle;
};

