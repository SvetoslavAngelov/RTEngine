#include "DirectXRHI/RTDXInterface.h"
#include "DirectXRHI/RTWinApp.h"
#include "Math/RTMath.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    RTDXInterface rtInterface{ 1280, 720, L"D3D12 RTInterface Debug" };

    Vec3D v3; 
    Vec2D v2;
    Point2D p2;
    Point3D p3;
    Normal3D n3;
    Ray r; 
    Matrix3D m3; 
    Matrix4D m4;

    return RTWinApp::Run(&rtInterface, hInstance, nCmdShow);
}
