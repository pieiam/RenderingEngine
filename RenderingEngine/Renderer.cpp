#include "Renderer.h"
#include "RenderingStates.h"
#include "RenderSet.h"
#include "VertexTypes.h"
#include <unordered_map>
#include "RenderContext.h"
#include "RenderSetTransparent.h"
#include "Shaders.h"
#include "Camera.h"
#include "TextureManager.h"
#include "Lights.h"
#include "ModelManager.h"
#include <dxgi.h>
#pragma comment(lib,"DXGI.lib")
#define RADIANS(degrees) ((3.14f / 180.0f) * degrees)
#define FOV 75
#define RATIO ((float)fWidth / (float)fHeight)
#define MAX_LIGHT_PER_TILE 512
#define TILE_SIZE 16
#define HRRetFalse(hr)if(hr != S_OK){return false;}
using namespace DirectX;
//Anonymous Namespace for use in this file
namespace
{
	//Used for OBJ Loading
	struct VertexMapStruct
	{
		TVertexDefault vertex;
		unsigned int index;
	};
	void BuildProjectionMatrix(TScene& tScene, float fZNear, float fZFar, float fHeight, float fWidth)
	{
		float yScale = 1 / (tan(RADIANS(FOV) / 2)); // 1 / Cotangent of half the FOV (90)
		float xScale = yScale / RATIO; // Width / Height

		tScene.f44ProjectionMatrix =
		{
			xScale, 0, 0, 0,
			0, yScale, 0, 0,
			0, 0, (fZFar / (fZFar - fZNear)), 1,
			0, 0, -(fZFar * fZNear) / (fZFar - fZNear), 0
		};
		XMMATRIX invProj = XMLoadFloat4x4(&tScene.f44ProjectionMatrix);
		invProj = XMMatrixInverse(&XMMatrixDeterminant(invProj), invProj);
		XMStoreFloat4x4(&tScene.f44InvProj, invProj);
	}
}

CShaders* CRenderer::GetShaders()
{
	return m_pcShaders;
}
void CRenderer::CreateRenderSets()
{
	m_vpRenderSets.resize(eContextTotal);
	m_vpRenderNoDepthSets.resize(eContextTotal);
	for (unsigned int i = 0; i < m_vpRenderSets.size(); ++i)
	{
		m_vpRenderSets[i] = new CRenderSet();
		m_vpRenderNoDepthSets[i] = new CRenderSet();
	}
	m_vpTransparentRenderSets.resize(eContextTransparentTotal);
	for (unsigned int i = 0; i < m_vpTransparentRenderSets.size(); ++i)
	{
		m_vpTransparentRenderSets[i] = new CRenderSetTransparent();
	}
	m_vpPostProcessSets.resize(eContextPostProcessTotal);
	for (unsigned int i = 0; i < m_vpPostProcessSets.size(); ++i)
	{
		m_vpPostProcessSets[i] = new CRenderSet();
	}
	//TODO Add Additional Render Sets


	//Default Set
	TShaders tShaders3D;
	tShaders3D.m_pPixelShader = m_pcShaders->GetPixelShader("PS3D");
	tShaders3D.m_pVertexShader = m_pcShaders->GetVertexShader("VS3D");
	TStates tStatesDefault;
	tStatesDefault.eDepthStencilState = eDepthStencilWriteEqual;
	ID3D11InputLayout* pDefaultIA = m_pcShaders->GetInputLayout(tShaders3D.m_pVertexShader);
	CRenderContext* m_pRenderContext = new CRenderContext(tShaders3D,tStatesDefault,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pDefaultIA);
	m_vpRenderSets[eContext3D]->SetRenderContext(m_pRenderContext);
	m_vpRenderNoDepthSets[eContext3D]->SetRenderContext(new CRenderContext(*m_pRenderContext));
	//Transparent
	TShaders tTransShaders;
	tTransShaders.m_pPixelShader = m_pcShaders->GetPixelShader("PSRefraction");
	tTransShaders.m_pVertexShader = m_pcShaders->GetVertexShader("VS3D");
	TStates tTransStates;
	tTransStates.eBlendState = EBlendState::eBlendStateActive;
	tTransStates.eDepthStencilState = EDepthStencilState::eDepthStencilBackIncr;
	tTransStates.eRasterizerState = ERasterizerState::eRasterizerStateDefault;
	m_pRenderContext = new CRenderContext(tTransShaders, tTransStates,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pDefaultIA);
	m_vpTransparentRenderSets[eContext3DTransparent]->SetRenderContext(m_pRenderContext);

	//Front Culled Set
	tStatesDefault.eRasterizerState = ERasterizerState::eRasterizerStateFrontCulling;
	m_pRenderContext = new CRenderContext(tShaders3D, tStatesDefault,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pDefaultIA);
	m_vpRenderSets[eContext3DFrontCulled]->SetRenderContext(m_pRenderContext);
	m_vpRenderNoDepthSets[eContext3DFrontCulled]->SetRenderContext(new CRenderContext(*m_pRenderContext));
	//Normal Mapped Set
	tStatesDefault.eRasterizerState = ERasterizerState::eRasterizerStateDefault;
	tShaders3D.m_pPixelShader = m_pcShaders->GetPixelShader("PS3DNorm");

	m_pRenderContext = new CRenderContext(tShaders3D, tStatesDefault,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pDefaultIA);
	m_vpRenderSets[eContext3DNorm]->SetRenderContext(m_pRenderContext);
	m_vpRenderNoDepthSets[eContext3DNorm]->SetRenderContext(new CRenderContext(*m_pRenderContext));

	//Front Culled + Normal Mapped Set
	tStatesDefault.eRasterizerState = ERasterizerState::eRasterizerStateFrontCulling;
	m_pRenderContext = new CRenderContext(tShaders3D, tStatesDefault,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pDefaultIA);
	m_vpRenderSets[eContext3DFrontCulledNorm]->SetRenderContext(m_pRenderContext);
	m_vpRenderNoDepthSets[eContext3DFrontCulledNorm]->SetRenderContext(new CRenderContext(*m_pRenderContext));

	//Debug Lights Set
	TStates tStatesLights;
	tStatesLights.eBlendState = eBlendStateInactive;
	tStatesLights.eDepthStencilState = eDepthStencilNoWrite;
	tStatesLights.eRasterizerState = eRasterizerStateWireframe;
	TShaders tShadersLights;
	tShadersLights.m_pPixelShader = m_pcShaders->GetPixelShader("PSDepth"); 
	tShadersLights.m_pVertexShader = m_pcShaders->GetVertexShader("VS3D");
	m_pRenderContext = new CRenderContext(tShadersLights, tStatesLights,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pDefaultIA);
	m_vpRenderSets[eContextWireframe]->SetRenderContext(m_pRenderContext);
	m_vpRenderNoDepthSets[eContextWireframe]->SetRenderContext(new CRenderContext(*m_pRenderContext));

	//Light Map PostProcess
	TShaders tShadersPostProcess;
	tShadersPostProcess.m_pPixelShader = m_pcShaders->GetPixelShader("PSDebugLightMap");
	tShadersPostProcess.m_pVertexShader = m_pcShaders->GetVertexShader("VSPassThrough");
	TStates tStatesPostProcess;
	tStatesPostProcess.eBlendState = EBlendState::eBlendStateActive;
	tStatesPostProcess.eDepthStencilState = eDepthStencilNoWrite;
	m_pRenderContext = new CRenderContext(tShadersPostProcess, tStatesPostProcess, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pDefaultIA);
	m_vpPostProcessSets[eContextLightMap]->SetRenderContext(m_pRenderContext);

	//Color Buffer Full Screen Process
	tShadersPostProcess.m_pPixelShader = m_pcShaders->GetPixelShader("PSColorBuffer");
	tStatesPostProcess.eBlendState = EBlendState::eBlendStateInactive;
	tStatesPostProcess.eDepthStencilState = eDepthStencilNoWrite;
	m_pRenderContext = new CRenderContext(tShadersPostProcess, tStatesPostProcess, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pDefaultIA);
	m_vpPostProcessSets[eContextColorBuffer]->SetRenderContext(m_pRenderContext);

	//Water Post Process
	tShadersPostProcess.m_pPixelShader = m_pcShaders->GetPixelShader("PSWater");
	tStatesPostProcess.eBlendState = EBlendState::eBlendStateActive;
	tStatesPostProcess.eDepthStencilState = eDepthStencilEqual;
	tStatesPostProcess.unStencilRef = 2;
	m_pRenderContext = new CRenderContext(tShadersPostProcess, tStatesPostProcess, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pDefaultIA);
	m_vpPostProcessSets[eContextWater]->SetRenderContext(m_pRenderContext);

	//Water Volume
	TShaders tWaterShaders;
	tWaterShaders.m_pPixelShader = m_pcShaders->GetPixelShader("PSDepth");
	tWaterShaders.m_pVertexShader = m_pcShaders->GetVertexShader("VS3D");
	TStates tWaterStates;
	tWaterStates.eBlendState = EBlendState::eBlendStateNoColor;
	tWaterStates.eDepthStencilState = EDepthStencilState::eStencilEqualIncr;
	tWaterStates.eRasterizerState = ERasterizerState::eRasterizerStateDefault;
	tWaterStates.unStencilRef = 1;
	m_pRenderContext = new CRenderContext(tWaterShaders, tWaterStates, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pDefaultIA);
	m_vpTransparentRenderSets[eContextWaterVolume]->SetRenderContext(m_pRenderContext);

}
//Singleton
bool CRenderer::Initialize(HWND hWnd, UINT width, UINT height, float fZNear, float fZFar)
{
	HRESULT hr;
	CD3D11_DEFAULT d3dDefaultValues;
	m_nResolutionHeight = height;
	m_nResolutionWidth = width;

	DXGI_SWAP_CHAIN_DESC d3dSwapDesc;
	ZeroMemory(&d3dSwapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	d3dSwapDesc.BufferCount = 1;
	d3dSwapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	d3dSwapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	d3dSwapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dSwapDesc.BufferDesc.RefreshRate.Numerator = 60;
	d3dSwapDesc.BufferDesc.RefreshRate.Denominator = 1;
	d3dSwapDesc.BufferDesc.Height = m_nResolutionHeight;
	d3dSwapDesc.BufferDesc.Width = m_nResolutionWidth;
	d3dSwapDesc.OutputWindow = hWnd;
	d3dSwapDesc.SampleDesc.Count = 1;
	d3dSwapDesc.SampleDesc.Quality = 0;
	d3dSwapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	d3dSwapDesc.Windowed = true;

	D3D_FEATURE_LEVEL d3dlevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	D3D_FEATURE_LEVEL d3dlevel;
	
	//Get Highest Memory Adapter
	IDXGIFactory * pTempFactory;
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pTempFactory));
	unsigned int unAdapterIndex = 0;
	IDXGIAdapter* pTempAdapter;
	std::vector<IDXGIAdapter*> vpAdapters;
	while (pTempFactory->EnumAdapters(unAdapterIndex, &pTempAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		vpAdapters.push_back(pTempAdapter);
		++unAdapterIndex;
	}
	DXGI_ADAPTER_DESC AdapterDesc;
	SIZE_T VideoMemorySize = 0;
	for (unsigned int i = 0; i < vpAdapters.size(); ++i)
	{
		hr = vpAdapters[i]->GetDesc(&AdapterDesc);
		if (AdapterDesc.DedicatedVideoMemory > VideoMemorySize)
		{
			unAdapterIndex = i;
			VideoMemorySize = AdapterDesc.DedicatedVideoMemory;
		}
	}
	SAFE_RELEASE(pTempFactory);
	hr = D3D11CreateDeviceAndSwapChain(
		vpAdapters[unAdapterIndex],
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
#if _DEBUG
		D3D11_CREATE_DEVICE_DEBUG,
#else
		D3D11_CREATE_DEVICE_SINGLETHREADED,
#endif
		d3dlevels,
		3,
		D3D11_SDK_VERSION,
		&d3dSwapDesc,
		&m_pd3dSwapChain,
		&m_pd3dDevice,
		&d3dlevel,
		&m_pd3dContext);
	HRRetFalse(hr);
	m_pd3dSwapChain->GetBuffer(0, __uuidof(m_pd3dBackBuffer), reinterpret_cast<void**>(&m_pd3dBackBuffer));
	hr = m_pd3dDevice->CreateRenderTargetView(m_pd3dBackBuffer, NULL, &m_pd3dRenderTargetView);
	HRRetFalse(hr);
	m_d3dViewport.Height = (float)height;
	m_d3dViewport.Width = (float)width;
	m_d3dViewport.MaxDepth = 1.0f;
	m_d3dViewport.MinDepth = 0.0f;
	m_d3dViewport.TopLeftX = 0;
	m_d3dViewport.TopLeftY = 0;
	m_pd3dContext->RSSetViewports(1, &m_d3dViewport);
	CD3D11_TEXTURE2D_DESC d3dDepthDesc(DXGI_FORMAT_R24G8_TYPELESS,width,height,1,0,D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE);
	d3dDepthDesc.MipLevels = 1;
	hr = m_pd3dDevice->CreateTexture2D(&d3dDepthDesc, NULL, &m_pd3dDepthStencilBuffer);
	HRRetFalse(hr);
	ID3D11Texture2D* tempText2d = nullptr;
	hr = m_pd3dDevice->CreateTexture2D(&d3dDepthDesc, NULL, &tempText2d);
	HRRetFalse(hr);

	CD3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	hr = m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, &m_pd3dDepthStencilView);	
	HRRetFalse(hr);
	hr = m_pd3dDevice->CreateDepthStencilView(tempText2d, &d3dDepthStencilViewDesc, &m_pd3dTransparencyDepthStencilView);
	HRRetFalse(hr);

	D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVdesc;
	ZeroMemory(&d3dSRVdesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	d3dSRVdesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	d3dSRVdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	d3dSRVdesc.Texture2D.MostDetailedMip = 0;
	d3dSRVdesc.Texture2D.MipLevels = 1;
	hr = m_pd3dDevice->CreateShaderResourceView(m_pd3dDepthStencilBuffer,&d3dSRVdesc, &m_pd3dDepthStencilSRV);
	HRRetFalse(hr);
	hr = m_pd3dDevice->CreateShaderResourceView(tempText2d, &d3dSRVdesc, &m_pd3dTransparencyDepthSRV);
	HRRetFalse(hr);
	SAFE_RELEASE(tempText2d);

	CD3D11_SAMPLER_DESC d3dSamplerDesc(d3dDefaultValues);
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = m_pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &m_pd3dSampleState);
	HRRetFalse(hr);

	m_pd3dContext->PSSetSamplers(0, 1, &m_pd3dSampleState);

	m_pcTextureManager = new CTextureManager();
	m_pcShaders = new CShaders();
	m_pcModelManager = new CModelManager();
	m_pcRenderingStates = new CRenderingStates();
	m_pcConstantBuffers = new CConstantBuffers();
	CreateRenderSets();
	m_pcCamera = new CCamera();
	m_pcLights = new CLights();
	//Create Projection Matrix
	BuildProjectionMatrix(m_tScene, fZNear, fZFar, (float)height, (float)width);	

	//Create Lighting Resources
	unsigned int unNumTiles = (width / TILE_SIZE) * (height / TILE_SIZE);
	D3D11_BUFFER_DESC d3dLightBufferDesc;
	ZeroMemory(&d3dLightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dLightBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	d3dLightBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dLightBufferDesc.ByteWidth = sizeof(unsigned int) * unNumTiles * MAX_LIGHT_PER_TILE;
	hr = m_pd3dDevice->CreateBuffer(&d3dLightBufferDesc, NULL, &m_pd3dLightBuffer);
	HRRetFalse(hr);

	D3D11_UNORDERED_ACCESS_VIEW_DESC d3dUAVDesc;
	ZeroMemory(&d3dUAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	d3dUAVDesc.Buffer.FirstElement = 0;
	d3dUAVDesc.Buffer.NumElements = MAX_LIGHT_PER_TILE * unNumTiles;
	d3dUAVDesc.Format = DXGI_FORMAT_R32_UINT;
	d3dUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	hr = m_pd3dDevice->CreateUnorderedAccessView(m_pd3dLightBuffer, &d3dUAVDesc, &m_pd3dLightUAV);
	HRRetFalse(hr);

	D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc;
	ZeroMemory(&d3dSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	d3dSRVDesc.Buffer.FirstElement = 0;
	d3dSRVDesc.Buffer.NumElements = MAX_LIGHT_PER_TILE * unNumTiles;
	d3dSRVDesc.Format = DXGI_FORMAT_R32_UINT;
	d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	hr = m_pd3dDevice->CreateShaderResourceView(m_pd3dLightBuffer, &d3dSRVDesc, &m_pd3dLightSRV);
	HRRetFalse(hr);

	hr = m_pd3dContext->QueryInterface(__uuidof(m_pUserDefinedAnnotation), reinterpret_cast<void**>(&m_pUserDefinedAnnotation));
	HRRetFalse(hr);

	//Create Render To Texture Color Buffer
	D3D11_TEXTURE2D_DESC d3dRTTTexDesc;
	ZeroMemory(&d3dRTTTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
	d3dRTTTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	d3dRTTTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	d3dRTTTexDesc.Width = width;
	d3dRTTTexDesc.Height = height;
	d3dRTTTexDesc.MipLevels = 1;
	d3dRTTTexDesc.ArraySize = 1;
	d3dRTTTexDesc.SampleDesc.Count = 1;
	d3dRTTTexDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dRTTTexDesc.CPUAccessFlags = 0;
	d3dRTTTexDesc.MiscFlags = 0;
	hr = m_pd3dDevice->CreateTexture2D(&d3dRTTTexDesc, NULL, &m_tRTTColorBuffer.pTexture2D);
	HRRetFalse(hr);

	D3D11_RENDER_TARGET_VIEW_DESC d3dRTVDesc;
	ZeroMemory(&d3dRTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	d3dRTVDesc.Format = d3dRTTTexDesc.Format;
	d3dRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	d3dRTVDesc.Texture2D.MipSlice = 0;
	hr = m_pd3dDevice->CreateRenderTargetView(m_tRTTColorBuffer.pTexture2D, &d3dRTVDesc, &m_tRTTColorBuffer.pRTV);
	HRRetFalse(hr);

	D3D11_SHADER_RESOURCE_VIEW_DESC d3dRTTSRVDesc;
	ZeroMemory(&d3dRTTSRVDesc, sizeof(d3dRTTSRVDesc));
	d3dRTTSRVDesc.Format = d3dRTTTexDesc.Format;
	d3dRTTSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	d3dRTTSRVDesc.Texture2D.MostDetailedMip = 0;
	d3dRTTSRVDesc.Texture2D.MipLevels = 1;
	hr = m_pd3dDevice->CreateShaderResourceView(m_tRTTColorBuffer.pTexture2D, &d3dRTTSRVDesc, &m_tRTTColorBuffer.pSRV);
	HRRetFalse(hr);

	return true;
}
void  CRenderer::Shutdown()
{
	SAFE_RELEASE(m_pUserDefinedAnnotation);
	SAFE_RELEASE(m_pd3dBackBuffer);
	SAFE_RELEASE(m_pd3dContext);
	SAFE_RELEASE(m_pd3dDepthStencilBuffer);
	SAFE_RELEASE(m_pd3dDepthStencilSRV);
	SAFE_RELEASE(m_pd3dDepthStencilView);
	SAFE_RELEASE(m_pd3dDevice);
	SAFE_RELEASE(m_pd3dTransparencyDepthStencilView);
	SAFE_RELEASE(m_pd3dRenderTargetView);
	SAFE_RELEASE(m_pd3dSwapChain);
	SAFE_RELEASE(m_pd3dSampleState); 
	SAFE_RELEASE(m_pd3dLightBuffer);
	SAFE_RELEASE(m_pd3dLightSRV);
	SAFE_RELEASE(m_pd3dTransparencyDepthSRV);
	SAFE_RELEASE(m_pd3dLightUAV);
	SAFE_DELETE(m_pcRenderingStates);
	SAFE_DELETE(m_pcConstantBuffers);
	SAFE_DELETE(m_pcModelManager);
	SAFE_DELETE(m_pcCamera);
	SAFE_DELETE(m_pcShaders);
	SAFE_DELETE(m_pcTextureManager);
	SAFE_DELETE(m_pcLights);
	SAFE_RELEASE(m_tRTTColorBuffer.pRTV);
	SAFE_RELEASE(m_tRTTColorBuffer.pSRV);
	SAFE_RELEASE(m_tRTTColorBuffer.pTexture2D);

	for (unsigned int i = 0; i < m_vpRenderSets.size(); ++i)
	{
		SAFE_DELETE(m_vpRenderSets[i]);
	}
	for (unsigned int i = 0; i < m_vpRenderNoDepthSets.size(); ++i)
	{
		SAFE_DELETE(m_vpRenderNoDepthSets[i]);
	}
	for (unsigned int i = 0; i < m_vpTransparentRenderSets.size(); ++i)
	{
		SAFE_DELETE(m_vpTransparentRenderSets[i]);
	}
	for (unsigned int i = 0; i < m_vpPostProcessSets.size(); ++i)
	{
		SAFE_DELETE(m_vpPostProcessSets[i]);
	}
}
CRenderer& CRenderer::GetInstance()
{
	static CRenderer instance;

	return instance;
}

//Accessors
ID3D11Device*  CRenderer::GetDevice()
{
	return m_pd3dDevice;
}
ID3D11DeviceContext*  CRenderer::GetContext()
{
	return m_pd3dContext;
}
IDXGISwapChain*  CRenderer::GetSwapChain()
{
	return m_pd3dSwapChain;
}

ID3D11RenderTargetView*  CRenderer::GetRenderTargetView()
{
	return m_pd3dRenderTargetView;
}
ID3D11Texture2D*  CRenderer::GetBackBuffer()
{
	return m_pd3dBackBuffer;
}

ID3D11Texture2D*  CRenderer::GetDepthStencilBuffer()
{
	return m_pd3dDepthStencilBuffer;
}
ID3D11DepthStencilView*  CRenderer::GetDepthStencilView()
{
	return m_pd3dDepthStencilView;
}
ID3D11ShaderResourceView*  CRenderer::GetDepthStencilShaderResourceView()
{
	return m_pd3dDepthStencilSRV;
}
TRenderToTexture CRenderer::GetRTTColorBuffer()
{
	return m_tRTTColorBuffer;
}
D3D11_VIEWPORT  CRenderer::GetViewport()
{
	return m_d3dViewport;
}
CRenderingStates* CRenderer::GetStates()
{
	return m_pcRenderingStates;
}
CConstantBuffers* CRenderer::GetConstantBuffers()
{
	return m_pcConstantBuffers;
}
CTextureManager* CRenderer::GetTextureManager()
{
	return m_pcTextureManager;
}
CModelManager* CRenderer::GetModelManager()
{
	return m_pcModelManager;
}
void CRenderer::AddRenderable(EContext eContext, CRenderable* pcRenderable, bool bDepthPrepass)
{
	if (bDepthPrepass)
		m_vpRenderSets[eContext]->GetRenderables().push_back(pcRenderable);
	else
		m_vpRenderNoDepthSets[eContext]->GetRenderables().push_back(pcRenderable);
}
void CRenderer::AddRenderable(EContextTransparent eContext, CRenderable* pcRenderable)
{
	m_vpTransparentRenderSets[eContext]->GetRenderables().push_back(pcRenderable);
}
void CRenderer::AddRenderable(EContextPostProcess eContext, CRenderable* pcRenderable)
{
	m_vpPostProcessSets[eContext]->GetRenderables().push_back(pcRenderable);
}
void CRenderer::Render()
{ 
	//Update Scene
	m_pUserDefinedAnnotation->BeginEvent(L"Updating Scene");
	m_pcCamera->CameraMovement();
	m_tScene.f4CameraPosition.x = m_pcCamera->GetWorldMatrix().m[3][0];
	m_tScene.f4CameraPosition.y = m_pcCamera->GetWorldMatrix().m[3][1];
	m_tScene.f4CameraPosition.z = m_pcCamera->GetWorldMatrix().m[3][2];
	m_tScene.f4CameraPosition.w = m_pcCamera->GetWorldMatrix().m[3][3];
	XMMATRIX invViewMatrix = XMLoadFloat4x4(&m_pcCamera->GetWorldMatrix());
	invViewMatrix = XMMatrixInverse(&XMMatrixDeterminant(invViewMatrix), invViewMatrix);
	XMStoreFloat4x4(&m_tScene.f44ViewMatrix, invViewMatrix);
	m_pcConstantBuffers->MapToConstantBuffer(&m_tScene, sizeof(TScene), EConstantBuffer::eSceneBuffer);
	m_pd3dContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);
	m_pUserDefinedAnnotation->EndEvent();
	//Update Lighting
	m_pUserDefinedAnnotation->BeginEvent(L"Updating Lights");
	m_pcLights->UpdateLights();
	m_pUserDefinedAnnotation->EndEvent();
	//Clear Render Target and depth buffer
	m_pUserDefinedAnnotation->BeginEvent(L"Clear Depth Stencil and RTV");
	unsigned int clear = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
	m_pd3dContext->ClearDepthStencilView(m_pd3dDepthStencilView, clear, 1.0f, 0);
	m_pUserDefinedAnnotation->EndEvent();
	//Do Depth Prepass
	m_pUserDefinedAnnotation->BeginEvent(L"Depth Prepass Opaque");
	for (unsigned int i = 0; i < m_vpRenderSets.size(); ++i)
	{
		m_vpRenderSets[i]->DepthProcess();
	}
	m_pd3dContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dTransparencyDepthStencilView);
	m_pUserDefinedAnnotation->EndEvent();
	//Depth Prepass of Transparent Object to find min depth
	m_pUserDefinedAnnotation->BeginEvent(L"Depth Prepass Transparent");
	for (unsigned int i = 0; i < m_vpTransparentRenderSets.size(); ++i)
	{
		m_vpTransparentRenderSets[i]->DepthProcess();
	}
	m_pd3dContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, NULL);
	m_pd3dContext->CSSetShaderResources(4, 1, &m_pd3dDepthStencilSRV);
	m_pd3dContext->CSSetShaderResources(5, 1, &m_pd3dTransparencyDepthSRV);
	m_pUserDefinedAnnotation->EndEvent();
	//Cull Lights
	m_pUserDefinedAnnotation->BeginEvent(L"Light Culling");
	ID3D11ShaderResourceView* pd3dNullSRV = NULL;
	ID3D11UnorderedAccessView* pd3dNullUAV = NULL;
	m_pd3dContext->PSSetShaderResources(3, 1, &pd3dNullSRV);
	m_pd3dContext->CSSetUnorderedAccessViews(0, 1, &m_pd3dLightUAV, nullptr);//Cant Set to CS and PS at same time
	m_pd3dContext->CSSetShader(m_pcShaders->GetComputeShader("CSLightCull"), nullptr, 0);
	unsigned int unTilesPerRow = m_nResolutionWidth / TILE_SIZE;
	unsigned int unTilesPerColumn = m_nResolutionHeight / TILE_SIZE;
	m_pd3dContext->Dispatch(unTilesPerRow, unTilesPerColumn, 1);
	m_pUserDefinedAnnotation->EndEvent();
	//Bind Light Index List
	m_pUserDefinedAnnotation->BeginEvent(L"Bind Light Index and Depth Buffer");
	m_pd3dContext->CSSetUnorderedAccessViews(0, 1, &pd3dNullUAV, nullptr);
	m_pd3dContext->PSSetShaderResources(3, 1, &m_pd3dLightSRV); //Cant Set to CS and PS at same time
	//Rebind Depth Buffer
	m_pd3dContext->CSSetShaderResources(4, 1,&pd3dNullSRV);
	m_pd3dContext->CSSetShaderResources(5, 1, &pd3dNullSRV);
	m_pd3dContext->PSSetShaderResources(6, 1, &pd3dNullSRV);
	m_pd3dContext->OMSetRenderTargets(1, &m_tRTTColorBuffer.pRTV, m_pd3dDepthStencilView);
	m_pUserDefinedAnnotation->EndEvent();
	//Render Objects
	m_pUserDefinedAnnotation->BeginEvent(L"Render Opaque Objects");
	for (unsigned int i = 0; i < m_vpRenderSets.size(); ++i)
	{
		m_vpRenderSets[i]->Process();
	}
	for (unsigned int i = 0; i < m_vpRenderNoDepthSets.size(); ++i)
	{
		m_vpRenderNoDepthSets[i]->Process();
	}
	m_pUserDefinedAnnotation->EndEvent();
	//Render Color Buffer
	m_pd3dContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);
	m_pd3dContext->PSSetShaderResources(6, 1, &m_tRTTColorBuffer.pSRV);
	m_pUserDefinedAnnotation->BeginEvent(L"Render ColorBuffer");
	m_vpPostProcessSets[eContextColorBuffer]->Process();
	m_pUserDefinedAnnotation->EndEvent();
	//Render Transparent Objects
	m_pUserDefinedAnnotation->BeginEvent(L"Render Transparent Objects");
	for (unsigned int i = 0; i < m_vpTransparentRenderSets.size(); ++i)
	{
		m_vpTransparentRenderSets[i]->Process();
	}
	m_pUserDefinedAnnotation->EndEvent();
	//Render Post Process Effects
	m_pUserDefinedAnnotation->BeginEvent(L"Render Post Process Objects");
	for (unsigned int i = 0; i < m_vpPostProcessSets.size(); ++i)
	{
		if (i != eContextColorBuffer)
			m_vpPostProcessSets[i]->Process();
	}
	m_pUserDefinedAnnotation->EndEvent();
	m_pUserDefinedAnnotation->BeginEvent(L"Present");
	m_pd3dSwapChain->Present(1, 0);
	m_pUserDefinedAnnotation->EndEvent();
}
void CRenderer::LoadObjFile(ID3D11Buffer** _indexBuffer, ID3D11Buffer** _vertexBuffer, std::string _filename, unsigned int *& _numIndicesPerPiece, unsigned int &_numPieces)
{
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> textureCoords;
	std::vector<unsigned int> indicesvec;
	std::vector<TVertexDefault> uniquevertsvec;
	std::vector<unsigned int> numindexes;
	unsigned int mtlcounter = 0;
	unsigned int indexcounter = 0;
	std::unordered_map<std::string, VertexMapStruct> uniquevertsmap;
	FILE * fin;
	fopen_s(&fin, _filename.c_str(), "r");
	if (fin == NULL)
	{
		assert(0 && "LOADOBJFILE ERROR");
		return;
	}
	while (true)
	{
		char linestart[128];

		int result = fscanf_s(fin, "%s", linestart, sizeof(linestart));
		if (result == EOF)
			break;
		if (strcmp(linestart, "v") == 0)
		{
			XMFLOAT3 pos;
			fscanf_s(fin, "%f %f %f\n", &pos.x, &pos.y, &pos.z);
			positions.push_back(pos);
		}
		else if (strcmp(linestart, "vt") == 0)
		{
			XMFLOAT2 uvs;
			fscanf_s(fin, "%f %f\n", &uvs.x, &uvs.y);
			uvs.y = 1 - uvs.y;
			textureCoords.push_back(uvs);
		}
		else if (strcmp(linestart, "vn") == 0)
		{
			XMFLOAT3 norm;
			fscanf_s(fin, "%f %f %f\n", &norm.x, &norm.y, &norm.z);
			normals.push_back(norm);
		}
		else if (strcmp(linestart, "f") == 0)
		{
			unsigned int PositionIndex[4], NormalsIndex[4], UVSIndex[4];
			int matches = fscanf_s(fin, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &PositionIndex[0], &UVSIndex[0], &NormalsIndex[0],
				&PositionIndex[1], &UVSIndex[1], &NormalsIndex[1],
				&PositionIndex[2], &UVSIndex[2], &NormalsIndex[2],
				&PositionIndex[3], &UVSIndex[3], &NormalsIndex[3]);
			if (matches == 12)
			{
				unsigned int localindices[4];
				//Quad
				for (unsigned int i = 0; i < 4; ++i)
				{

					std::string key = std::to_string(PositionIndex[i]) + std::to_string(NormalsIndex[i]) + std::to_string(UVSIndex[i]);
					//TEST to see if unique
					if (uniquevertsmap.find(key) == uniquevertsmap.end())
					{
						//New vert
						TVertexDefault univert;
						univert.position.m[0] = positions[PositionIndex[i] - 1].x;
						univert.position.m[1] = positions[PositionIndex[i] - 1].y;
						univert.position.m[2] = positions[PositionIndex[i] - 1].z;
						univert.position.m[3] = 1.0f;


						univert.normal.m[0] = normals[NormalsIndex[i] - 1].x;
						univert.normal.m[1] = normals[NormalsIndex[i] - 1].y;
						univert.normal.m[2] = normals[NormalsIndex[i] - 1].z;

						univert.texCoords.m[0] = textureCoords[UVSIndex[i] - 1].x;
						univert.texCoords.m[1] = textureCoords[UVSIndex[i] - 1].y;

						//Place new vert in map
						uniquevertsmap[key].vertex = univert;
						uniquevertsmap[key].index = (unsigned int)uniquevertsvec.size();
						//Grab index of vert and place into the vector
						localindices[i] = (unsigned int)uniquevertsvec.size();
						uniquevertsvec.push_back(univert);
					}
					else
					{
						//found vert

						//Get the index of vertex in the vector
						localindices[i] = uniquevertsmap.find(key)->second.index;
					}
				}
				//Create Indices for triangles in quad

				//first triangle
				indicesvec.push_back(localindices[0]);
				indicesvec.push_back(localindices[1]);
				indicesvec.push_back(localindices[2]);
				//second triangle
				indicesvec.push_back(localindices[0]);
				indicesvec.push_back(localindices[2]);
				indicesvec.push_back(localindices[3]);
				indexcounter += 6;

			}
			else if (matches == 9)
			{
				//Triangle
				unsigned int localindices[3];
				for (unsigned int i = 0; i < 3; ++i)
				{
					std::string key = std::to_string(PositionIndex[i]) + std::to_string(NormalsIndex[i]) + std::to_string(UVSIndex[i]);
					//TEST to see if unique
					if (uniquevertsmap.find(key) == uniquevertsmap.end())
					{
						//New vert
						TVertexDefault univert;
						univert.position.m[0] = positions[PositionIndex[i] - 1].x;
						univert.position.m[1] = positions[PositionIndex[i] - 1].y;
						univert.position.m[2] = positions[PositionIndex[i] - 1].z;
						univert.position.m[3] = 1.0f;

						univert.normal.m[0] = normals[NormalsIndex[i] - 1].x;
						univert.normal.m[1] = normals[NormalsIndex[i] - 1].y;
						univert.normal.m[2] = normals[NormalsIndex[i] - 1].z;

						univert.texCoords.m[0] = textureCoords[UVSIndex[i] - 1].x;
						univert.texCoords.m[1] = textureCoords[UVSIndex[i] - 1].y;

						//Place new vert in map
						uniquevertsmap[key].vertex = univert;
						uniquevertsmap[key].index = (unsigned int)uniquevertsvec.size();
						//Grab index of vert and place into the vector
						localindices[i] = (unsigned int)uniquevertsvec.size();
						uniquevertsvec.push_back(univert);
					}
					else
					{
						//found vert
						//Get the index of vertex in the vector
						localindices[i] = uniquevertsmap.find(key)->second.index;
					}
				}
				//Create Indices for triangle
				indicesvec.push_back(localindices[0]);
				indicesvec.push_back(localindices[1]);
				indicesvec.push_back(localindices[2]);
				indexcounter += 3;
			}
		}
		else if (strcmp(linestart, "usemtl") == 0)
		{
			mtlcounter++;
			if (mtlcounter == 1)
			{
				numindexes.clear();
				continue;
			}
			numindexes.push_back(indexcounter);
			indexcounter = 0;
		}
	}
	//Vertexs
	TVertexDefault* _verts = new TVertexDefault[uniquevertsvec.size()];
	for (unsigned int i = 0; i < uniquevertsvec.size(); ++i)
	{
		_verts[i] = uniquevertsvec[i];
	}
	CreateVertexBuffer(_verts, sizeof(TVertexDefault), uniquevertsvec.size(), _vertexBuffer);
	delete[] _verts;
	//Indexes
	numindexes.push_back(indexcounter);
	_numPieces = (unsigned int)numindexes.size();
	_numIndicesPerPiece = new unsigned int[numindexes.size()];
	for (unsigned int i = 0; i < numindexes.size(); i++)
	{
		_numIndicesPerPiece[i] = numindexes[i];
	}
	unsigned int * _indices = new unsigned int[indicesvec.size()];
	for (unsigned int i = 0; i < indicesvec.size(); ++i)
	{
		_indices[i] = indicesvec[i];
	}
	CreateIndexBuffer(_indices, indicesvec.size(), _indexBuffer);
	delete[] _indices;
}
void CRenderer::CreateVertexBuffer(void* _VertexArray, unsigned int _sizeOfVertexStruct, unsigned int _numVerts, ID3D11Buffer** _vertexBuffer)
{
	D3D11_BUFFER_DESC vertbufferdesc;
	ZeroMemory(&vertbufferdesc, sizeof(D3D11_BUFFER_DESC));
	vertbufferdesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertbufferdesc.ByteWidth = _sizeOfVertexStruct * _numVerts;
	vertbufferdesc.StructureByteStride = _sizeOfVertexStruct;


	D3D11_SUBRESOURCE_DATA vertexdata;
	ZeroMemory(&vertexdata, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexdata.pSysMem = _VertexArray;


	HRESULT hr = m_pd3dDevice->CreateBuffer(&vertbufferdesc, &vertexdata, _vertexBuffer);
	if (hr != S_OK)
	{
		assert(0 && "VertexBuffer Issues");
	}
}
void CRenderer::CreateIndexBuffer(unsigned int* _IndexArray, unsigned int _numIndices, ID3D11Buffer** _indexBuffer)
{
	D3D11_BUFFER_DESC indexdesc;
	ZeroMemory(&indexdesc, sizeof(D3D11_BUFFER_DESC));
	indexdesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexdesc.ByteWidth = sizeof(unsigned int) * _numIndices;
	indexdesc.StructureByteStride = sizeof(unsigned int);

	D3D11_SUBRESOURCE_DATA indexdata;
	ZeroMemory(&indexdata, sizeof(D3D11_SUBRESOURCE_DATA));
	indexdata.pSysMem = _IndexArray;

	HRESULT hr = m_pd3dDevice->CreateBuffer(&indexdesc, &indexdata, _indexBuffer);
	if (hr != S_OK)
	{
		assert(0 && "IndexBuffer Issues");
	}
}
void CRenderer::ClearPostProcessSet()
{
	for (unsigned int i = 0; i < m_vpPostProcessSets.size(); ++i)
	{
		m_vpPostProcessSets[i]->GetRenderables().clear();
	}
}