#include "ModelManager.h"
#include "Renderer.h"
CModelManager::CModelManager()
{

}
CModelManager::~CModelManager()
{
	auto iter = m_mpBuffers.begin();
	while (iter != m_mpBuffers.end())
	{
		SAFE_RELEASE(iter->second.pVertexBuffer);
		SAFE_RELEASE(iter->second.pIndexBuffer);
		delete[] iter->second.pNumIndexPerPiece;
		++iter;
	}
	auto iterMesh = m_mpMeshes.begin();
	while (iterMesh != m_mpMeshes.end())
	{
		delete iterMesh->second;
		++iterMesh;
	}
}
void CModelManager::LoadObj(std::string szFilePath)
{
	TBuffers tBuffer;
	TMesh* tMesh = new TMesh;
	CRenderer::GetInstance().LoadObjFile(&tBuffer.pIndexBuffer, &tBuffer.pVertexBuffer, szFilePath, tBuffer.pNumIndexPerPiece, tBuffer.numPieces, (void**)&tMesh);
	m_mpBuffers[szFilePath] = tBuffer;
	m_mpMeshes[szFilePath] = tMesh;
}

TBuffers CModelManager::GetModel(std::string szFilePath)
{
	if (m_mpBuffers.find(szFilePath) == m_mpBuffers.end())
	{
		LoadObj(szFilePath);
	}

	return m_mpBuffers[szFilePath];
}
const CModelManager::TMesh* CModelManager::GetMesh(std::string szFilePath)
{
	if (m_mpMeshes.find(szFilePath) == m_mpMeshes.end())
	{
		LoadObj(szFilePath);
	}
	return m_mpMeshes[szFilePath];
}