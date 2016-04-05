#include <Windows.h>
#include <d3d.h>
#include "Renderer.h"
#include "RenderMeshOBJ.h"
#include "RenderShape.h"
#include "Time.h"
#include "TextureManager.h"
#if _DEBUG
#include "vld.h"
#endif
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define ZFAR 3000.0f
#define ZNEAR .1f

//  Globals //
CTime gTime;

CRenderShape* water = nullptr;

//  Forward Declarations //
bool Run(void);
void Input(void);
void Initialize();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//  Entry Point //
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//  Initialize Window // 
	WNDCLASSEX wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.lpszClassName = "RenderTest";
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	RegisterClassEx(&wndClass);
	RECT window_size = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);
	HWND window = CreateWindow("RenderTest",
		"RenderTest",
		WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		window_size.right - window_size.left,
		window_size.bottom - window_size.top,
		NULL,
		NULL,
		hInstance,
		NULL);
	ShowWindow(window, SW_SHOW);
	//  Initialize Rendering Engine Singleton //
	CRenderer& rend = CRenderer::GetInstance();
	rend.Initialize(window, WINDOW_WIDTH, WINDOW_HEIGHT, ZNEAR, ZFAR);
	//  Initialize The Game or Scene //
	Initialize();
	//  Handle Windows Messages //
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT && Run())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	//  Shutdown Renderer Singleton //
	CRenderer::GetInstance().Shutdown();
	return 0;
}
//  Initializes the Game or Scene //
void Initialize()
{
	CRenderer& rend = CRenderer::GetInstance();
	CRenderMeshOBJ* dragonite = new CRenderMeshOBJ("../RenderingEngine/Assets/Dragonite.obj");
	dragonite->SetPosition(0, 0.0f, -300.0f);
	dragonite->RotateGlobalX(-90.0f);
	dragonite->GetShaderResourceViews().push_back(rend.GetTextureManager()->GetSRV(L"../RenderingEngine/Assets/Dragonite1.dds"));
	dragonite->GetShaderResourceViews().push_back(rend.GetTextureManager()->GetSRV(L"../RenderingEngine/Assets/Dragonite1.dds"));
	dragonite->GetShaderResourceViews().push_back(rend.GetTextureManager()->GetSRV(L"../RenderingEngine/Assets/Dragonite3.dds"));
	dragonite->GetShaderResourceViews().push_back(rend.GetTextureManager()->GetSRV(L"../RenderingEngine/Assets/Dragonite4.dds"));
	rend.AddRenderable(eContext3D, dragonite);
	
	CRenderMeshOBJ* table = new CRenderMeshOBJ("../RenderingEngine/Assets/Table.obj");
	rend.AddRenderable(eContext3D, table);
	table->UniformScale(3.0f);
	table->SetPosition(0, 65.0f, 0);
	table->GetShaderResourceViews().push_back(rend.GetTextureManager()->GetSRV(L"../RenderingEngine/Assets/Table.dds"));

	CRenderMeshOBJ* cube = new CRenderMeshOBJ("../RenderingEngine/Assets/TestCube.obj",true);
	rend.AddRenderable(eContext3DFrontCulledNorm, cube);
	cube->UniformScale(1200.0f);
	cube->SetPosition(0, 600.0f, 0);
	cube->GetShaderResourceViews().push_back(rend.GetTextureManager()->GetSRV(L"../RenderingEngine/Assets/WoodDiffuse.dds"));
	cube->GetShaderResourceViews().push_back(rend.GetTextureManager()->GetSRV(L"../RenderingEngine/Assets/WoodNormal.dds"));

	CRenderMeshOBJ* bottle = new CRenderMeshOBJ("../RenderingEngine/Assets/Bottle.obj",true);
	rend.AddRenderable(eContext3DTransparent, bottle);
	bottle->SetPosition(0, 165.0f, 0);
	bottle->UniformScale(100.0f);
	bottle->GetShaderResourceViews().push_back(rend.GetTextureManager()->GetSRV(L"../RenderingEngine/Assets/Bottle.dds"));
	bottle->GetShaderResourceViews().push_back(rend.GetTextureManager()->GetSRV(L"../RenderingEngine/Assets/Bottle.dds"));
	bottle->GetShaderResourceViews().push_back(rend.GetTextureManager()->GetSRV(L"../RenderingEngine/Assets/Bump.dds"));
	bottle->GetShaderResourceViews().push_back(rend.GetTextureManager()->GetSRV(L"../RenderingEngine/Assets/Bump.dds"));
	
	CRenderShape* waterPostProcess = new CRenderShape(ERenderShape::eFullScreenQuad);
	rend.AddRenderable(eContextWater, waterPostProcess);

	CRenderMeshOBJ* WaterVolume = new CRenderMeshOBJ("../RenderingEngine/Assets/WaterVolume.obj", false);
	rend.AddRenderable(eContextWaterVolume, WaterVolume);
	WaterVolume->SetPosition(0, 165.0f, 0);
	WaterVolume->UniformScale(100.0f);
	WaterVolume->GetShaderResourceViews().push_back(rend.GetTextureManager()->GetSRV(L"../RenderingEngine/Assets/WoodDiffuse.dds"));

}
//  The "Game Loop" function that will be called every frame //
bool Run(void)
{
	//  Update Time Object //
	gTime.Tick();
	//  Render The Scene //
	CRenderer::GetInstance().Render();
	//  Get and Process input from the user //
	Input();
	//  Return true for normal execution of the program
	return true;
}
//  Gather and Process input from the user //
void Input(void)
{
	if (GetAsyncKeyState(VK_TAB) & 1)
	{
		//Toggle LightMap
		CRenderer::GetInstance().GetLightMap()->SetActive(!CRenderer::GetInstance().GetLightMap()->GetActive());
	}
}
//  Simple WndProc for handling windows messages //
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		message = WM_DESTROY;
	}
	switch (message)
	{
	case(WM_DESTROY) :
	{
		PostQuitMessage(0);
	}
	break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}