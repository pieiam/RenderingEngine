#include "RenderingStates.h"
#include <assert.h>
#include "Renderer.h"

void CRenderingStates::DepthStencilInitialize(CRenderer* pcRenderer)
{
	m_vpDepthStencilStates.resize(eDepthStateTotal);
	HRESULT hr;
	CD3D11_DEFAULT d3dDefaultValues;
	CD3D11_DEPTH_STENCIL_DESC d3dDepthStencilDesc(d3dDefaultValues);
	ID3D11DepthStencilState* pd3dDepthStencilState = nullptr;
	d3dDepthStencilDesc.StencilEnable = FALSE;

	hr = pcRenderer->GetDevice()->CreateDepthStencilState(&d3dDepthStencilDesc, &pd3dDepthStencilState);
	m_vpDepthStencilStates[EDepthStencilState::eDepthStencilWriteAll] = pd3dDepthStencilState;
	if (hr != S_OK)
	{
		assert(0 && "CRendering State DepthStencil Issues");
	}
	d3dDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = pcRenderer->GetDevice()->CreateDepthStencilState(&d3dDepthStencilDesc, &pd3dDepthStencilState);
	m_vpDepthStencilStates[EDepthStencilState::eDepthStencilNoWrite] = pd3dDepthStencilState;
	if (hr != S_OK)
	{
		assert(0 &&"CRendering State DepthStencil Issues");
	}
	d3dDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D11_COMPARISON_EQUAL;
	hr = pcRenderer->GetDevice()->CreateDepthStencilState(&d3dDepthStencilDesc, &pd3dDepthStencilState);
	m_vpDepthStencilStates[EDepthStencilState::eDepthStencilWriteEqual] = pd3dDepthStencilState;

	d3dDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = pcRenderer->GetDevice()->CreateDepthStencilState(&d3dDepthStencilDesc, &pd3dDepthStencilState);
	m_vpDepthStencilStates[EDepthStencilState::eDepthStencilNoWriteEqual] = pd3dDepthStencilState;

	// Make Depth Stencil States with Stencil enabled
	D3D11_DEPTH_STENCIL_DESC d3dStencilDesc;
	ZeroMemory(&d3dStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	d3dStencilDesc.DepthEnable = true;
	d3dStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	d3dStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	d3dStencilDesc.StencilEnable = TRUE;
	d3dStencilDesc.StencilReadMask = 255;
	d3dStencilDesc.StencilWriteMask = 255;
	d3dStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	d3dStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	d3dStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	d3dStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	d3dStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	hr = pcRenderer->GetDevice()->CreateDepthStencilState(&d3dStencilDesc, &pd3dDepthStencilState);
	m_vpDepthStencilStates[EDepthStencilState::eDepthStencilBackIncr] = pd3dDepthStencilState;

	d3dStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	d3dStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	hr = pcRenderer->GetDevice()->CreateDepthStencilState(&d3dStencilDesc, &pd3dDepthStencilState);
	m_vpDepthStencilStates[EDepthStencilState::eDepthStencilEqual] = pd3dDepthStencilState;

	d3dStencilDesc.DepthEnable = false;
	d3dStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	hr = pcRenderer->GetDevice()->CreateDepthStencilState(&d3dStencilDesc, &pd3dDepthStencilState);
	m_vpDepthStencilStates[EDepthStencilState::eStencilEqualIncr] = pd3dDepthStencilState;

}
void CRenderingStates::RasterizerInitialize(CRenderer* pcRenderer)
{
	m_vpRasterizerStates.resize(eRasterizerStateTotal);
	HRESULT hr;
	CD3D11_DEFAULT d3dDefaultValues;
	CD3D11_RASTERIZER_DESC d3dRasterizerStateDesc(d3dDefaultValues);
	ID3D11RasterizerState* pd3dRasterizerState = nullptr;
	hr = pcRenderer->GetDevice()->CreateRasterizerState(&d3dRasterizerStateDesc, &pd3dRasterizerState);
	m_vpRasterizerStates[ERasterizerState::eRasterizerStateDefault] = pd3dRasterizerState;
	if (hr != S_OK)
	{
		assert(0 &&"CRendering State Rasterizer Issues");
	}
	d3dRasterizerStateDesc.CullMode = D3D11_CULL_NONE;
	hr = pcRenderer->GetDevice()->CreateRasterizerState(&d3dRasterizerStateDesc, &pd3dRasterizerState);
	m_vpRasterizerStates[ERasterizerState::eRasterizerStateNoCulling] = pd3dRasterizerState;
	if (hr != S_OK)
	{
		assert(0 &&"CRendering State Rasterizer Issues");
	}
	d3dRasterizerStateDesc.CullMode = D3D11_CULL_FRONT;
	hr = pcRenderer->GetDevice()->CreateRasterizerState(&d3dRasterizerStateDesc, &pd3dRasterizerState);
	m_vpRasterizerStates[ERasterizerState::eRasterizerStateFrontCulling] = pd3dRasterizerState;
	if (hr != S_OK)
	{
		assert(0 &&"CRendering State Rasterizer Issues");
	}
	d3dRasterizerStateDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerStateDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = pcRenderer->GetDevice()->CreateRasterizerState(&d3dRasterizerStateDesc, &pd3dRasterizerState);
	m_vpRasterizerStates[ERasterizerState::eRasterizerStateWireframe] = pd3dRasterizerState;
	if (hr != S_OK)
	{
		assert(0 && "CRendering State Rasterizer Issues");
	}
}
void CRenderingStates::BlendInitialize(CRenderer* pcRenderer)
{
	m_vpBlendStates.resize(eBlendStateTotal);
	HRESULT hr;
	CD3D11_DEFAULT d3dDefaultValues;
	CD3D11_BLEND_DESC d3dBlendDesc(d3dDefaultValues);
	ID3D11BlendState* pd3dBlendState = nullptr;
	hr = pcRenderer->GetDevice()->CreateBlendState(&d3dBlendDesc, &pd3dBlendState);
	m_vpBlendStates[EBlendState::eBlendStateInactive] = pd3dBlendState;
	if (hr != S_OK)
	{
		assert(0 && "CRendering State Blend Issues");
	}
	for (unsigned int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		d3dBlendDesc.RenderTarget[i].BlendEnable = TRUE;
		d3dBlendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		d3dBlendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		d3dBlendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		d3dBlendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		d3dBlendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		d3dBlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		d3dBlendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	hr = pcRenderer->GetDevice()->CreateBlendState(&d3dBlendDesc, &pd3dBlendState);
	m_vpBlendStates[EBlendState::eBlendStateActive] = pd3dBlendState;
	if (hr != S_OK)
	{
		assert(0 && "CRendering State Blend Issues");
	}
	for (unsigned int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		d3dBlendDesc.RenderTarget[i].BlendEnable = FALSE;
		d3dBlendDesc.RenderTarget[i].RenderTargetWriteMask = 0;
	}
	hr = pcRenderer->GetDevice()->CreateBlendState(&d3dBlendDesc, &pd3dBlendState);
	m_vpBlendStates[EBlendState::eBlendStateNoColor] = pd3dBlendState;
	if (hr != S_OK)
	{
		assert(0 && "CRendering State Blend Issues");
	}
}

CRenderingStates::CRenderingStates()
{
	CRenderer* pcRenderer = &CRenderer::GetInstance();
	DepthStencilInitialize(pcRenderer);
	RasterizerInitialize(pcRenderer);
	BlendInitialize(pcRenderer);
}
CRenderingStates::~CRenderingStates()
{
	for (unsigned int i = 0; i < m_vpBlendStates.size(); i++)
	{
		SAFE_RELEASE(m_vpBlendStates[i]);
	}
	for (unsigned int i = 0; i < m_vpDepthStencilStates.size(); i++)
	{
		SAFE_RELEASE(m_vpDepthStencilStates[i]);
	}
	for (unsigned int i = 0; i < m_vpRasterizerStates.size(); i++)
	{
		SAFE_RELEASE(m_vpRasterizerStates[i]);
	}
}
ID3D11BlendState* CRenderingStates::GetBlendState(EBlendState eState)
{
	return m_vpBlendStates[eState];
}
ID3D11RasterizerState* CRenderingStates::GetRasterizerState(ERasterizerState eState)
{
	return m_vpRasterizerStates[eState];
}
ID3D11DepthStencilState* CRenderingStates::GetDepthStencilState(EDepthStencilState eState)
{
	return m_vpDepthStencilStates[eState];
}