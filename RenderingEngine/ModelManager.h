#pragma once
#include <unordered_map>
#include <string>
#include <d3d11.h>
struct TBuffers
{
	ID3D11Buffer* pVertexBuffer = nullptr;
	ID3D11Buffer* pIndexBuffer = nullptr;
	unsigned int* pNumIndexPerPiece;
	unsigned int numPieces;
};
class CModelManager
{
private:
	std::unordered_map<std::string, TBuffers> m_mpBuffers;
public:
	CModelManager();
	~CModelManager();
	TBuffers GetModel(std::string szFilePath);
};