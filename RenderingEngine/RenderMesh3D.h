#pragma once
#include "RenderMesh.h"
#include "ConstantBuffers.h"

class CRenderMesh3D : public CRenderMesh
{
protected:
	ID3D11Buffer* m_pIndexBuffer;
	std::vector<unsigned int> m_vIndices;
	TObject m_tObject;
	// Helper Functions //
	void BindIndexBuffer();
	void CreateIndexBuffer();
public:
	// Constructors // 
	CRenderMesh3D();
	CRenderMesh3D(std::vector<TVertexDefault> vtVerts, std::vector<unsigned int> vIndices);
	CRenderMesh3D(std::vector<TVertexUnlit> vtVerts, std::vector<unsigned int> vIndices);
	CRenderMesh3D(std::vector<TVertexTangent> vtVerts, std::vector<unsigned int> vIndices);

	// Destructor //
	virtual ~CRenderMesh3D();

	// Modify TObject Matrix
	void UniformScale(float _fScale);
	void SetPosition(float _fX, float _fY, float _fZ);
	void SetPosition(DirectX::XMFLOAT4 _Pos);
	void SetPosition(DirectX::XMFLOAT3 _Pos);
	// In Degrees
	void RotateGlobalX(float _fRotation);
	void RotateGlobalY(float _fRotation);
	void RotateGlobalZ(float _fRotation);

	// Process Function //
	virtual void Process() override;


	// Accessors and Mutators //
	ID3D11Buffer* GetIndexBuffer();
	void SetIndexBuffer(ID3D11Buffer* pIndexBuffer);

	TObject& GetObjectStruct(){ return m_tObject; }
	std::vector<unsigned int>& GetIndices();
};