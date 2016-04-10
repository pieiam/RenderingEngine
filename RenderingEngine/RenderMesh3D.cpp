#include "RenderMesh3D.h"
#include "Renderer.h"
#define RADIANS(degrees) ((3.14f / 180.0f) * degrees)
using namespace DirectX;
void CRenderMesh3D::BindIndexBuffer()
{
	const UINT unOffsets = 0;
	CRenderer::GetInstance().GetContext()->IASetIndexBuffer(m_pIndexBuffer,DXGI_FORMAT_R32_UINT,unOffsets);
}
void CRenderMesh3D::CreateIndexBuffer()
{
	HRESULT hr;

	D3D11_BUFFER_DESC d3dBufferDesc;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));

	d3dBufferData.pSysMem = &m_vIndices[0];

	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.ByteWidth = sizeof(unsigned int);
	d3dBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

	hr = CRenderer::GetInstance().GetDevice()->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pIndexBuffer);
	if (hr != S_OK)
	{
		assert(0 && "RenderMesh3D CTor Issues");
	}
}
CRenderMesh3D::CRenderMesh3D()
{
	XMMATRIX matIdentity = XMMatrixIdentity();
	XMStoreFloat4x4(&m_tObject.f44WorldMatrix, matIdentity);
}
CRenderMesh3D::CRenderMesh3D(std::vector<TVertexDefault> vtVerts, std::vector<unsigned int> vIndices)
{
	XMMATRIX matIdentity = XMMatrixIdentity();
	XMStoreFloat4x4(&m_tObject.f44WorldMatrix, matIdentity);
	m_vIndices = vIndices;	
	CreateIndexBuffer();

	// Vertex Buffer Creation //
	HRESULT hr;

	D3D11_BUFFER_DESC d3dBufferDesc;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));

	d3dBufferData.pSysMem = &vtVerts[0];

	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.ByteWidth = sizeof(TVertexDefault);
	d3dBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

	hr = CRenderer::GetInstance().GetDevice()->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pVertexBuffer);
	if (hr != S_OK)
	{
		assert(0 && "CRenderMesh3D CTor Issues");
	}
}
CRenderMesh3D::CRenderMesh3D(std::vector<TVertexUnlit> vtVerts, std::vector<unsigned int> vIndices)
{
	XMMATRIX matIdentity = XMMatrixIdentity();
	XMStoreFloat4x4(&m_tObject.f44WorldMatrix, matIdentity);
	m_vIndices = vIndices;
	CreateIndexBuffer();

	// Vertex Buffer Creation //
	HRESULT hr;

	D3D11_BUFFER_DESC d3dBufferDesc;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));

	d3dBufferData.pSysMem = &vtVerts[0];

	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.ByteWidth = sizeof(TVertexUnlit);
	d3dBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

	hr = CRenderer::GetInstance().GetDevice()->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pVertexBuffer);
	if (hr != S_OK)
	{
		assert(0 && "CRenderMesh3D CTor Issues");
	}
}
CRenderMesh3D::CRenderMesh3D(std::vector<TVertexTangent> vtVerts, std::vector<unsigned int> vIndices)
{
	XMMATRIX matIdentity = XMMatrixIdentity();
	XMStoreFloat4x4(&m_tObject.f44WorldMatrix, matIdentity);
	m_vIndices = vIndices;
	CreateIndexBuffer();

	// Vertex Buffer Creation //
	HRESULT hr;

	D3D11_BUFFER_DESC d3dBufferDesc;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));

	d3dBufferData.pSysMem = &vtVerts[0];

	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.ByteWidth = sizeof(TVertexTangent);
	d3dBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

	hr = CRenderer::GetInstance().GetDevice()->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pVertexBuffer);
	if (hr != S_OK)
	{
		assert(0 && "CRenderMesh3D CTor Issues");
	}
}
CRenderMesh3D::~CRenderMesh3D()
{
//	SAFE_RELEASE(m_pIndexBuffer);
}
void CRenderMesh3D::Process()
{
	if (m_bIsActive == false)
		return;
	BindSRVsToPS();
	BindVertexBuffer();
	BindIndexBuffer();
	CRenderer::GetInstance().GetConstantBuffers()->MapToConstantBuffer(&m_tObject, sizeof(TObject), EConstantBuffer::eObjectBuffer);
	CRenderer::GetInstance().GetContext()->DrawIndexed(m_vIndices.size(), 0, 0);
}

ID3D11Buffer* CRenderMesh3D::GetIndexBuffer()
{
	return m_pIndexBuffer;
}
void CRenderMesh3D::SetIndexBuffer(ID3D11Buffer* pIndexBuffer)
{
	m_pIndexBuffer = pIndexBuffer;
}
std::vector<unsigned int>& CRenderMesh3D::GetIndices()
{
	return m_vIndices;
}
void CRenderMesh3D::UniformScale(float _fScale)
{
	XMFLOAT3 Pos = XMFLOAT3(m_tObject.f44WorldMatrix._41, m_tObject.f44WorldMatrix._42, m_tObject.f44WorldMatrix._43);
	m_tObject.f44WorldMatrix._41 = m_tObject.f44WorldMatrix._42 = m_tObject.f44WorldMatrix._43 = 0.0f;
	XMMATRIX Scaling = XMMatrixScaling(_fScale, _fScale, _fScale);
	XMMATRIX World = XMLoadFloat4x4(&m_tObject.f44WorldMatrix);
	World = XMMatrixMultiply(World, Scaling);
	XMStoreFloat4x4(&m_tObject.f44WorldMatrix, World);
	m_tObject.f44WorldMatrix._41 = Pos.x;
	m_tObject.f44WorldMatrix._42 = Pos.y;
	m_tObject.f44WorldMatrix._43 = Pos.z;
}
void CRenderMesh3D::SetPosition(float _fX, float _fY, float _fZ)
{
	m_tObject.f44WorldMatrix._41 = _fX;
	m_tObject.f44WorldMatrix._42 = _fY;
	m_tObject.f44WorldMatrix._43 = _fZ;
}
void CRenderMesh3D::SetPosition(XMFLOAT4 _Pos)
{
	this->SetPosition(_Pos.x, _Pos.y, _Pos.z);
}
void CRenderMesh3D::SetPosition(XMFLOAT3 _Pos)
{
	this->SetPosition(_Pos.x, _Pos.y, _Pos.z);
}
void CRenderMesh3D::RotateGlobalX(float _fRotation)
{
	XMFLOAT3 Pos = XMFLOAT3(m_tObject.f44WorldMatrix._41, m_tObject.f44WorldMatrix._42, m_tObject.f44WorldMatrix._43);
	m_tObject.f44WorldMatrix._41 = m_tObject.f44WorldMatrix._42 = m_tObject.f44WorldMatrix._43 = 0.0f;
	XMMATRIX Rotation = XMMatrixRotationX(RADIANS(_fRotation));
	XMMATRIX World = XMLoadFloat4x4(&m_tObject.f44WorldMatrix);
	World = XMMatrixMultiply(World, Rotation);
	XMStoreFloat4x4(&m_tObject.f44WorldMatrix, World);
	m_tObject.f44WorldMatrix._41 = Pos.x;
	m_tObject.f44WorldMatrix._42 = Pos.y;
	m_tObject.f44WorldMatrix._43 = Pos.z;

}
void CRenderMesh3D::RotateGlobalY(float _fRotation)
{
	XMFLOAT3 Pos = XMFLOAT3(m_tObject.f44WorldMatrix._41, m_tObject.f44WorldMatrix._42, m_tObject.f44WorldMatrix._43);
	m_tObject.f44WorldMatrix._41 = m_tObject.f44WorldMatrix._42 = m_tObject.f44WorldMatrix._43 = 0.0f;
	XMMATRIX Rotation = XMMatrixRotationY(RADIANS(_fRotation));
	XMMATRIX World = XMLoadFloat4x4(&m_tObject.f44WorldMatrix);
	World = XMMatrixMultiply(World, Rotation);
	XMStoreFloat4x4(&m_tObject.f44WorldMatrix, World);
	m_tObject.f44WorldMatrix._41 = Pos.x;
	m_tObject.f44WorldMatrix._42 = Pos.y;
	m_tObject.f44WorldMatrix._43 = Pos.z;
}
void CRenderMesh3D::RotateGlobalZ(float _fRotation)
{
	XMFLOAT3 Pos = XMFLOAT3(m_tObject.f44WorldMatrix._41, m_tObject.f44WorldMatrix._42, m_tObject.f44WorldMatrix._43);
	m_tObject.f44WorldMatrix._41 = m_tObject.f44WorldMatrix._42 = m_tObject.f44WorldMatrix._43 = 0.0f;
	XMMATRIX Rotation = XMMatrixRotationZ(RADIANS(_fRotation));
	XMMATRIX World = XMLoadFloat4x4(&m_tObject.f44WorldMatrix);
	World = XMMatrixMultiply(World, Rotation);
	XMStoreFloat4x4(&m_tObject.f44WorldMatrix, World);
	m_tObject.f44WorldMatrix._41 = Pos.x;
	m_tObject.f44WorldMatrix._42 = Pos.y;
	m_tObject.f44WorldMatrix._43 = Pos.z;
}