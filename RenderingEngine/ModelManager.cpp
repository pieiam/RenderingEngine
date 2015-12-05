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
}
TBuffers CModelManager::GetModel(std::string szFilePath)
{
	if (m_mpBuffers.find(szFilePath) == m_mpBuffers.end())
	{
		TBuffers tBuffer;
		CRenderer::GetInstance().LoadObjFile(&tBuffer.pIndexBuffer, &tBuffer.pVertexBuffer, szFilePath, tBuffer.pNumIndexPerPiece, tBuffer.numPieces);
		m_mpBuffers[szFilePath] = tBuffer;
	}

	return m_mpBuffers[szFilePath];
}