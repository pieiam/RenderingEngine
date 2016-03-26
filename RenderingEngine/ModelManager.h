#pragma once
#include <unordered_map>
#include <string>
#include <d3d11.h>
#include "VertexTypes.h"
struct TBuffers
{
	ID3D11Buffer* pVertexBuffer = nullptr;
	ID3D11Buffer* pIndexBuffer = nullptr;
	unsigned int* pNumIndexPerPiece;
	unsigned int numPieces;
};
class CModelManager
{
	// Public Structs in this namespace
public:
	struct TTriangle
	{
		unsigned int nIndices[3];
	};
	struct TMesh
	{
		std::vector<TVertexDefault> vtVerts;
		std::vector<unsigned int> vnIndices;
		std::vector<TTriangle> vtTriangles;
		std::string szFilepath;
	};

	// Private Data members and functions
private:
	std::unordered_map<std::string, TBuffers> m_mpBuffers;
	std::unordered_map<std::string, TMesh*> m_mpMeshes;
	void LoadObj(std::string szFilePath);
	//Public Methods
public:
	CModelManager();
	~CModelManager();
	TBuffers GetModel(std::string szFilePath);
	const TMesh* GetMesh(std::string szFilePath);
};