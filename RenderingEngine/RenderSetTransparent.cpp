#include "RenderSetTransparent.h"
#include "RenderContext.h"
#include "Renderer.h"
#include "Shaders.h"
void CRenderSetTransparent::Process()
{
	m_pcRenderContextFrontCulled->Process();
	for (unsigned int i = 0; i < m_vpRenderables.size(); i++)
	{
		m_vpRenderables[i]->Process();
	}
	m_pcRenderContextFrontCulled->UnProcess();
	m_pcRenderContext->Process();
	for (unsigned int i = 0; i < m_vpRenderables.size(); i++)
	{
		m_vpRenderables[i]->Process();
	}
	m_pcRenderContext->UnProcess();
}
void CRenderSetTransparent::DepthProcess()
{
	m_pcRenderContextDepth->Process();
	for (unsigned int i = 0; i < m_vpRenderables.size(); ++i)
	{
		m_vpRenderables[i]->Process();
	}
	m_pcRenderContextDepth->UnProcess();
}
void CRenderSetTransparent::SetRenderContext(CRenderContext* pcRenderContext)
{
	//Main Context
	m_pcRenderContext = pcRenderContext;
	
	//Backface Context
	TShaders tShaders = m_pcRenderContext->GetShaders();
	TStates tStates = m_pcRenderContext->GetStates();
	tStates.eRasterizerState = ERasterizerState::eRasterizerStateFrontCulling;
	tShaders.m_pVertexShader = CRenderer::GetInstance().GetShaders()->GetVertexShader("VS3DRevNorm");
	m_pcRenderContextFrontCulled = new CRenderContext(tShaders, tStates,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, CRenderer::GetInstance().GetShaders()->GetInputLayout(m_pcRenderContext->GetShaders().m_pVertexShader));

	//Depth Context
	TShaders shaders = m_pcRenderContext->GetShaders();
	shaders.m_pPixelShader = CRenderer::GetInstance().GetShaders()->GetPixelShader("PSDepth");
	TStates states = pcRenderContext->GetStates();
	states.eBlendState = EBlendState::eBlendStateNoColor;
	states.eRasterizerState = ERasterizerState::eRasterizerStateNoCulling;
	m_pcRenderContextDepth = new CRenderContext(shaders, states, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, CRenderer::GetInstance().GetShaders()->GetInputLayout(m_pcRenderContext->GetShaders().m_pVertexShader));
}
CRenderSetTransparent::CRenderSetTransparent()
{

}
CRenderSetTransparent::~CRenderSetTransparent()
{
	SAFE_DELETE(m_pcRenderContextFrontCulled);
}
