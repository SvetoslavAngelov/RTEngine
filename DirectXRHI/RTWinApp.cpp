#include "RTWinApp.h"
#include "RTDXInterface.h"

HWND RTWinApp::hwnd = nullptr; 
RECT RTWinApp::windowRect = { 0, 0, 0, 0 };

int RTWinApp::Run(RTDXInterface* rtInterface, HINSTANCE hInstance, int nCmdShow)
{
	try
	{
		// Initialise the window class. 
		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = hInstance; 
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.lpszClassName = L"RTEngine";
		RegisterClassEx(&windowClass);

		windowRect = {
			0,
			0,
			(LONG)rtInterface->GetViewportWidth(),
			(LONG)rtInterface->GetViewportHeight() };

		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

		// Create the window and store a handle to it. 
		hwnd = CreateWindowW(
			windowClass.lpszClassName,
			rtInterface->GetWindowName().c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr,	// No parent window. 
			nullptr,	// No menus.
			hInstance,
			rtInterface
		);

		// Initialise the RTInterface. 
		rtInterface->OnInit(); 

		ShowWindow(hwnd, nCmdShow);

		// Main loop. 
		MSG msg = {}; 
		while (msg.message != WM_QUIT)
		{
			// Process any messages in the queue. 
			if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}

		// Return this part of the WM_QUIT message back to Windows. 
		return (char)msg.wParam;
	}
	catch (std::exception& e)
	{
		OutputDebugString(L"Application hit a problem: ");
		OutputDebugStringA(e.what());
		OutputDebugString(L"\nTerminating.\n");

		return EXIT_FAILURE;
	}
}

LRESULT CALLBACK RTWinApp::WindowProc(HWND ehwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RTDXInterface* rtInterface = reinterpret_cast<RTDXInterface*>(GetWindowLongPtr(ehwnd, GWLP_USERDATA));

	switch (message)
	{
		case WM_CREATE:
		{
			// Set the RTDXInterface* passed in to CreateWindow.
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtrW(ehwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));

		}
			return 0; 

		case WM_PAINT:
			if (rtInterface)
			{
				rtInterface->OnUpdate(); 
				rtInterface->OnRender();
			}
			return 0; 

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	// Handle any messages not in the switch statement. 
	return DefWindowProcW(ehwnd, message, wParam, lParam);
}