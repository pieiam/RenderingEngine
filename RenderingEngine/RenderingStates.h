#pragma once
#include <d3d11.h>
#include <vector>

enum EBlendState {eBlendStateActive, eBlendStateInactive,eBlendStateNoColor , eBlendStateTotal};
enum ERasterizerState { eRasterizerStateDefault, eRasterizerStateWireframe, eRasterizerStateNoCulling, eRasterizerStateFrontCulling, eRasterizerStateTotal };
enum EDepthStencilState {eDepthStencilWriteAll,eDepthStencilNoWrite, eDepthStencilWriteEqual,eDepthStencilNoWriteEqual
	,eDepthStencilBackIncr,eDepthStencilEqual,eStencilEqualIncr,eDepthStateTotal};

class CRenderer;
class CRenderingStates
{
private:
	std::vector<ID3D11BlendState*> m_vpBlendStates;
	std::vector<ID3D11RasterizerState*> m_vpRasterizerStates;
	std::vector<ID3D11DepthStencilState*> m_vpDepthStencilStates;

	void DepthStencilInitialize(CRenderer* pcRenderer);
	void RasterizerInitialize(CRenderer* pcRenderer);
	void BlendInitialize(CRenderer* pcRenderer);

public:
	CRenderingStates();
	~CRenderingStates();
	ID3D11BlendState* GetBlendState(EBlendState eState);
	ID3D11RasterizerState* GetRasterizerState(ERasterizerState eState);
	ID3D11DepthStencilState* GetDepthStencilState(EDepthStencilState eState);
};