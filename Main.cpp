#include "DirectXRHI/stdafx.h"
#include "DirectXRHI/RTDXInterface.h"
#include "DirectXRHI/RTWinApp.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    RTDXInterface rtInterface{ 1280, 720, L"D3D12 RTInterface Debug" };

    return RTWinApp::Run(&rtInterface, hInstance, nCmdShow);
}
