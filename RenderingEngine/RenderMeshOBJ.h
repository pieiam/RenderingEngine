#pragma once
#include "RenderMesh3D.h"
class CRenderMeshOBJ : public CRenderMesh3D
{
private:
	unsigned int m_unNumPieces = 0;
	unsigned int* m_pOBJNumIndices;
public:
	CRenderMeshOBJ(std::string szOBJFilepath,bool bNormalMapped = false);
	~CRenderMeshOBJ();
	void Process() override;
};