#pragma once
#include <DirectXMath.h>
using DirectX::XMFLOAT4;
class CRenderable;
#include <vector>
#define MAX_POINT_LIGHTS 1024
struct TPointLight
{
	// Position and Radius
	XMFLOAT4 f4PosRadius = {0,0,0,0};
	XMFLOAT4 f4Color = {1,1,1,1};
};
struct TPointLightStream
{
	//Just Position and Radius because Compute Shader doesn't need Color
	XMFLOAT4 f4PosRadius = { 0, 0, 0, 0 };
};
class CLights
{
private:
	std::vector<TPointLight> m_vtPointLights;
	std::vector<TPointLightStream> m_vtPointLightStream;
	std::vector<CRenderable*> m_vpLightRenderables;
public:
	CLights();
	~CLights();
	void Initialize();
	void UpdateLights();
	void RandomizeLights(XMFLOAT4 f4PosRadMin, XMFLOAT4 f4PosRadMax, XMFLOAT4 f4ColorMin, XMFLOAT4 f4ColorMax);
};