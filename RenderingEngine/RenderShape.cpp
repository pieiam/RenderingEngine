#include "RenderShape.h"
#include "Renderer.h"
#include <DirectXMath.h>
using namespace DirectX;
CRenderShape::CRenderShape(ERenderShape eShape)
{
	CRenderer& rend = CRenderer::GetInstance();
	if (eShape == eFullScreenQuad)
	{
		XMMATRIX matIdentity = XMMatrixIdentity();
		XMStoreFloat4x4(&m_tObject.f44WorldMatrix, matIdentity);

		unsigned int unIndex[6];
		unIndex[0] = 0;
		unIndex[1] = 1;
		unIndex[2] = 2;
		unIndex[3] = 2;
		unIndex[4] = 3;
		unIndex[5] = 0;
		rend.CreateIndexBuffer(unIndex, 6, &m_pIndexBuffer);

		for (unsigned int i = 0; i < 6; i++)
		{
			m_vIndices.push_back(unIndex[i]);
		}
		TVertexDefault tVerts[4];
		tVerts[0].position = { -1, 1, 0, 1 };
		tVerts[0].texCoords = { 0, 0, 0, 0 };
		tVerts[1].position = { 1, 1, 0, 1 };
		tVerts[1].texCoords = { 1, 0, 0, 0 };
		tVerts[2].position = { 1, -1, 0, 1 };
		tVerts[2].texCoords = { 1, 1, 0, 0 };
		tVerts[3].position = { -1, -1, 0, 1 };
		tVerts[3].texCoords = { 0, 1, 0, 0 };
		rend.CreateVertexBuffer(tVerts, sizeof(TVertexDefault), 4, &m_pVertexBuffer);
	}
}
CRenderShape::~CRenderShape()
{
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
}
void CRenderShape::Process()
{
	if (m_bIsActive == false)
		return;
	BindVertexBuffer();
	BindIndexBuffer();
	CRenderer& cRenderer = CRenderer::GetInstance();
	cRenderer.GetConstantBuffers()->MapToConstantBuffer(&m_tObject, sizeof(TObject), EConstantBuffer::eObjectBuffer);

	//Only Bind to Diffuse Buffer
	if (m_vpSRVs.size() > 0)
	{
		if (m_vpSRVs[0])
		{
			cRenderer.GetContext()->PSSetShaderResources(0, 1, &m_vpSRVs[0]);
		}
	}
	cRenderer.GetContext()->DrawIndexed(m_vIndices.size(), 0, 0);
}