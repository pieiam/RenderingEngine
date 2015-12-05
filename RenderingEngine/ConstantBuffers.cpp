#include "ConstantBuffers.h"
#include "Renderer.h"
#include "Lights.h"
CConstantBuffers::CConstantBuffers()
{
	HRESULT hr;
	CRenderer& cRenderer = CRenderer::GetInstance();
	m_vpConstantBuffers.resize(eTotalConstantBuffer);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.ByteWidth = sizeof(TObject);
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.MiscFlags = 0;
	d3dBufferDesc.StructureByteStride = 0;
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	ID3D11Buffer* pd3dConstantBuffer;
	hr = cRenderer.GetDevice()->CreateBuffer(&d3dBufferDesc, NULL, &pd3dConstantBuffer);
	if (hr != S_OK)
	{
		assert(0 &&"Constant Buffer Constructor Issues");
	}
	m_vpConstantBuffers[eObjectBuffer] = pd3dConstantBuffer;
	
	ID3D11Buffer* pd3dConstantBuffer2;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.MiscFlags = 0;
	d3dBufferDesc.StructureByteStride = 0;
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(TScene);
	hr = cRenderer.GetDevice()->CreateBuffer(&d3dBufferDesc, NULL, &pd3dConstantBuffer2);
	if (hr != S_OK)
	{
		assert(0 &&"Constant Buffer Constructor Issues");
	}
	m_vpConstantBuffers[eSceneBuffer] = pd3dConstantBuffer2;

	ID3D11Buffer* pd3dConstantBuffer3;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.MiscFlags = 0;
	d3dBufferDesc.StructureByteStride = 0;
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(TPointLight) * MAX_POINT_LIGHTS;
	hr = cRenderer.GetDevice()->CreateBuffer(&d3dBufferDesc, NULL, &pd3dConstantBuffer3);
	if (hr != S_OK)
	{
		assert(0 && "Constant Buffer Constructor Issues");
	}
	m_vpConstantBuffers[ePointLightBuffer] = pd3dConstantBuffer3;

	ID3D11Buffer* pd3dConstantBuffer4;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.MiscFlags = 0;
	d3dBufferDesc.StructureByteStride = 0;
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(TPointLightStream) * MAX_POINT_LIGHTS;
	hr = cRenderer.GetDevice()->CreateBuffer(&d3dBufferDesc, NULL, &pd3dConstantBuffer4);
	if (hr != S_OK)
	{
		assert(0 && "Constant Buffer Constructor Issues");
	}
	m_vpConstantBuffers[ePointLightStreamBuffer] = pd3dConstantBuffer4;

	BindConstantBuffers();
}
CConstantBuffers::~CConstantBuffers()
{
	for (unsigned int i = 0; i < m_vpConstantBuffers.size(); i++)
	{
		SAFE_RELEASE(m_vpConstantBuffers[i]);
	}
}
void CConstantBuffers::BindConstantBuffers()
{
	CRenderer& cRenderer = CRenderer::GetInstance();
	cRenderer.GetContext()->VSSetConstantBuffers(eObjectBuffer, 1, &m_vpConstantBuffers[eObjectBuffer]);
	cRenderer.GetContext()->VSSetConstantBuffers(eSceneBuffer, 1, &m_vpConstantBuffers[eSceneBuffer]);
	cRenderer.GetContext()->PSSetConstantBuffers(ePointLightBuffer, 1, &m_vpConstantBuffers[ePointLightBuffer]);
	cRenderer.GetContext()->PSSetConstantBuffers(eSceneBuffer, 1, &m_vpConstantBuffers[eSceneBuffer]);
	cRenderer.GetContext()->CSSetConstantBuffers(eSceneBuffer, 1, &m_vpConstantBuffers[eSceneBuffer]);
	cRenderer.GetContext()->CSSetConstantBuffers(ePointLightStreamBuffer, 1, &m_vpConstantBuffers[ePointLightStreamBuffer]);

	//TODO Add new Constant buffers as they are declared
}
void CConstantBuffers::MapToConstantBuffer(void* pData,unsigned int unSizeOfData, EConstantBuffer eBuffer)
{
	if (pData != nullptr)
	{
		CRenderer& cRenderer = CRenderer::GetInstance();
		D3D11_MAPPED_SUBRESOURCE d3dSubresource;
		ID3D11Buffer* pMappedBuffer = m_vpConstantBuffers[eBuffer];
		ZeroMemory(&d3dSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		cRenderer.GetContext()->Map(pMappedBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &d3dSubresource);
		memcpy(d3dSubresource.pData, pData, unSizeOfData);
		cRenderer.GetContext()->Unmap(pMappedBuffer, NULL);
	}
}