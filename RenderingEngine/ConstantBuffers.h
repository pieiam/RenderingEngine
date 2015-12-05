#pragma once
#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>
using DirectX::XMFLOAT4X4;
using DirectX::XMFLOAT4;
//Class will initialize and store constant buffers
enum EConstantBuffer {eObjectBuffer, eSceneBuffer,ePointLightBuffer,ePointLightStreamBuffer, eTotalConstantBuffer};
class CRenderer;
class CConstantBuffers
{
private:
	std::vector<ID3D11Buffer*> m_vpConstantBuffers;
public:
	CConstantBuffers();
	~CConstantBuffers();
	void BindConstantBuffers();
	void MapToConstantBuffer(void* pData,unsigned int unSizeOfData,  EConstantBuffer eBuffer);
};
struct TObject
{
	XMFLOAT4X4 f44WorldMatrix;
};
struct TScene
{
	XMFLOAT4X4 f44ViewMatrix;
	XMFLOAT4X4 f44ProjectionMatrix;
	XMFLOAT4X4 f44InvProj;
	XMFLOAT4 f4CameraPosition;
};