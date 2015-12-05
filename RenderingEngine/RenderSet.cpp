#include "RenderSet.h"
#include "RenderContext.h"
#include "Renderer.h"
#include "Shaders.h"
CRenderSet::CRenderSet()
{

}
CRenderSet::~CRenderSet()
{
	SAFE_DELETE(m_pcRenderContext);
	for (unsigned int i = 0; i < m_vpRenderables.size(); i++)
	{
		SAFE_DELETE(m_vpRenderables[i]);
	}
	SAFE_DELETE(m_pcRenderContextDepth);
}
void CRenderSet::Process()
{
	m_pcRenderContext->Process();
	for (unsigned int i = 0; i < m_vpRenderables.size(); i++)
	{
		m_vpRenderables[i]->Process();
	}
	m_pcRenderContext->UnProcess();
}
void CRenderSet::DepthProcess()
{
	m_pcRenderContextDepth->Process();
	for (unsigned int i = 0; i < m_vpRenderables.size(); i++)
	{
		m_vpRenderables[i]->Process();
	}
	m_pcRenderContextDepth->UnProcess();
}
void CRenderSet::SetRenderContext(CRenderContext* pcRenderContext)
{
	m_pcRenderContext = pcRenderContext;
	TShaders shaders = m_pcRenderContext->GetShaders();
	shaders.m_pPixelShader = CRenderer::GetInstance().GetShaders()->GetPixelShader("PSDepth");
	TStates states = pcRenderContext->GetStates();
	states.eDepthStencilState = EDepthStencilState::eDepthStencilWriteAll;
	states.eBlendState = EBlendState::eBlendStateNoColor;
	m_pcRenderContextDepth = new CRenderContext(shaders, states, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, CRenderer::GetInstance().GetShaders()->GetInputLayout(m_pcRenderContext->GetShaders().m_pVertexShader));
}
CRenderContext* CRenderSet::GetRenderContext()
{
	return m_pcRenderContext;
}
std::vector<CRenderable*>& CRenderSet::GetRenderables()
{
	return m_vpRenderables;
}