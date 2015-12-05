#include "Shaders.h"
#include "Renderer.h"
//TODO Add Shader Includes
//Vertex Shader Includes
#include "VS3D.csh"
#include "VSPassThrough.csh"
#include "VS3DRevNorm.csh"
//Pixel Shader Includes
#include "PS3D.csh"
#include "PS3DNorm.csh"
#include "PSDebugLightMap.csh"
#include "PSDepth.csh"
#include "PS3DSpecLit.csh"
#include "PSRefraction.csh"
#include "PSColorBuffer.csh"
#include "PSWater.csh"
//Compute Shader Includes
#include "CSLightCull.csh"
#define ShaderHR(x){if(hr!=S_OK){assert(0 && "Load Shader error");}}
void CShaders::LoadShaders()
{
	//TODO 
	HRESULT hr;
	ID3D11Device* pDevice = CRenderer::GetInstance().GetDevice();
	ID3D11VertexShader* vsTemp = nullptr;
	ID3D11PixelShader* psTemp = nullptr;
	ID3D11ComputeShader* csTemp = nullptr;
	//Create Vertex Shaders
	hr = pDevice->CreateVertexShader(VS3D, sizeof(VS3D), nullptr, &vsTemp);
	ShaderHR(hr);
	m_mapVertexShaders["VS3D"] = vsTemp;
	hr = pDevice->CreateVertexShader(VSPassThrough, sizeof(VSPassThrough), nullptr, &vsTemp);
	ShaderHR(hr);
	m_mapVertexShaders["VSPassThrough"] = vsTemp;
	hr = pDevice->CreateVertexShader(VS3DRevNorm, sizeof(VS3DRevNorm), nullptr, &vsTemp);
	ShaderHR(hr);
	m_mapVertexShaders["VS3DRevNorm"] = vsTemp;
	//Create Pixel Shaders
	hr = pDevice->CreatePixelShader(PS3D, sizeof(PS3D), nullptr, &psTemp);
	ShaderHR(hr);
	m_mapPixelShaders["PS3D"] = psTemp;
	hr = pDevice->CreatePixelShader(PS3DNorm, sizeof(PS3DNorm), nullptr, &psTemp);
	ShaderHR(hr);
	m_mapPixelShaders["PS3DNorm"] = psTemp;
	hr = pDevice->CreatePixelShader(PSDebugLightMap, sizeof(PSDebugLightMap), nullptr, &psTemp);
	ShaderHR(hr);
	m_mapPixelShaders["PSDebugLightMap"] = psTemp;
	hr = pDevice->CreatePixelShader(PSDepth, sizeof(PSDepth), nullptr, &psTemp);
	ShaderHR(hr);
	m_mapPixelShaders["PSDepth"] = psTemp;
	hr = pDevice->CreatePixelShader(PS3DSpecLit, sizeof(PS3DSpecLit), nullptr, &psTemp);
	ShaderHR(hr);
	m_mapPixelShaders["PS3DSpecLit"] = psTemp;
	hr = pDevice->CreatePixelShader(PSRefraction, sizeof(PSRefraction), nullptr, &psTemp);
	ShaderHR(hr);
	m_mapPixelShaders["PSRefraction"] = psTemp;
	hr = pDevice->CreatePixelShader(PSColorBuffer, sizeof(PSColorBuffer), nullptr, &psTemp);
	ShaderHR(hr);
	m_mapPixelShaders["PSColorBuffer"] = psTemp;
	hr = pDevice->CreatePixelShader(PSWater, sizeof(PSWater), nullptr, &psTemp);
	ShaderHR(hr);
	m_mapPixelShaders["PSWater"] = psTemp;
	//Create Compute Shaders
	hr = pDevice->CreateComputeShader(CSLightCull, sizeof(CSLightCull), nullptr, &csTemp);
	ShaderHR(hr);
	m_mapComputeShaders["CSLightCull"] = csTemp;
}
void CShaders::CreateInputLayouts()
{
	HRESULT hr;
	ID3D11Device* pDevice = CRenderer::GetInstance().GetDevice();
	ID3D11InputLayout* pInputLayout = nullptr;
	//TODO 
	D3D11_INPUT_ELEMENT_DESC d3dLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = pDevice->CreateInputLayout(d3dLayout, 3, &VS3D, sizeof(VS3D), &pInputLayout);
	if (hr != S_OK)
	{
		assert(0 && "Input Layout error");
	}
	m_mapInputLayouts[m_mapVertexShaders["VS3D"]] = pInputLayout;
}
CShaders::CShaders()
{
	LoadShaders();
	CreateInputLayouts();
}
CShaders::~CShaders()
{
	auto compIter = m_mapComputeShaders.begin();
	while (compIter != m_mapComputeShaders.end())
	{
		SAFE_RELEASE(compIter->second);
		++compIter;
	}
	auto vertIter = m_mapVertexShaders.begin();
	while (vertIter != m_mapVertexShaders.end())
	{
		SAFE_RELEASE(vertIter->second);
		++vertIter;
	}
	auto pixelIter = m_mapPixelShaders.begin();
	while (pixelIter != m_mapPixelShaders.end())
	{
		SAFE_RELEASE(pixelIter->second);
		++pixelIter;
	}
	auto geoIter = m_mapGeometryShaders.begin();
	while (geoIter != m_mapGeometryShaders.end())
	{
		SAFE_RELEASE(geoIter->second);
		++geoIter;
	}
	auto hullIter = m_mapHullShaders.begin();
	while (hullIter != m_mapHullShaders.end())
	{
		SAFE_RELEASE(hullIter->second);
		++hullIter;
	}
	auto domainIter = m_mapDomainShaders.begin();
	while (domainIter != m_mapDomainShaders.end())
	{
		SAFE_RELEASE(domainIter->second);
		++domainIter;
	}
	auto inputIter = m_mapInputLayouts.begin();
	while (inputIter != m_mapInputLayouts.end())
	{
		SAFE_RELEASE(inputIter->second);
		++inputIter;
	}
}
ID3D11VertexShader* CShaders::GetVertexShader(std::string szShaderName)
{
	return m_mapVertexShaders[szShaderName];
}
ID3D11PixelShader* CShaders::GetPixelShader(std::string szShaderName)
{
	return m_mapPixelShaders[szShaderName];
}
ID3D11GeometryShader* CShaders::GetGeometryShader(std::string szShaderName)
{
	return m_mapGeometryShaders[szShaderName];
}
ID3D11HullShader* CShaders::GetHullShader(std::string szShaderName)
{
	return m_mapHullShaders[szShaderName];
}
ID3D11DomainShader* CShaders::GetDomainShader(std::string szShaderName)
{
	return m_mapDomainShaders[szShaderName];
}
ID3D11ComputeShader* CShaders::GetComputeShader(std::string szShaderName)
{
	return m_mapComputeShaders[szShaderName];
}
ID3D11InputLayout* CShaders::GetInputLayout(ID3D11VertexShader* vertShader)
{
	return m_mapInputLayouts[vertShader];
}