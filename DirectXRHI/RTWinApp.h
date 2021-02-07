#pragma once

#include "stdafx.h"

/*
	Helper interface class for the Win32 App resources.
	Using Microsoft's Win32Application.h and Win32Application.cpp as reference.
	Source: https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12Raytracing/
*/

class RTWinApp {

public: 
	static int Run(class RTDXInterface* rtInterface, HINSTANCE hInstance, int nCmdShow);
	static HWND GetHwnd() { return hwnd; }

protected: 
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static HWND hwnd; 
	static RECT windowRect; 
};