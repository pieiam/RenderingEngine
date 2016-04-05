#ifndef RENDERER_H
#define RENDERER_H

#include <d3d11.h>
#include <D3D11_1.h>
#pragma comment(lib,"d3d11.lib")
#include <DirectXMath.h>
#include <vector>
#include <unordered_map>
#include <string>
#include "ConstantBuffers.h"
#define SAFE_RELEASE(p){if(p){p->Release(); p = nullptr;}}
#define SAFE_DELETE(p){if(p){delete p; p = nullptr;}}

enum EContext{ eContext3D, eContext3DFrontCulled, eContext3DNorm, eContext3DFrontCulledNorm, eContextWireframe , eContextTotal };
enum EContextTransparent{ eContext3DTransparent, eContextWaterVolume, eContextTransparentTotal };
enum EContextPostProcess{ eContextColorBuffer, eContextLightMap,eContextWater,eContextPostProcessTotal };


class CRenderingStates;
class CRenderable;
class CRenderSet;
class CRenderSetTransparent;
class CShaders;
class CCamera;
class CTextureManager;
class CLights;
class CModelManager;
class CRenderShape;
struct TRenderToTexture
{
	ID3D11Texture2D* pTexture2D = nullptr;
	ID3D11ShaderResourceView* pSRV = nullptr;
	ID3D11RenderTargetView* pRTV = nullptr;
};
class CRenderer
{
private:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pd3dContext = nullptr;
	IDXGISwapChain* m_pd3dSwapChain = nullptr;

	ID3D11RenderTargetView* m_pd3dRenderTargetView = nullptr;
	ID3D11Texture2D* m_pd3dBackBuffer = nullptr;
	
	ID3D11Texture2D* m_pd3dDepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_pd3dDepthStencilView = nullptr;
	ID3D11DepthStencilView* m_pd3dTransparencyDepthStencilView = nullptr;
	ID3D11ShaderResourceView* m_pd3dDepthStencilSRV = nullptr;
	ID3D11ShaderResourceView* m_pd3dTransparencyDepthSRV = nullptr;

	//Render to Texture For Refraction
	TRenderToTexture m_tRTTColorBuffer;

	CRenderShape* m_pColorBuffer = nullptr;
	CRenderShape* m_pLightMap = nullptr;

	ID3D11SamplerState* m_pd3dSampleState = nullptr;

	D3D11_VIEWPORT m_d3dViewport;

	UINT m_nResolutionWidth = 0;
	UINT m_nResolutionHeight = 0;

	CRenderingStates* m_pcRenderingStates = nullptr;
	CConstantBuffers* m_pcConstantBuffers = nullptr;
	CShaders* m_pcShaders = nullptr;
	CTextureManager* m_pcTextureManager = nullptr;
	CLights* m_pcLights = nullptr;
	CModelManager* m_pcModelManager = nullptr;

	CCamera* m_pcCamera = nullptr;
	TScene m_tScene;

	//LightCulling Stuff
	ID3D11Buffer* m_pd3dLightBuffer = nullptr;
	ID3D11ShaderResourceView* m_pd3dLightSRV = nullptr;
	ID3D11UnorderedAccessView* m_pd3dLightUAV = nullptr;
	//Render Sets
	std::vector<CRenderSet*> m_vpRenderSets;
	std::vector<CRenderSet*> m_vpRenderNoDepthSets;
	std::vector<CRenderSet*> m_vpTransparentRenderSets;
	std::vector<CRenderSet*> m_vpPostProcessSets;

	//Singleton
	CRenderer() = default;
	CRenderer(CRenderer const&) = delete;
	void operator=(CRenderer const&) = delete;

	// Helper Functions //
	void CreateRenderSets();
	ID3DUserDefinedAnnotation* m_pUserDefinedAnnotation = nullptr;


public:
	bool Initialize(HWND hWnd, UINT width, UINT height, float fZNear, float fZFar);
	void Shutdown();
	//Singleton
	static CRenderer& GetInstance();

	//  Accessors //
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
	IDXGISwapChain* GetSwapChain();

	ID3D11RenderTargetView* GetRenderTargetView();
	ID3D11Texture2D* GetBackBuffer();

	ID3D11Texture2D* GetDepthStencilBuffer();
	ID3D11DepthStencilView* GetDepthStencilView();
	ID3D11ShaderResourceView* GetDepthStencilShaderResourceView();
	TRenderToTexture GetRTTColorBuffer();

	CRenderShape* GetColorBuffer();
	CRenderShape* GetLightMap();

	D3D11_VIEWPORT GetViewport();

	CRenderingStates* GetStates();
	CConstantBuffers* GetConstantBuffers();
	CShaders* GetShaders();
	CTextureManager* GetTextureManager();
	CModelManager* GetModelManager();

	void AddRenderable(EContext eContext, CRenderable* pcRenderable,bool bDepthPrepass = true);
	void AddRenderable(EContextTransparent eContext, CRenderable* pcRenderable);
	void AddRenderable(EContextPostProcess eContext, CRenderable* pcRenderable);
	void Render();
	void LoadObjFile(ID3D11Buffer** _indexBuffer, ID3D11Buffer** _vertexBuffer, std::string _filename, unsigned int *& _numIndicesPerPiece, unsigned int &_numPieces, void** _TMesh);

	void CreateVertexBuffer(void* _VertexArray, unsigned int _sizeOfVertexStruct, unsigned int _numVerts, ID3D11Buffer** _vertexBuffer);
	void CreateIndexBuffer(unsigned int* _IndexArray, unsigned int _numIndices, ID3D11Buffer** _indexBuffer);

	void ClearPostProcessSet();
};

#endif