#include "RenderMeshOBJ.h"
#include "Renderer.h"
#include <DirectXMath.h>
#include "ModelManager.h"
using namespace DirectX;
CRenderMeshOBJ::CRenderMeshOBJ(std::string szOBJFilepath, bool bNormalMapped)
{
	m_bNormalMapped = bNormalMapped;
	XMMATRIX matIdentity = XMMatrixIdentity();
	XMStoreFloat4x4(&m_tObject.f44WorldMatrix, matIdentity);
	TBuffers tModelBuffers = CRenderer::GetInstance().GetModelManager()->GetModel(szOBJFilepath);
	m_pIndexBuffer = tModelBuffers.pIndexBuffer;
	m_pVertexBuffer = tModelBuffers.pVertexBuffer;
	m_pOBJNumIndices = tModelBuffers.pNumIndexPerPiece;
	m_unNumPieces = tModelBuffers.numPieces;
	//.LoadObjFile(&m_pIndexBuffer, &m_pVertexBuffer, szOBJFilepath, m_pOBJNumIndices, m_unNumPieces);
}
CRenderMeshOBJ::~CRenderMeshOBJ()
{
//	delete[] m_pOBJNumIndices;
}
void CRenderMeshOBJ::Process()
{
	if (m_bIsActive == false)
		return;
	BindVertexBuffer();
	BindIndexBuffer();
	CRenderer& cRenderer = CRenderer::GetInstance();
	cRenderer.GetConstantBuffers()->MapToConstantBuffer(&m_tObject, sizeof(TObject), EConstantBuffer::eObjectBuffer);
	for (unsigned int i = 0; i < m_unNumPieces; i++)
	{
		//Only Bind to Diffuse Buffer
		if (m_vpSRVs.size() > i)
		{
			if (m_vpSRVs[i])
			{
				cRenderer.GetContext()->PSSetShaderResources(0, 1, &m_vpSRVs[i]);
			}
			if (m_bNormalMapped)
			{
				cRenderer.GetContext()->PSSetShaderResources(1, 1, &m_vpSRVs[i+m_unNumPieces]);
			}
		}
		unsigned int offset = 0;
		for (int j = i - 1; j >= 0; j--)
		{
			offset += m_pOBJNumIndices[j];
		}
		cRenderer.GetContext()->DrawIndexed(m_pOBJNumIndices[i], offset,0);
	}
}