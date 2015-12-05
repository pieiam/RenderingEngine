#include "RenderContext.h"
#include "Renderer.h"
CRenderContext::CRenderContext(TShaders tShaders, TStates tStates,
	D3D11_PRIMITIVE_TOPOLOGY d3dPrimitiveTopology, ID3D11InputLayout* pd3dInputLayout)
{
	m_tShaders = tShaders;
	m_tStates = tStates;
	m_d3dPrimitiveTopology = d3dPrimitiveTopology;
	m_pd3dInputLayout = pd3dInputLayout;
}
CRenderContext::~CRenderContext()
{

}
void CRenderContext::Process()
{
	CRenderer& cRenderer = CRenderer::GetInstance();
	if (m_tShaders.m_pDomainShader)
	{
		cRenderer.GetContext()->DSSetShader(m_tShaders.m_pDomainShader, nullptr, 0);
	}
	if (m_tShaders.m_pGeometryShader)
	{
		cRenderer.GetContext()->GSSetShader(m_tShaders.m_pGeometryShader, nullptr, 0);
	}
	if (m_tShaders.m_pHullShader)
	{
		cRenderer.GetContext()->HSSetShader(m_tShaders.m_pHullShader, nullptr, 0);
	}
	if (m_tShaders.m_pPixelShader)
	{
		cRenderer.GetContext()->PSSetShader(m_tShaders.m_pPixelShader, nullptr, 0);
	}
	if (m_tShaders.m_pVertexShader)
	{
		cRenderer.GetContext()->VSSetShader(m_tShaders.m_pVertexShader, nullptr, 0);
	}
	cRenderer.GetContext()->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	cRenderer.GetContext()->IASetInputLayout(m_pd3dInputLayout);
	cRenderer.GetContext()->RSSetState(cRenderer.GetStates()->GetRasterizerState(m_tStates.eRasterizerState));
	cRenderer.GetContext()->OMSetBlendState(cRenderer.GetStates()->GetBlendState(m_tStates.eBlendState),NULL,0xffffffff);
	cRenderer.GetContext()->OMSetDepthStencilState(cRenderer.GetStates()->GetDepthStencilState(m_tStates.eDepthStencilState),m_tStates.unStencilRef);
}
void CRenderContext::UnProcess()
{
	CRenderer& cRenderer = CRenderer::GetInstance();
	if (m_tShaders.m_pDomainShader)
	{
		cRenderer.GetContext()->DSSetShader(nullptr, nullptr, 0);
	}
	if (m_tShaders.m_pGeometryShader)
	{
		cRenderer.GetContext()->GSSetShader(nullptr, nullptr, 0);
	}
	if (m_tShaders.m_pHullShader)
	{
		cRenderer.GetContext()->HSSetShader(nullptr, nullptr, 0);
	}
	if (m_tShaders.m_pPixelShader)
	{
		cRenderer.GetContext()->PSSetShader(nullptr, nullptr, 0);
	}
	if (m_tShaders.m_pVertexShader)
	{
		cRenderer.GetContext()->VSSetShader(nullptr, nullptr, 0);
	}
}
TShaders CRenderContext::GetShaders()
{
	return m_tShaders;
}
TStates CRenderContext::GetStates()
{
	return m_tStates;
}