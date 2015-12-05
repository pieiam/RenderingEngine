#include "RenderMesh.h"
#include "Renderer.h"
void CRenderMesh::BindVertexBuffer()
{
	const UINT unStride = sizeof(TVertexDefault);
	const UINT unOffsets = 0;
	CRenderer::GetInstance().GetContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &unStride, &unOffsets);

}
void CRenderMesh::BindSRVsToPS()
{
	for (unsigned int i = 0; i < m_vpSRVs.size(); i++)
	{
		CRenderer::GetInstance().GetContext()->PSSetShaderResources(i, 1, &m_vpSRVs[i]);
	}
}
CRenderMesh::CRenderMesh()
{
	//Does Nothing
}
CRenderMesh::~CRenderMesh()
{
//	SAFE_RELEASE(m_pVertexBuffer);
}
void CRenderMesh::Process()
{
	//Does Nothing
}
ID3D11Buffer* CRenderMesh::GetVertexBuffer()
{
	return m_pVertexBuffer;
}
void CRenderMesh::SetVertexBuffer(ID3D11Buffer* VertexBuffer)
{
	m_pVertexBuffer = VertexBuffer;
}
std::vector<ID3D11ShaderResourceView*>& CRenderMesh::GetShaderResourceViews()
{
	return m_vpSRVs;
}
void CRenderMesh::SetActive(bool bActive)
{
	m_bIsActive = bActive;
}
bool CRenderMesh::GetActive()
{
	return m_bIsActive;
}