#pragma once
#include "Renderable.h"
#include "RenderingStates.h"

struct TShaders
{
	ID3D11VertexShader* m_pVertexShader = nullptr;
	ID3D11PixelShader* m_pPixelShader = nullptr;
	ID3D11GeometryShader* m_pGeometryShader = nullptr;
	ID3D11HullShader* m_pHullShader = nullptr;
	ID3D11DomainShader* m_pDomainShader = nullptr;
};
struct TStates
{
	EDepthStencilState eDepthStencilState = eDepthStencilWriteAll;
	EBlendState eBlendState = eBlendStateInactive;
	ERasterizerState eRasterizerState = eRasterizerStateDefault;
	UINT unStencilRef = 1;
};
class CRenderContext : public CRenderable
{
private:
	ID3D11InputLayout* m_pd3dInputLayout;
	TShaders m_tShaders;
	TStates m_tStates;
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;
public:
	CRenderContext(TShaders tShaders, TStates tStates, D3D11_PRIMITIVE_TOPOLOGY d3dPrimitiveTopology, ID3D11InputLayout* pd3dInputLayout);
	~CRenderContext();
	void Process() override;
	void UnProcess();
	TShaders GetShaders();
	TStates GetStates();
};
