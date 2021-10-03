#include "RTDXInterface.h"
#include "RTDeviceResources.h"
#include "RTWinApp.h"


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
	}
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
