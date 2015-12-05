#pragma once
#include "Renderable.h"
#include <vector>
#include "VertexTypes.h"
class CRenderMesh : public CRenderable
{
protected:
	ID3D11Buffer* m_pVertexBuffer;
	std::vector<ID3D11ShaderResourceView*> m_vpSRVs;
	bool m_bNormalMapped = false;
	bool m_bIsActive = true;
	// Helper Functions //
	void BindVertexBuffer();
	void BindSRVsToPS();
public:
	CRenderMesh();
	virtual ~CRenderMesh();
	virtual void Process() override;

	ID3D11Buffer* GetVertexBuffer();
	void SetVertexBuffer(ID3D11Buffer* VertexBuffer);

	std::vector<ID3D11ShaderResourceView*>& GetShaderResourceViews();
	void SetActive(bool bActive);
	bool GetActive();
};